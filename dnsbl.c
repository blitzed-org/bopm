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

#include <stdio.h>
#include <stdlib.h>
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
#include "extern.h"
#include "log.h"

extern unsigned int STAT_DNSBL_MATCHES;

/* check an ip address for presence in a DNS (black|block)list.
 * All we need to do is reverse the octets, append the BL zone
 * and then do a gethostbyname.  If that gives us an answer, then
 * it is on the BL */
int dnsbl_check(const char *addr, const char *irc_nick,
		const char *irc_user, char *irc_addr)
{
   struct in_addr in;
   struct hostent *he;
   char *buf;
   int buflen, a, b, c, d;
  
   if(!inet_aton(addr, &in))
    {
      log("DNSBL -> Invalid address '%s', ignoring.", addr);
      return(0);
    }

   d = (int) (in.s_addr >> 24) & 0xFF;
   c = (int) (in.s_addr >> 16) & 0xFF;
   b = (int) (in.s_addr >> 8) & 0xFF;
   a = (int) in.s_addr & 0xFF;

   /* enough for a reversed IP and the zone */
   buflen = 17 + strlen(CONF_DNSBL_ZONE);
   buf = malloc(buflen * sizeof(char));

   snprintf(buf, buflen, "%d.%d.%d.%d.%s", d, c, b, a, CONF_DNSBL_ZONE);
  
   if(OPT_DEBUG)
      log("DNSBL -> Checking %s", buf);
   
   if(!(he = gethostbyname(buf)))
    {
      switch(h_errno)
       {
	 /* some of these errors indicate serious problems that will
	  * require admin intervention */
         case NO_RECOVERY:
            irc_send("PRIVMSG %s :Whilst checking '%s' against my dnsbl "
		     "zone '%s', I got a non-recoverable name server "
		     "error.  Might want to take a look!", CONF_CHANNELS,
		     addr, CONF_DNSBL_ZONE);
	    break;
	 case TRY_AGAIN:
	    irc_send("PRIVMSG %s :Whilst checking '%s' against my dnsbl "
		     "zone '%s', I got a temporary error on the "
		     "authoritative name server.  Might want to take a "
		     "look!", CONF_CHANNELS, addr, CONF_DNSBL_ZONE);
	    break;

	 default:
            /* some other error but we don't care, we can just treat this
	     * IP as OK */
	    break;
       }

      free(buf);
      return(0);
    }
   free(buf);

   /* we got an answer, so we need to kline this IP now */
   log("DNSBL -> %s appears in BL zone %s", addr, CONF_DNSBL_ZONE);
   irc_send("PRIVMSG %s :DNSBL: %s!%s@%s appears in BL zone %s",
	    CONF_CHANNELS, irc_nick, irc_user, irc_addr,
	    CONF_DNSBL_ZONE);
   irc_kline(irc_addr);
   STAT_DNSBL_MATCHES++;
   return(1);
}

/* send an email to report this open proxy */
void dnsbl_report(struct scan_struct *ss)
{
   log("Would be emailing now");
}
