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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "irc.h"
#include "config.h"
#include "extern.h"


/* Certain variables we don't want to allocate memory for over and over again
 * so global scope is given */


int                 IRC_FD = -1;      /* File descriptor for IRC client        */
struct sockaddr_in  IRC_SVR;          /* Sock Address Struct for IRC server    */
struct hostent     *IRC_HOST;         /* Hostent struct for IRC server         */
fd_set              IRC_FDSET;        /* fd_set for IRC data                   */

struct timeval      IRC_TIMEOUT;      /* timeval struct for select() timeout   */
                    

void irc_cycle()
{
      /* No socket open */
	
      if(IRC_FD == -1)
          irc_connect();

      IRC_TIMEOUT.tv_sec  = 0;
      IRC_TIMEOUT.tv_usec = 50000;   /* .5 seconds to avoid excessive CPU use on select() */
      
      //Poll for data using select
      
}

void irc_connect()
{
       memset(&IRC_SVR, 0, sizeof(IRC_SVR));

       /* Resolve IRC host */
       if(!(IRC_HOST = gethostbyname(CONF_SERVER)))
            return;    /* Generate ERROR here */


       IRC_SVR.sin_family      = AF_INET;
       IRC_SVR.sin_port        = CONF_PORT;
       IRC_SVR.sin_addr.s_addr = inet_addr(IRC_HOST->h_addr_list[0]);

       if(IRC_SVR.sin_addr.s_addr == INADDR_NONE)
            return;   /* Generate ERROR here */

       /* Request File Descriptor for Socket */

       IRC_FD = socket(PF_INET, SOCK_STREAM, 0);

       if(IRC_FD == -1)
            return;  /* Generate ERROR here */

       /* Connect to IRC Server */

       if(connect(IRC_FD, (struct sockaddr *) &IRC_SVR , sizeof(IRC_SVR)) == -1)
            return; /* Generate ERROR here */

       /* for debug use only, will be removed later */
       send(IRC_FD, "NICK Bopm\n",10,0);
       send(IRC_FD, "USER Bopm Bopm Bopm :Bopm\n",26,0);
}
