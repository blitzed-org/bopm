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
#include <time.h>

#include "irc.h"
#include "misc.h"
#include "stats.h"
#include "extern.h"

void do_stats_init()
{
   STAT_START_TIME = time(NULL);
   STAT_NUM_CONNECTS = 0;
}

void do_stats(const char *target)
{
   irc_send("PRIVMSG %s :Uptime: %s", target,
	    dissect_time(time(NULL) - STAT_START_TIME));
   irc_send("PRIVMSG %s :Found %u WinGates, %u open", target,
	    STAT_NUM_WINGATE, STAT_NUM_WINGATE_OPEN);
   irc_send("PRIVMSG %s :Found %u SOCKS4 servers, %u open", target,
	    STAT_NUM_SOCKS4, STAT_NUM_SOCKS4_OPEN);
   irc_send("PRIVMSG %s :Found %u SOCKS5 servers, %u open", target,
	    STAT_NUM_SOCKS5, STAT_NUM_SOCKS5_OPEN);
   irc_send("PRIVMSG %s :Found %u HTTP proxies, %u open", target,
	    STAT_NUM_HTTP, STAT_NUM_HTTP_OPEN);
   irc_send("PRIVMSG %s :Number of connects: %u", target, STAT_NUM_CONNECTS);
   return;
}
