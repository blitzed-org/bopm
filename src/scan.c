/* vim: set shiftwidth=3 softtabstop=3 expandtab: */ 

/*
 * Copyright (C) 2002  Erik Fears
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to
 *
 *       The Free Software Foundation, Inc.
 *       59 Temple Place - Suite 330
 *       Boston, MA  02111-1307, USA.
 *
 *
 */

#include "setup.h"

#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#ifdef STDC_HEADERS
# include <stdlib.h>
# include <string.h>
#endif

#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include <errno.h>
#include <fcntl.h>

#ifdef HAVE_SYS_POLL_H
# include <sys/poll.h>
#endif

#include "config.h"
#include "irc.h"
#include "log.h"
#include "opercmd.h"
#include "scan.h"
#include "stats.h"
#include "dnsbl.h"
#include "extern.h"
#include "options.h"
#include "negcache.h"
#include "malloc.h"
#include "match.h"

/* Libopm */

#include "libopm/src/opm.h"
#include "libopm/src/opm_common.h"
#include "libopm/src/opm_error.h"
#include "libopm/src/opm_types.h"


/* GLOBAL LISTS */

static list_t *SCANNERS = NULL;   /* List of OPM_T */
static list_t *MASKS    = NULL;   /* Associative list of masks->scanners */


/* Negative Cache */
struct cnode *nc_head;


/* Function declarations */

struct scan_struct *scan_create(char **, char *);
void scan_free(struct scan_struct *);
void scan_irckline(struct scan_struct *);
void scan_positive(struct scan_struct *);
void scan_negative(struct scan_struct *);
static void scan_log(OPM_REMOTE_T *);

/** Callbacks for LIBOPM */
void scan_open_proxy(OPM_T *, OPM_REMOTE_T *, int, void *);
void scan_negotiation_failed(OPM_T *, OPM_REMOTE_T *, int, void *);
void scan_timeout(OPM_T *, OPM_REMOTE_T *, int, void *);
void scan_end(OPM_T *, OPM_REMOTE_T *, int, void *);
void scan_handle_error(OPM_T *, OPM_REMOTE_T *, int, void *);

extern FILE *scanlogfile;

/* scan_cycle
 *
 *    Perform scanner tasks.
 */

void scan_cycle()
{
   node_t *p;
   struct scanner_struct *scs;

   /* Cycle through the blacklist first.. */
   dnsbl_cycle();

   /* Cycle each scanner object */
   LIST_FOREACH(p, SCANNERS->head)
   {
      scs = (struct scanner_struct *) p->data;
      opm_cycle(scs->scanner);
   }
}




/* scan_timer
 *   
 *    Perform actions that are to be performed every ~1 second.
 *
 * Parameters: NONE
 * Return: NONE
 *
 */
void scan_timer()
{
   static int nc_counter;

   if (OptionsItem->negcache > 0)
   {
      if (nc_counter++ >= NEG_CACHE_REBUILD)
      {
         /*
          * Time to rebuild the negative
          * cache.
          */
         if(OPT_DEBUG)
            log("SCAN -> Rebuilding negative cache");

         negcache_rebuild();
         nc_counter = 0;
      }
   }
}



/* scan_init

      Initialize scanner and masks list based on configuration.

   Parameters:
      None
   
   Return:
      None
*/

