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
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "extern.h"
#include "irc.h"
#include "log.h"
#include "opercmd.h"
#include "scan.h"
#include "stats.h"
#include "options.h"
#include "version.h"

void do_alarm(int);
void do_int(int);

int ALARMED = 0;

int OPT_DEBUG = 0;
char *CONFNAME = DEFAULTNAME;

char *CONFFILE, *LOGFILE, *PIDFILE;

struct sigaction ALARMACTION;
struct sigaction INTACTION;

int main(int argc, char **argv)
{

   FILE *pidout;
   int pid, c, lenc, lenl, lenp;
   char spid[16];

   do_stats_init();
   do_scan_init();

   while(1)
    {
       c = getopt(argc, argv, "dc:");

       if(c == -1)
           break;

       switch(c)
        {
	   case 'c':
	       CONFNAME = strdup(optarg);
	       break;
           case 'd':
               OPT_DEBUG++;
               break;
           case '?':
           default:
               /* unknown arg, guess we'll just do nothing for now */
               break;
        }
    }	

   lenc = strlen(CONFNAME) + strlen(CONFEXT) + 2;
   lenl = strlen(CONFNAME) + strlen(LOGEXT) + 2;
   lenp = strlen(CONFNAME) + strlen(PIDEXT) + 2;

   CONFFILE = (char *) malloc(lenc * sizeof(char));
   LOGFILE = (char *) malloc(lenl * sizeof(char));
   PIDFILE = (char *) malloc(lenp * sizeof(char));

   snprintf(CONFFILE, lenc, "%s.%s", CONFNAME, CONFEXT);
   snprintf(LOGFILE, lenl, "%s.%s", CONFNAME, LOGEXT);
   snprintf(PIDFILE, lenp, "%s.%s", CONFNAME, PIDEXT);

   /* Fork off */

   if(!OPT_DEBUG)
    {
       if((pid = fork()) < 0)
        {
	   perror("fork()");
	   exit(1);
	}
       else if(pid != 0)
        {
           pidout = fopen(PIDFILE, "w");
           snprintf(spid, 16, "%d", pid);

           if(pidout)         
                fwrite(spid, sizeof(char), strlen(spid), pidout);   
         
           fclose(pidout);

           exit(0);
        }

       /* get us in our own process group */
       if(setpgid(0, 0) < 0)
        {
	   perror("setpgid()");
	   exit(1);
	}

       /* reset file mode */
       umask(0);

       /* close file descriptors */
       close(STDIN_FILENO);
       close(STDOUT_FILENO);
       close(STDERR_FILENO);

       log_open(LOGFILE); 
    }
   else
    {
       log("MAIN -> Debug level %d", OPT_DEBUG);
    }

    log("MAIN -> BOPM %s started.", VERSION);
    log("MAIN -> Reading configuration file...");

    config_load(CONFFILE);

    /* Setup alarm & int handlers */
 
    ALARMACTION.sa_handler = &(do_alarm);  
    ALARMACTION.sa_flags = SA_RESTART;
    INTACTION.sa_handler = &(do_int);
    
    sigaction(SIGALRM, &ALARMACTION, 0);
    sigaction(SIGINT, &INTACTION, 0);

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
    
    if(!OPT_DEBUG)
        log_close();
    return 0;
}


void do_alarm(int notused)
{
   ALARMED = 1;
   alarm(1);
}

void do_int(int notused)
{
   log("MAIN -> Caught SIGINT, bye!");
   exit(0);
}
