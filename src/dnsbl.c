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

#include "irc.h"
#include "opercmd.h"
#include "scan.h"
#include "dnsbl.h"
#include "config.h"
#include "extern.h"
#include "log.h"

extern unsigned int STAT_DNSBL_MATCHES;

/*
 * Check an ip address for presence in a DNS (black|block)list.  All we need
 * to do is reverse the octets, append the BL zone and then do a
 * gethostbyname.  If that gives us an answer, then it is on the BL.
 */
int dnsbl_check(const char *addr, const char *irc_nick,
    const char *irc_user, char *irc_addr)
{
	size_t buflen;
	struct in_addr in;
	struct hostent *he;
	char *buf;
	char text_type[100];
	unsigned char a, b, c, d;
	int type;
  
	if (!inet_aton(addr, &in)) {
		log("DNSBL -> Invalid address '%s', ignoring.", addr);
		return(0);
	}

	d = (unsigned char) (in.s_addr >> 24) & 0xFF;
	c = (unsigned char) (in.s_addr >> 16) & 0xFF;
	b = (unsigned char) (in.s_addr >> 8) & 0xFF;
	a = (unsigned char) in.s_addr & 0xFF;

	/* Enough for a reversed IP and the zone. */
	buflen = 18 + strlen(CONF_DNSBL_ZONE);
	buf = malloc(buflen * sizeof(*buf));

#ifdef WORDS_BIGENDIAN
	snprintf(buf, buflen, "%d.%d.%d.%d.%s.", a, b, c, d,
	    CONF_DNSBL_ZONE);
#else
	snprintf(buf, buflen, "%d.%d.%d.%d.%s.", d, c, b, a,
	    CONF_DNSBL_ZONE);
#endif

	if (OPT_DEBUG)
		log("DNSBL -> Checking %s", buf);
   
	if (!(he = gethostbyname(buf))) {
		switch(h_errno) {
			/*
			 * Some of these errors indicate serious problems
			 * that will require admin intervention.
			 */
		case NO_RECOVERY:
			irc_send("PRIVMSG %s :Whilst checking '%s' "
			    "against my dnsbl zone '%s', I got a "
			    "non-recoverable name server error.  Might "
			    "want to take a look, check if your local "
			    "resolver is working!?", CONF_CHANNELS, addr,
			    CONF_DNSBL_ZONE);
			break;
		case TRY_AGAIN:
			irc_send("PRIVMSG %s :Whilst checking '%s' "
			    "against my dnsbl zone '%s', I got a "
			    "temporary nameserver error.  Might want to "
			    "take a look, check if your local resolver is "
			    "working!?", CONF_CHANNELS, addr,
			    CONF_DNSBL_ZONE);
			break;

		default:
			/*
			 * Some other error but we don't care, we can just
			 * treat this IP as OK.
			 */
	    		break;
		}

		free(buf);
		return(0);
	}
	free(buf);

	/*
	 * We got an answer, so we need to kline this IP now.
	 */
	irc_kline(irc_addr, (char *)addr);

	text_type[0] = '\0';
	type = (int)he->h_addr_list[0][3];

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
	
	log("DNSBL -> %s appears in BL zone %s type %s", addr, CONF_DNSBL_ZONE,
		text_type);
	irc_send("PRIVMSG %s :DNSBL: %s!%s@%s appears in BL zone %s type %s",
	    CONF_CHANNELS, irc_nick, irc_user, irc_addr, CONF_DNSBL_ZONE,
	    text_type);

	STAT_DNSBL_MATCHES++;
	return(1);
}

/*
 * Send an email to report this open proxy.
 */
void dnsbl_report(struct scan_struct *ss)
{
 	char buf[4096], cmdbuf[512];
	FILE *fp;

	if (!ss || !ss->addr)
		return;

	/* XXX: Don't report HTTP Post */
	if(strcmp(ss->protocol->type, "HTTP Post"))
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
}
