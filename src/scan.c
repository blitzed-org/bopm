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

/* Function declarations */

struct scan_struct *scan_create(char **, char *);
void scan_free(struct scan_struct *);

/* Callbacks for LIBOPM */
void scan_open_proxy(OPM_T *, OPM_REMOTE_T *, int, void *);
void scan_negotiation_failed(OPM_T *, OPM_REMOTE_T *, int, void *);
void scan_timeout(OPM_T *, OPM_REMOTE_T *, int, void *);
void scan_end(OPM_T *, OPM_REMOTE_T *, int, void *);
void scan_handle_error(OPM_T *, OPM_REMOTE_T *, int, void *);


/* scan_cycle
 *
 *    Perform scanner tasks.
 */

void scan_cycle()
{
   node_t *p;
   struct scanner_struct *scs;


   /* Cycle each scanner object */
   LIST_FOREACH(p, SCANNERS->head)
   {
      scs = (struct scanner_struct *) p->data;
      opm_cycle(scs->scanner);
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
   node_t *p, *p1, *p2, *p3, *node;

   struct UserConf *uc;
   struct ScannerConf *sc;
   struct ProtocolConf *pc;

   struct scanner_struct *scs;
   struct mask_struct    *ms;

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

      /* Setup configuration */
      opm_config(scs->scanner, OPM_CONFIG_FD_LIMIT, &(sc->fd));
      opm_config(scs->scanner, OPM_CONFIG_SCAN_IP, sc->target_ip);
      opm_config(scs->scanner, OPM_CONFIG_SCAN_PORT, &(sc->target_port));
      opm_config(scs->scanner, OPM_CONFIG_TIMEOUT, &(sc->timeout));
      opm_config(scs->scanner, OPM_CONFIG_MAX_READ, &(sc->max_read));
      opm_config(scs->scanner, OPM_CONFIG_TARGET_STRING, sc->target_string);


      /* Setup callbacks */
      opm_callback(scs->scanner, OPM_CALLBACK_OPENPROXY, &scan_open_proxy, NULL);
      opm_callback(scs->scanner, OPM_CALLBACK_NEGFAIL, &scan_negotiation_failed, NULL);
      opm_callback(scs->scanner, OPM_CALLBACK_TIMEOUT, &scan_timeout, NULL);
      opm_callback(scs->scanner, OPM_CALLBACK_END, &scan_end, NULL);
      opm_callback(scs->scanner, OPM_CALLBACK_ERROR, &scan_handle_error, NULL);


      /* Setup the protocols */
      LIST_FOREACH(p1, sc->protocols->head)
      {
         pc = (struct ProtocolConf *) p1->data;
         opm_addtype(scs->scanner, pc->type, pc->port);
      }
     
      node = node_create(scs);
      list_add(SCANNERS, node);
   }


   /* Link masks to scanners */
   LIST_FOREACH(p, UserItemList->head)
   {
      uc = (struct UserConf *) p->data;
      LIST_FOREACH(p1, uc->masks->head)
      {
         mask = (char *) p1->data;
         LIST_FOREACH(p2, uc->scanners->head)
         {
            scannername = (char *) p2->data;
            LIST_FOREACH(p3, SCANNERS->head)
            {
               scs = (struct scanner_struct *) p3->data;
               if(strcasecmp(scannername, scs->name) == 0)
               {
                  if(OPT_DEBUG)
                     log("SCAN -> Linking the mask [%s] to scanner [%s]", mask, scannername);

                  ms = (struct mask_struct *) MyMalloc(sizeof(struct mask_struct));
                  ms->mask = (char *) DupString(mask);
                  ms->scs = scs;

                  node = node_create(ms);
                  list_add(MASKS, node);
               }
            }            
         } 
      }
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
 
   node_t *p;
   struct scan_struct *ss;
   struct scanner_struct *scs;
   struct mask_struct *ms;
   
   /* Have to use MSGLENMAX here because it is unknown what the max size of username/hostname can be.
      Some ircds use really mad values for these */
   static char mask[MSGLENMAX];

   /* FIXME: Check negcache here before any scanning */
   

   /* create scan_struct */
   ss = scan_create(user, msg);

   /* Store ss in the remote struct, so that in callbacks we have ss */
   ss->remote->data = ss;

   /* Generate user mask */
   snprintf(mask, MSGLENMAX, "%s!%s@%s", ss->irc_nick, ss->irc_username, ss->irc_hostname);

   /* Add ss->remote to all matching scanners */
   LIST_FOREACH(p, MASKS->head)
   {
      ms = (struct mask_struct *) p->data;
      if(match(ms->mask, mask))
      {
         scs = ms->scs;
         if(OPT_DEBUG)
            log("SCAN -> Passing %s to scanner [%s]", mask, scs->name);

         ss->scans++;  /* Increase scan count */
         opm_scan(scs->scanner, ss->remote);
      }
   }
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
   switch(err)
   {
      case OPM_ERR_MAX_READ:
         break;
      case OPM_ERR_BIND:
         break;
      case OPM_ERR_NOFD:
         break;
      default:   /* Unknown Error! */
         break;
   }
}



char *scan_getprototype(int protocol)
{
   return "SOMETYPE";
}
