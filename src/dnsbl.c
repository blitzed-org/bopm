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


/*
 * Check an ip address for presence in a DNS (black|block)list.  All we need
 * to do is reverse the octets, append the BL zone and then do a
 * gethostbyname.  If that gives us an answer, then it is on the BL.
 */
int dnsbl_check(const char *addr, const char *irc_nick,
                const char *irc_user, char *irc_addr)
{
    return(0);
}

/*
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