void scan_init()
{
   node_t *p, *p2, *p3, *p4, *node;

   struct UserConf *uc;
   struct ScannerConf *sc;
   struct ProtocolConf *pc;
   struct scanner_struct *scs;

   char *mask;
   char *scannername;

   /* FIXME: If rehash code is ever added, cleanup would need done here. */

   SCANNERS = list_create();
   MASKS    = list_create();

   /* Setup each individual scanner */
   LIST_FOREACH(p, ScannerItemList->head)
   {
      sc = (struct ScannerConf *) p->data;
      scs = (struct scanner_struct *) MyMalloc(sizeof(struct scanner_struct));

      if(OPT_DEBUG)
         log("SCAN -> Setting up scanner [%s]", sc->name);

      /* Build the scanner */
      scs->scanner = opm_create();
      scs->name = (char *) DupString(sc->name);
      scs->masks = list_create();

      /* Setup configuration */
      opm_config(scs->scanner, OPM_CONFIG_FD_LIMIT, &(sc->fd));
      opm_config(scs->scanner, OPM_CONFIG_SCAN_IP, sc->target_ip);
      opm_config(scs->scanner, OPM_CONFIG_SCAN_PORT, &(sc->target_port));
      opm_config(scs->scanner, OPM_CONFIG_TIMEOUT, &(sc->timeout));
      opm_config(scs->scanner, OPM_CONFIG_MAX_READ, &(sc->max_read));
      opm_config(scs->scanner, OPM_CONFIG_BIND_IP, sc->vhost);

      /* add target strings */
      LIST_FOREACH(p2, sc->target_string->head)
      opm_config(scs->scanner, OPM_CONFIG_TARGET_STRING, (char *) p2->data);

      /* Setup callbacks */
      opm_callback(scs->scanner, OPM_CALLBACK_OPENPROXY, &scan_open_proxy, scs);
      opm_callback(scs->scanner, OPM_CALLBACK_NEGFAIL, &scan_negotiation_failed, scs);
      opm_callback(scs->scanner, OPM_CALLBACK_TIMEOUT, &scan_timeout, scs);
      opm_callback(scs->scanner, OPM_CALLBACK_END, &scan_end, scs);
      opm_callback(scs->scanner, OPM_CALLBACK_ERROR, &scan_handle_error, scs);


      /* Setup the protocols */
      LIST_FOREACH(p2, sc->protocols->head)
      {
         pc = (struct ProtocolConf *) p2->data;

         if(OPT_DEBUG >= 2)
            log("SCAN -> Adding protocol %s:%d to scanner [%s]", scan_gettype(pc->type), pc->port,
                scs->name);

         if(opm_addtype(scs->scanner, pc->type, pc->port) == OPM_ERR_BADPROTOCOL)
            log("SCAN -> Error bad protocol %s:%d in scanner [%s]", scan_gettype(pc->type), pc->port,
                scs->name);
      }

      node = node_create(scs);
      list_add(SCANNERS, node);
   }


   /* Give scanners a list of masks they scan */
   LIST_FOREACH(p, SCANNERS->head)
   {
      scs = (struct scanner_struct *) p->data;

      LIST_FOREACH(p2, UserItemList->head)
      {
         uc = (struct UserConf *) p2->data;
         LIST_FOREACH(p3, uc->scanners->head)
         {
            scannername = (char *) p3->data;
            /* Add all these masks to scanner */
            if(strcasecmp(scannername, scs->name) == 0)
            {
               LIST_FOREACH(p4, uc->masks->head)
               {
                  mask = (char *) p4->data;

                  if(OPT_DEBUG)
                     log("SCAN -> Linking the mask [%s] to scanner [%s]", mask, scannername);

                  node = node_create(DupString(mask));
                  list_add(scs->masks, node);
               }
               break;
            }
         }
      }
   }

   /* Initialise negative cache */
   if (OptionsItem->negcache > 0)
   {
      if(OPT_DEBUG >= 2)
         log("SCAN -> Initializing negative cache");
      nc_init(&nc_head);
   }
}


/* scan_connect
 *
 *    scan_connect is called when m_notice (irc.c) matches a connection
 *    notice and parses the connecting user out of it.
 *
 * Parameters:
 *    user: Parsed items from the connection notice:
 *          user[0] = connecting users nickname
 *          user[1] = connecting users username
 *          user[2] = connecting users hostname
 *          user[3] = connecting users IP
 *          msg     = Original connect notice
 * Return: NONE
 *
 */

