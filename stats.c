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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <time.h>

#include "irc.h"
#include "misc.h"
#include "stats.h"
#include "extern.h"
#include "opercmd.h"
#include "scan.h"
#include "stats.h"

time_t STAT_START_TIME;
int STAT_NUM_CONNECTS;

extern protocol_hash SCAN_PROTOCOLS[];
extern size_t SCAN_NUMPROTOCOLS;

void do_stats_init()
{
   STAT_START_TIME = time(NULL);
   STAT_NUM_CONNECTS = 0;
}

void do_stats(const char *target)
{
   int i;
 
   irc_send("PRIVMSG %s :Uptime: %s", target,
	    dissect_time(time(NULL) - STAT_START_TIME));


   for(i = 0; i < SCAN_NUMPROTOCOLS;i++)
      irc_send("PRIVMSG %s :Found %u [%s], %u open.", target,  
               SCAN_PROTOCOLS[i].stat_num, SCAN_PROTOCOLS[i].type,
               SCAN_PROTOCOLS[i].stat_numopen);
        
   irc_send("PRIVMSG %s :Number of connects: %u", target, STAT_NUM_CONNECTS);      

   return;
}
