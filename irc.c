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

int IRC_FD = -1;              /* File descriptor for IRC client */
struct sockaddr_in IRC_SVR;


void irc_cycle()
{
      /* No socket open */
	
      if(IRC_FD == -1)
	{
             memset(&IRC_SVR, 0, sizeof(IRC_SVR));
	     
	     IRC_SVR.sin_family = AF_INET;
	     IRC_SVR.sin_port = CONF_PORT;
             IRC_SVR.sin_addr.s_addr = inet_addr(CONF_SERVER); 
             
	     if(IRC_SVR.sin_addr.s_addr == INADDR_NONE)
		  return;   /* Generate ERROR here */

             /* Request File Descriptor for Socket */
	     
	     IRC_FD = socket(PF_INET, SOCK_STREAM);

	     if(IRC_FD == -1)
		  return;  /* Generate ERROR here */

	     /* Connect to IRC Server */

	     if(connect(IRC_FD, &IRC_SVR, sizeof(IRC_SVR)) == -1)
		  return; /* Generate ERROR here */
	
	}
}