void scan_connect(char **user, char *msg)
{

   struct bopm_sockaddr ip;

   node_t *p, *p2;
   struct scan_struct *ss;
   struct scanner_struct *scs;
   char *scsmask;
   int ret;

   /* Have to use MSGLENMAX here because it is unknown what the max size of username/hostname can be.
      Some ircds use really mad values for these */
   static char mask[MSGLENMAX];
   static char ipmask[MSGLENMAX];

   /* Check negcache before anything */
   if(OptionsItem->negcache > 0)
   {
      if (!inet_pton(AF_INET, user[3], &(ip.sa4.sin_addr)))
      {
         log("SCAN -> Invalid IPv4 address '%s'!", user[3]);
         return;
      }
      else
      {
         if(check_neg_cache(ip.sa4.sin_addr.s_addr) != NULL)
         {
            if(OPT_DEBUG)
               log("SCAN -> %s!%s@%s (%s) is negatively cached. Skipping all tests.",
                   user[0], user[1], user[2], user[3]);
            return;
         }
      }
   }

   /* Generate user mask */
   snprintf(mask, MSGLENMAX, "%s!%s@%s", user[0], user[1], user[2]);
   snprintf(ipmask, MSGLENMAX, "%s!%s@%s", user[0], user[1], user[3]);

   /* Check exempt list now that we have a mask */
   if(scan_checkexempt(mask, ipmask))
   {
      if(OPT_DEBUG)
         log("SCAN -> %s is exempt from scanning", mask);
      return;
   }

   /* create scan_struct */
   ss = scan_create(user, msg);

   /* Store ss in the remote struct, so that in callbacks we have ss */
   ss->remote->data = ss;

   /* Start checking our DNSBLs */
   if(LIST_SIZE(OpmItem->blacklists) > 0)
      dnsbl_add(ss);

   /* Add ss->remote to all matching scanners */
   LIST_FOREACH(p, SCANNERS->head)
   {
      scs = (struct scanner_struct *) p->data;
      LIST_FOREACH(p2, scs->masks->head)
      {
         scsmask = (char *) p2->data;
         if(match(scsmask, mask))
         {
            if(OPT_DEBUG)
               log("SCAN -> Passing %s to scanner [%s]", mask, scs->name);

            if((ret = opm_scan(scs->scanner, ss->remote)) != OPM_SUCCESS)
            {
               switch(ret)
               {
                  case OPM_ERR_NOPROTOCOLS:
                     continue;
                     break;
                  case OPM_ERR_BADADDR:
                     log("OPM -> Bad address %s.", ss->manual_target->name, ss->ip);
                     break;
                  default:
                     log("OPM -> Unknown error %s.", ss->manual_target->name, ss->ip);
                     break;
               }
            }
            else
               ss->scans++; /* Increase scan count only if OPM_SUCCESS */

            break; /* Continue to next scanner */
         }
      }
   }

   /* All scanners returned !OPM_SUCCESS and there were no dnsbl checks */
   if(ss->scans == 0)
      scan_free(ss);
}




/* scan_create
 *
 *    Allocate scan struct, including user information and REMOTE 
 *    for LIBOPM.
 *
 * Parameters:
 *    user: Parsed items from the connection notice:
 *          user[0] = connecting users nickname
 *          user[1] = connecting users username
 *          user[2] = connecting users hostname
 *          user[3] = connecting users IP
 *          msg     = Original connect notice (used as PROOF)
 *
 * Return: Pointer to new scan_struct
 *
 */

struct scan_struct *scan_create(char **user, char *msg)
{
   struct scan_struct *ss;

   ss = (struct scan_struct *) MyMalloc(sizeof(struct scan_struct));

   ss->irc_nick = (char *) DupString(user[0]);
   ss->irc_username = (char *) DupString(user[1]);
   ss->irc_hostname = (char *) DupString(user[2]);
   ss->ip = (char *) DupString(user[3]);
   ss->proof = (char *) DupString(msg);

