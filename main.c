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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>

#include "config.h"
#include "extern.h"
#include "irc.h"
#include "log.h"
#include "scan.h"

void do_alarm(int);

int ALARMED = 0;

struct sigaction ALARMACTION;

int main()
{

    log_open("bopm.log"); 

    log("MAIN -> BOPM started.");
    log("MAIN -> Reading configuration file...");

    config_load("bopm.conf");

    /* Setup alarm */
 
    ALARMACTION.sa_handler = &(do_alarm);  
    ALARMACTION.sa_flags = SA_RESTART;
 
    sigaction(SIGALRM, &ALARMACTION, 0);

    /* Ignore SIGPIPE */
    signal(SIGPIPE, SIG_IGN);

    alarm(1);

    while(1)     
     {
	irc_cycle();
        scan_cycle();

        if(ALARMED)
         {
            irc_timer();
            scan_timer();
            ALARMED = 0;
         }
     }
    
    log_close();
    return 0;
}


void do_alarm(int notused)
{
   ALARMED = 1;
   alarm(1);
}

