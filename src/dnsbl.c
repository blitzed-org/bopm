/*
Copyright (C) 2002  Erik Fears
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
 
      Foundation, Inc.
      59 Temple Place - Suite 330
      Boston, MA  02111-1307, USA.
 
*/

#include "setup.h"

#include <stdio.h>

#ifdef STDC_HEADERS
#include <stdlib.h>
#endif

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>

#include "compat.h"
#include "config.h"
#include "dnsbl.h"
#include "extern.h"
#include "list.h"
#include "log.h"
#include "scan.h"


/*
 * Work out the DNSBL zones and send the dns query
 */
void dnsbl_add(struct scan_struct *ss)
{
   struct in_addr in;
   unsigned char a, b, c, d;
   char lookup[DNSBL_LOOKUPLEN];
   node_t *p;
   int res;

   if (!inet_aton(ss->ip, &in))
   {
      log("DNSBL -> Invalid address '%s', ignoring.", ss->ip);
      return;
   }

   d = (unsigned char) (in.s_addr >> 24) & 0xFF;
   c = (unsigned char) (in.s_addr >> 16) & 0xFF;
   b = (unsigned char) (in.s_addr >> 8) & 0xFF;
   a = (unsigned char) in.s_addr & 0xFF;

   LIST_FOREACH(p, OpmItem->blacklists->head)
   {
#ifdef WORDS_BIGENDIAN
      snprintf(lookup, DNSBL_LOOKUPLEN, "%d.%d.%d.%d.%s", a, b, c, d,
               (char *) p->data);
#else
      snprintf(lookup, DNSBL_LOOKUPLEN, "%d.%d.%d.%d.%s", d, c, b, a,
               (char *) p->data);
#endif

      if(OPT_DEBUG)
         log("DNSBL -> Passed '%s' to resolver", lookup);

      res = firedns_getip4(lookup, (void *) ss);

      if(res == -1)
         log("DNSBL -> Error sending dns lookup  '%s'", lookup);
      else
         ss->scans++; /* Increase scan count - one for each blacklist */
   }
}

void dnsbl_log_positive(struct scan_struct *ss, char *lookup, unsigned char type)
{
   char text_type[100];

   text_type[0] = '\0';

   if(type & DNSBL_TYPE_WG)
      strcat(text_type, "Wingate, ");
   if(type & DNSBL_TYPE_SOCKS)
      strcat(text_type, "Socks, ");
   if(type & DNSBL_TYPE_HTTP)
      strcat(text_type, "HTTP, ");
   if(type & DNSBL_TYPE_CISCO)
      strcat(text_type, "Cisco, ");
   if(type & DNSBL_TYPE_HTTPPOST)
      strcat(text_type, "HTTP Post, ");

   if(text_type[0] != '\0')
      *(strrchr(text_type, ',')) = '\0';

   if(strlen(ss->ip) < strlen(lookup)) {
      lookup += strlen(ss->ip) + 1;
   }


   if(ss->manual_target == NULL)
   {
      log("DNSBL -> %s!%s@%s appears in BL zone %s (%s)", ss->irc_nick, ss->irc_username,
          ss->irc_hostname, lookup, text_type);
      irc_send_channels("DNSBL -> %s!%s@%s appears in BL zone %s (%s)", ss->irc_nick,
                        ss->irc_username, ss->irc_hostname, lookup, text_type);
   }
   else /* Manual scan */
      irc_send("PRIVMSG %s :CHECK -> DNSBL -> %s appears in BL zone %s (%s)",
               ss->manual_target->name, ss->ip, lookup, text_type);

   /* record stat */
   stats_dnsblrecv();
}

void dnsbl_result(struct firedns_result *res)
{
   struct scan_struct *ss;
   ss = (struct scan_struct *) res->info;

   if(OPT_DEBUG)
      log("DNSBL -> Lookup result for %s!%s@%s (%s) %d.%d.%d.%d (error: %d)",
          ss->irc_nick,
          ss->irc_username,
          ss->irc_hostname,
          res->lookup,
          (unsigned char)res->text[0],
          (unsigned char)res->text[1],
          (unsigned char)res->text[2],
          (unsigned char)res->text[3], fdns_errno);

   /* Everything is OK */
   if(res->text[0] == '\0' && fdns_errno == FDNS_ERR_NXDOMAIN)
   {
      if(ss->manual_target != NULL)
         irc_send("PRIVMSG %s :CHECK -> DNSBL -> %s does not appear in BL zone %s", 
                   ss->manual_target->name, ss->ip,
                   (strlen(ss->ip) < strlen(res->lookup)) ? (res->lookup + strlen(ss->ip) + 1) : res->lookup);


      ss->scans--;            /* we are done with ss here */
      scan_checkfinished(ss); /* this could free ss, don't use ss after this point */
      return;
   }
   /* Either an error, or a positive lookup */

   if(fdns_errno == FDNS_ERR_NONE)
   {
      /* Only report it if no other scans have found positives yet. */
      if(ss->manual_target == NULL)
         scan_positive(ss);
      dnsbl_log_positive(ss, res->lookup, (unsigned char)res->text[3]);
   }
   else
   {
      /* XXX: old bopm sometimes reports failures on these.. */
      log("DNSBL -> Lookup error on %s: %s", res->lookup,
	      firedns_strerror(fdns_errno));
   }

   /* Check if ss has any remaining scans */
   ss->scans--; /* We are done with ss here */
   scan_checkfinished(ss); /* this could free ss, don't use ss after this point */
}

void dnsbl_cycle(void)
{
   firedns_cycle();
}

/*
 * Send an email to report this open proxy.
 */

void dnsbl_report(struct scan_struct *ss)
{
   char buf[4096], cmdbuf[512];
   FILE *fp;

   if(ss->ip == NULL)
      return;

   if(strlen(OpmItem->dnsbl_to) == 0 || strlen(OpmItem->dnsbl_from) == 0 || strlen(OpmItem->sendmail) == 0)
      return;


   snprintf(cmdbuf, sizeof(cmdbuf), "%s -t", OpmItem->sendmail);
   snprintf(buf, sizeof(buf),
            "From: %s <%s>\n"
            "To: %s\n"
            "Subject: BOPM Report\n\n"
            "%s: %s:%d\n\n"
            "%s\n", IRCItem->nick, OpmItem->dnsbl_from, OpmItem->dnsbl_to,
            scan_gettype(ss->remote->protocol), ss->ip, ss->remote->port,
	    ss->proof);

   if(OPT_DEBUG >= 3)
      log("DNSBL -> Sending following email:\n%s\n", buf);

   if ((fp = popen(cmdbuf, "w")) == NULL)
   {
      log("DNSBL -> Failed to create pipe to '%s' for email report!", cmdbuf);
      irc_send_channels("I was trying to create a pipe to'%s' to send a DNSBL "
                        "report, and it failed! I'll give up for now.",
                        cmdbuf);
      return;
   }

   fputs(buf, fp);
   pclose(fp);

   log("DNSBL -> Sent report to %s [%s]", OpmItem->dnsbl_to, ss->ip);
   /* record send in stats */
   stats_dnsblsend();
}