   ss->remote = opm_remote_create(ss->ip);
   ss->scans = 0;
   ss->positive = 0;

   ss->manual_target = NULL;

   assert(ss->remote);
   return ss;
}




/* scan_free
 *
 *    Free a scan_struct. This should only be done if the scan struct has no scans left!
 *
 * Parameters:
 *    ss: scan_struct to free
 * 
 * Return: NONE
 *
 */

void scan_free(struct scan_struct *ss)
{
   if(ss == NULL)
      return;

   MyFree(ss->irc_nick);
   MyFree(ss->irc_username);
   MyFree(ss->irc_hostname);
   MyFree(ss->ip);
   MyFree(ss->proof);

   opm_remote_free(ss->remote);
   MyFree(ss);
}



/* scan_open_proxy CALLBACK
 *
 *    Called by libopm when a proxy is verified open.
 *
 * Parameters:
 *    scanner: Scanner that found the open proxy.
 *    remote: Remote struct containing information regarding remote end
 *
 * Return: NONE
 * 
 */

void scan_open_proxy(OPM_T *scanner, OPM_REMOTE_T *remote, int notused, void *data)
{
   struct scan_struct *ss;
   struct scanner_struct *scs;

   /* Record that a scan happened */
   scan_log(remote);

   scs = (struct scanner_struct *) data;
   ss = (struct scan_struct *) remote->data;

   if(ss->manual_target == NULL)
   {
      scan_positive(ss);
      /* Report to blacklist */
      dnsbl_report(ss);

      irc_send_channels("OPEN PROXY -> %s!%s@%s %s:%d (%s) [%s]", ss->irc_nick, ss->irc_username, 
                         ss->irc_hostname, remote->ip, remote->port, scan_gettype(remote->protocol), 
                         scs->name);

      log("SCAN -> OPEN PROXY %s!%s@%s %s:%d (%s) [%s]", ss->irc_nick, ss->irc_username, ss->irc_hostname,
                        remote->ip, remote->port, scan_gettype(remote->protocol), scs->name);

   }
   else
   {
      irc_send("PRIVMSG %s :CHECK -> OPEN PROXY %s:%d (%s) [%s]", ss->manual_target->name, remote->ip, 
                remote->port, scan_gettype(remote->protocol), scs->name);

      log("SCAN -> OPEN PROXY %s:%d (%s) [%s]", remote->ip, remote->port, 
           scan_gettype(remote->protocol), scs->name);
   }


   /* Record the proxy for stats purposes */
   stats_openproxy(remote->protocol);
}




/* scan_negotiatoin_failed CALLBACK
 *
 *    Called by libopm when negotiation of a specific protocol failed.
 *
 * Parameters:
 *    scanner: Scanner where the negotiation failed.
 *    remote: Remote struct containing information regarding remote end
 *
 * Return: NONE
 *
 */

void scan_negotiation_failed(OPM_T *scanner, OPM_REMOTE_T *remote, int notused, void *data)
{
   struct scan_struct *ss;
   struct scanner_struct *scs;

   /* Record that a scan happened */
   scan_log(remote);

   scs = (struct scanner_struct *) data;
   ss = (struct scan_struct *) remote->data;

   if(OPT_DEBUG)
      log("SCAN -> Negotiation failed %s:%d (%s) [%s] (%d bytes read)", remote->ip, remote->port,
          scan_gettype(remote->protocol), scs->name, remote->bytes_read);

   if(ss->manual_target != NULL)
      irc_send("PRIVMSG %s :CHECK -> Negotiation failed %s:%d (%s) [%s] (%d bytes read)", 
                ss->manual_target->name, remote->ip, remote->port, scan_gettype(remote->protocol), 
                scs->name, remote->bytes_read);

}



