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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/resource.h> /* getrlimit */
#include <errno.h>
#include <fcntl.h>

#include <stdio.h>
#include <time.h>

#include "irc.h"
#include "misc.h"
#include "stats.h"
#include "config.h"
#include "extern.h"
#include "opercmd.h"
#include "scan.h"
#include "stats.h"
#include "libopm/src/opm_types.h"

static time_t STATS_UPTIME;
static unsigned int STATS_CONNECTIONS;
static unsigned int STATS_DNSBLRECV;
static unsigned int STATS_DNSBLSENT;

static struct StatsHash STATS_PROXIES[] =
   {
      {OPM_TYPE_HTTP,     0, "HTTP"     },
      {OPM_TYPE_HTTPPOST, 0, "HTTPPOST" },
      {OPM_TYPE_SOCKS4,   0, "SOCKS4"   },
      {OPM_TYPE_SOCKS5,   0, "SOCKS5"   },
      {OPM_TYPE_ROUTER,   0, "ROUTER"   },
      {OPM_TYPE_WINGATE,  0, "WINGATE"  }
   };


/* stats_init
 *
 *    Perform initialization of bopm stats 
 *
 * Parameters: NONE
 * Return: NONE
 * 
 */

void stats_init()
{
   time(&STATS_UPTIME);
}




/* stats_openproxy
 *
 *    Record open proxy.
 *
 *
 * Parameters: NONE
 * Return: NONE
 * 
 */

void stats_openproxy(int type)
{
   int i;

   for(i = 0; i < (sizeof(STATS_PROXIES) / sizeof(struct StatsHash)); i++)
      if(STATS_PROXIES[i].type == type)
         STATS_PROXIES[i].count++;
}



/* stats_connect
 *
 *    Record IRC connect.
 *
 *
 * Parameters: NONE
 * Return: NONE
 * 
 */


void stats_connect()
{
   STATS_CONNECTIONS++;
}



/* stats_dnsblrecv
 *
 *    Record that a user was found in the blacklist.
 *
 * Parameters: NONE
 * Return: NONE
 *
 */

void stats_dnsblrecv()
{
   STATS_DNSBLRECV++;
}




/* stats_dnsblsend
 *
 *    Record a sent report
 *
 * Parameters: NONE
 * Return: NONE
 *
 */

void stats_dnsblsend()
{
   STATS_DNSBLSENT++;
}




/* stats_output
 *
 *    Output stats to target via privmsg
 *
 *
 * Parameters: NONE
 * Return: NONE
 * 
 */

void stats_output(char *target)
{
   int i;
   time_t present;
   time_t uptime;

   time(&present);
   uptime = present - STATS_UPTIME;

   irc_send("PRIVMSG %s :Uptime: %s", target, dissect_time(uptime));

   if(STATS_DNSBLRECV > 0)
      irc_send("PRIVMSG %s :DNSBL: %u successful lookups from blacklists", target, STATS_DNSBLRECV);

   if(STATS_DNSBLSENT > 0)
      irc_send("PRIVMSG %s :DNSBL: %u reports sent", target, STATS_DNSBLSENT);

   for(i = 0; i < (sizeof(STATS_PROXIES) / sizeof(struct StatsHash)); i++)
      if(STATS_PROXIES[i].count > 0)
         irc_send("PRIVMSG %s :Found %u (%s) open.", target, STATS_PROXIES[i].count, STATS_PROXIES[i].name);

   irc_send("PRIVMSG %s :Number of connects: %u (%.2f/minute)",
            target, STATS_CONNECTIONS, STATS_CONNECTIONS ?
            (float)STATS_CONNECTIONS / ((float)uptime / 60.0) : 0.0);

}



/* fdstats_output
 *
 *    Output file descriptor stats to target via privmsg
 *
 *
 * Parameters: NONE
 * Return: NONE
 *
 */

void fdstats_output(char *target)
{
   unsigned total_fd_use;
   struct rlimit rlim;
   int i, ret;

   /* Get file descriptor ceiling */
   if(getrlimit(RLIMIT_NOFILE, &rlim) == -1)
   {
      log("FDSTAT -> getrlimit() error retrieving RLIMIT_NOFILE (%s)", strerror(errno));
      irc_send("PRIVMSG %s :FDSTAT -> getrlimit() error retrieving RLIMIT_NOFILE (%s)",
                target,  strerror(errno));
      return;
   }

   /* Check which file descriptors are active */
   total_fd_use = 0;
   for(i = 0; i < rlim.rlim_cur; i++)
   {
      ret = fcntl(i,F_GETFD,0);
      if((errno != EBADF) && (ret != -1))
         total_fd_use++;
   }

   irc_send("PRIVMSG %s :Total open FD: %u/%d", target, total_fd_use, rlim.rlim_cur);
}
