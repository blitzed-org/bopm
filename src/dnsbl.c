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

#include "config.h"
#include "dnsbl.h"
#include "extern.h"
#include "list.h"
#include "log.h"
#include "scan.h"


/* FIXME
 * Work out the DNSBL zones and send the dns query
 */
void dnsbl_add(struct scan_struct *ss)
{
   struct in_addr in;
   unsigned char a, b, c, d;
   char lookup[DNSBL_LOOKUPLEN];
   node_t *p;
   int res;

   if (!inet_aton(ss->ip, &in)) {
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
         log("DNSBL -> Passed '%s' to firedns", lookup);

      res = firedns_getip4(lookup, (void *) ss);

      if(res == -1)
         log("DNSBL -> Error sending dns lookup '%s'", lookup);
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

    log("DNSBL -> %s!%s@%s appears in BL zone %s (%s)", ss->irc_nick, ss->irc_username, ss->irc_hostname, lookup, text_type);

    irc_send_channels("DNSBL -> %s!%s@%s appears in BL zone %s (%s)", ss->irc_nick, ss->irc_username, ss->irc_hostname, lookup, text_type);
}

void dnsbl_result(struct firedns_result *res)
{
   struct scan_struct *ss;

   ss = (struct scan_struct *) res->info;
   ss->scans--; /* We will have finished with this when we return */

   if(OPT_DEBUG)
      log("DNSBL -> Lookup result: %s!%s@%s %d.%d.%d.%d (%d)",
          ss->irc_nick,
          ss->irc_username,
          ss->irc_hostname,
          (unsigned char)res->text[0],
          (unsigned char)res->text[1],
          (unsigned char)res->text[2],
          (unsigned char)res->text[3], fdns_errno);

   /* Everything is OK */
   if(res->text[0] == '\0' && fdns_errno == FDNS_ERR_NXDOMAIN)
      return;

   /* Either an error, or a positive lookup */

   if(fdns_errno == FDNS_ERR_NONE)
   {
      /* Only report it if no other scans have found positives yet */
      if(!ss->positive)
      {
         scan_positive(ss);
         dnsbl_log_positive(ss, res->lookup, (unsigned char)res->text[3]);
      }
   }
   else
   {
       // XXX: old bopm sometimes reports failures on these..
      log("DNSBL -> Weird error! fdns_errno = %d", fdns_errno);
   }
   
   /* Check if ss has any remaining scans */
   scan_checkfinished(ss);
}

void dnsbl_cycle(void)
{
   firedns_cycle();
}

/* FIXME
 * Send an email to report this open proxy.
 */
/*
void dnsbl_report(struct scan_struct *ss)
{
 	char buf[4096], cmdbuf[512];
	FILE *fp;
 
	if (!ss || !ss->addr)
		return;
 
	snprintf(cmdbuf, sizeof(cmdbuf), "%s -t", CONF_SENDMAIL);
	snprintf(buf, sizeof(buf),
	    "From: %s <%s>\n"
            "To: %s\n"
	    "Subject: BOPM Report\n\n"
	    "%s: %s\n\n"
	    "%s\n", CONF_NICK, CONF_DNSBL_FROM, CONF_DNSBL_TO,
	    ss->protocol->type, ss->addr, ss->conn_notice);
 
	if ((fp = popen(cmdbuf, "w")) == NULL) {
		log("DNSBL -> Failed to create pipe to '%s' for email "
		    "report!", cmdbuf);
		irc_send("PRIVMSG %s :I was trying to create a pipe to "
		    "'%s' to send a DNSBL report, and it failed!  I'll "
		    "give up for now.", CONF_CHANNELS, cmdbuf);
		return;
	}
 
	fputs(buf, fp);
	pclose(fp);
 
	log("DNSBL -> Sent report to %s", CONF_DNSBL_TO);
	STAT_DNSBL_REPORTS++;
} */