/* scan_timeout CALLBACK
 *
 *    Called by libopm when the negotiation of a specific protocol timed out.
 *
 * Parameters:
 *    scanner: Scanner where the connection timed out.
 *    remote: Remote struct containing information regarding remote end
 *
 * Return: NONE
 *
 */

void scan_timeout(OPM_T *scanner, OPM_REMOTE_T *remote, int notused, void *data)
{
   struct scan_struct *ss;
   struct scanner_struct *scs;

   /* Record that a scan happened */
   scan_log(remote);

   scs = (struct scanner_struct *) data;
   ss = (struct scan_struct *) remote->data;

   if(OPT_DEBUG)
      log("SCAN -> Negotiation timed out %s:%d (%s) [%s] (%d bytes read)", remote->ip, remote->port,
          scan_gettype(remote->protocol), scs->name, remote->bytes_read);

   if(ss->manual_target != NULL)
      irc_send("PRIVMSG %s :CHECK -> Negotiation timed out %s:%d (%s) [%s] (%d bytes read)", 
                ss->manual_target->name, remote->ip, remote->port, scan_gettype(remote->protocol), 
                scs->name, remote->bytes_read);

}



/* scan_end CALLBACK
 *
 *    Called by libopm when a specific SCAN has completed (all protocols in that scan).
 *
 * Parameters:
 *    scanner: Scanner the scan ended on.
 *    remote: Remote struct containing information regarding remote end
 *
 * Return: NONE
 *
 */

void scan_end(OPM_T *scanner, OPM_REMOTE_T *remote, int notused, void *data)
{
   struct scan_struct *ss;
   struct scanner_struct *scs;

   scs = (struct scanner_struct *) data;
   ss = (struct scan_struct *) remote->data;

   if(OPT_DEBUG)
      log("SCAN -> Scan %s [%s] completed", remote->ip, scs->name);

   ss->scans--;
   scan_checkfinished(ss);
}




/* scan_handle_error CALLBACK
 *
 *    Called by libopm when an error occurs with a specific connection. This does not
 *    mean the entire scan has ended.
 *
 * Parameters:
 *    scanner: Scanner where the error occured.
 *    remote: Remote struct containing information regarding remote end
 *    err: OPM_ERROR code describing the error.
 *
 * Return: NONE
 *
 */

void scan_handle_error(OPM_T *scanner, OPM_REMOTE_T *remote, int err, void *data)
{

   struct scan_struct *ss;
   struct scanner_struct *scs;

   scs = (struct scanner_struct *) data;
   ss = (struct scan_struct *) remote->data;

   switch(err)
   {
      case OPM_ERR_MAX_READ:
         if(OPT_DEBUG >= 2)
            log("SCAN -> Max read on %s:%d (%s) [%s] (%d bytes read)", remote->ip, remote->port,
                scan_gettype(remote->protocol), scs->name, remote->bytes_read);
  
         if(ss->manual_target != NULL)
            irc_send("PRIVMSG %s :CHECK -> Negotiation failed %s:%d (%s) [%s] (%d bytes read)", 
                      ss->manual_target->name, remote->ip, remote->port, scan_gettype(remote->protocol), 
                      scs->name, remote->bytes_read);
         break;
      case OPM_ERR_BIND:
         log("SCAN -> Bind error on %s:%d (%s) [%s]", remote->ip, remote->port,
                scan_gettype(remote->protocol), scs->name);
         break;
      case OPM_ERR_NOFD:
         log("SCAN -> File descriptor allocation error %s:%d (%s) [%s]", remote->ip, remote->port,
                scan_gettype(remote->protocol), scs->name);

         irc_send("PRIVMSG %s :CHECK -> Scan failed %s:%d (%s) [%s] (file descriptor allocation error)",
                   ss->manual_target->name, remote->ip, remote->port, scan_gettype(remote->protocol),
                   scs->name);
         break;
      default:   /* Unknown Error! */
         if(OPT_DEBUG)
            log("SCAN -> Unknown error %s:%d (%s) [%s]", remote->ip, remote->port,
                scan_gettype(remote->protocol), scs->name);
         break;
   }
}


/* scan_gettype(int protocol)
 *
 *    Return human readable name of OPM PROTOCOL given OPM_TYPE_PROTOCOL
 *
 * Parameters:
 *    protocol: Protocol to return (from libopm/src/opm_types.h)
 *
 * Return:
 *    Pointer to static string containing human readable form of protocol
 *    name
 *
 */

char *scan_gettype(int protocol)
{
   int i;
   static char *undef = "undefined";

   static struct protocol_assoc protocols[] =
      {
         { OPM_TYPE_HTTP,     "HTTP"     },
         { OPM_TYPE_HTTPPOST, "HTTPPOST" },
         { OPM_TYPE_SOCKS4,   "SOCKS4"   },
         { OPM_TYPE_SOCKS5,   "SOCKS5"   },
         { OPM_TYPE_WINGATE,  "WINGATE"  },
         { OPM_TYPE_ROUTER,   "ROUTER"   }
      };

   for(i = 0; i < (sizeof(protocols) / sizeof(struct protocol_assoc)); i++)
      if(protocol == protocols[i].type)
         return protocols[i].name;

   return undef;
}



/* scan_positive
 *
 *    Remote host (defined by ss) has been found positive by one or more tests.
 *
 * Parameters:
 *    ss: scan_struct containing information regarding positive host 
 *
 * Return: NONE
 *
 */

void scan_positive(struct scan_struct *ss)
{
   node_t *node;

   OPM_T *scanner;

   /* Format KLINE and send to IRC server */
   scan_irckline(ss);

   /* Speed up the cleanup procedure */
   /* Close all scans prematurely */
   LIST_FOREACH(node, SCANNERS->head)
   {
      scanner = (OPM_T *) ((struct scanner_struct *) node->data)->scanner;
      opm_end(scanner, ss->remote);
   }

   /* Set it as a positive (to avoid a scan_negative call later on */
   ss->positive = 1;
}



/* scan_negative
 *
 *    Remote host (defined by ss) has passed all tests.
 *
 * Parameters:
 *    ss: scan_struct containing information regarding negative host.
 *
 * Return: NONE
 *
 */

void scan_negative(struct scan_struct *ss)
{
   /* Insert IP in negcache */
   if(OptionsItem->negcache > 0)
   {
      if(OPT_DEBUG >= 2)
         log("SCAN -> Adding %s to negative cache", ss->ip);
      negcache_insert(ss->ip);
   }
}


/* scan_irckline
 *
 *    ss has been found as a positive host and is to be klined. Format a kline message using IRCItem->kline
 *    as a format, then pass it to irc_send() to be sent to the remote server.
 *
 * Parameters:
 *    ss: scan_struct containing information regarding host to be klined
 * 
 * Return: NONE
 *
 */

void scan_irckline(struct scan_struct *ss)
{

   char *format;       /* INPUT  */
   char message[MSGLENMAX];  /* OUTPUT */

   unsigned short pos = 0;   /* position in format */
   unsigned short len = 0;   /* position in message */
   unsigned short size = 0;  /* temporary size buffer */

   int i;

   struct kline_format_assoc table[] =
      {
         {'i',   (void *) ss->ip,               FORMATTYPE_STRING },
         {'h',   (void *) ss->irc_hostname,     FORMATTYPE_STRING },
         {'u',   (void *) ss->irc_username,     FORMATTYPE_STRING },
         {'n',   (void *) ss->irc_nick,         FORMATTYPE_STRING }
      };

   format = IRCItem->kline;

   /* copy format to message character by character, inserting any matching data after % */
   while(format[pos] != '\0' && len < (MSGLENMAX - 2))
   {
      switch(format[pos])
      {

         case '%':
            /* % is the last char in the string, move on */
            if(format[pos + 1] == '\0')
               continue;

            /* %% escapes % and becomes % */
            if(format[pos + 1] == '%')
            {
               message[len++] = '%';
               pos++; /* skip past the escaped % */
               break;
            }

            /* Safe to check against table now */
            for(i = 0; i < (sizeof(table) / sizeof(struct kline_format_assoc)); i++)
            {
               if(table[i].key == format[pos + 1])
               {
                  switch(table[i].type)
                  {
                     case FORMATTYPE_STRING:

                        size = strlen( (char *) table[i].data);

                        /* Check if the new string can fit! */
                        if( (size + len) > (MSGLENMAX - 1) )
                           break;
                        else
                        {
                           strcat(message, (char *) table[i].data);
                           len += size;
                        }

                     default:
                        break;
                  }
               }
            }
            /* Skip key character */
            pos++;
            break;

         default:
            message[len++] = format[pos];
            message[len] = '\0';
            break;
      }
      /* continue to next character in format */
      pos++;
   }
   irc_send("%s", message);
}


/* scan_checkfinished
 *
 *   Check if a scan is complete (ss->scans <= 0)
 *   and free it if need be.
 *
 */

void scan_checkfinished(struct scan_struct *ss)
{
   if(ss->scans <= 0)
   {

      if(ss->manual_target != NULL)
         irc_send("PRIVMSG %s :CHECK -> All tests on %s completed.", ss->manual_target->name, ss->ip);
      else
      {
         if(OPT_DEBUG) /* If there was a manual_target, then irc_nick, etc is NULL */
            log("SCAN -> All tests on %s!%s@%s complete.", ss->irc_nick, ss->irc_username, ss->irc_hostname);

         /* Scan was a negative */
         if(!ss->positive)
            scan_negative(ss);
      }

      scan_free(ss);
   }
}



/* scan_manual
 *
 *    Create a manual scan. A manual scan is a scan where the
 *    scan_struct contains a manual_target pointer.
 */
void scan_manual(char *param, struct ChannelConf *target)
{
   struct hostent *he;
   struct scan_struct *ss;
   struct scanner_struct *scs;

   char *ip;
   char *scannername;

   node_t *p;
   int ret;

   /* If there were no parameters sent, simply alert the user and return */
   if(param == NULL)
   {
      irc_send("PRIVMSG %s :OPM -> Invalid parameters.", target->name);
      return;
   }

   /* Try to extract a scanner name from param, otherwise we'll be
      adding to all scanners */
   ip = param;

   if((scannername = strchr(param, ' ')) != NULL)
   {
      *scannername = '\0';
      scannername++;
   }

   ss = (struct scan_struct *) MyMalloc(sizeof(struct scan_struct));

   /* If IP is a hostname, resolve it using gethostbyname (which will block!) */
   if (!(he = gethostbyname(ip)))
   {
      switch (h_errno)
      {
         case HOST_NOT_FOUND:
            irc_send("PRIVMSG %s :CHECK -> Host '%s' is unknown.",
                      target->name, ip);
            return;
         case NO_ADDRESS:
            irc_send("PRIVMSG %s :CHECK -> The specified name '%s' exists, but has no address.",
                      target->name, ip);
            return;
         case NO_RECOVERY:
            irc_send("PRIVMSG %s :CHECK -> An unrecoverable error occured whilst resolving '%s'.",
                      target->name, ip);
            return;
         case TRY_AGAIN:
            irc_send("PRIVMSG %s :CHECK -> A temporary nameserver error occurred resolving '%s'.",
                      target->name, ip);
            return;
         default:
            irc_send("PRIVMSG %s :CHECK -> Unknown error resolving '%s' (sorry!)", target->name, ip);
            return;
      }
   }

   /* IP = the resolved IP now (it was the ip OR hostname before) */
   ip = inet_ntoa(*((struct in_addr *) he->h_addr));

   /* These don't exist in a manual scan */
   ss->irc_nick     = NULL;
   ss->irc_username = NULL;
   ss->irc_hostname = NULL;
   ss->proof        = NULL;

   ss->ip = DupString(ip);

   ss->remote = opm_remote_create(ss->ip);
   ss->remote->data = ss;
   ss->scans = 0;
   ss->positive = 0;

   ss->manual_target = target;

   assert(ss->remote);

   if(scannername != NULL)
      irc_send("PRIVMSG %s :CHECK -> Checking '%s' for open proxies [%s]", 
                target->name, ip,  scannername);
   else
      irc_send("PRIVMSG %s :CHECK -> Checking '%s' for open proxies on all scanners",
                target->name, ip);

   if(LIST_SIZE(OpmItem->blacklists) > 0)
      dnsbl_add(ss);

   /* Add ss->remote to all scanners */
   LIST_FOREACH(p, SCANNERS->head)
   {
      scs = (struct scanner_struct *) p->data;

      /* If we have a scannername, only allow that scanner
         to be used */
      if(scannername != NULL)
         if(strcasecmp(scannername, scs->name))
            continue;

      if(OPT_DEBUG)
         log("SCAN -> Passing %s to scanner [%s] (MANUAL SCAN)", ip, scs->name);

      if((ret = opm_scan(scs->scanner, ss->remote)) != OPM_SUCCESS)
      {
         switch(ret)
         {
            case OPM_ERR_NOPROTOCOLS:
               break;
            case OPM_ERR_BADADDR:
               irc_send("PRIVMSG %s :OPM -> Bad address %s [%s]", ss->manual_target->name, 
                         ss->ip, scs->name);
               break;
            default:
               irc_send("PRIVMSG %s :OPM -> Unknown error %s [%s]", ss->manual_target->name, 
                         ss->ip, scs->name);
               break;
         }
      }
      else
         ss->scans++; /* Increase scan count only if OPM_SUCCESS */
   }

   /* If all of the scanners gave !OPM_SUCCESS and there were no dnsbl checks, 
      cleanup here */
   if(ss->scans == 0)
   {
      if(scannername != NULL)
         irc_send("PRIVMSG %s :CHECK -> No such scanner '%s', or '%s' has 0 protocols.", 
                   ss->manual_target->name, scannername, scannername);

      irc_send("PRIVMSG %s :CHECK -> No scans active on '%s', aborting scan.", 
                ss->manual_target->name, ss->ip);
      scan_free(ss);
   }
}



/* scan_checkexempt
 * 
 *    Check mask against exempt list.
 * 
 * Parameters:
 *     mask: Mask to check
 * 
 * Return:
 *     1 if mask is in list
 *     0 if mask is not in list 
 */

int scan_checkexempt(char *mask, char *ipmask)
{
   node_t *node;
   char *exempt_mask;

   LIST_FOREACH(node, ExemptItem->masks->head)
   {
      exempt_mask = (char *) node->data;
      if(match(exempt_mask, mask) || match(exempt_mask, ipmask))
         return 1;
   }

   return 0;
}


/* scan_log
 *
 *    Log the fact that a given ip/port/protocol has just been scanned, if the
 *    user has asked for this to be logged.
 *
 * Parameters:
 *     remote: OPM_REMOTE_T for the remote end
 */

static void scan_log(OPM_REMOTE_T *remote)
{
   char buf_present[25];
   time_t present;
   struct tm *tm_present;
   struct scan_struct *ss = (struct scan_struct *) remote->data;

   if(!(OptionsItem->scanlog && scanlogfile))
      return;

   time(&present);
   tm_present = gmtime(&present);
   strftime(buf_present, sizeof(buf_present), "%b %d %H:%M:%S %Y", tm_present);

   fprintf(scanlogfile, "[%s] %s:%d (%s) \"%s\"\n", buf_present,
       remote->ip, remote->port, scan_gettype(remote->protocol), ss->proof);
   fflush(scanlogfile);
}
