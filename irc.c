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
#include <time.h>
#include <errno.h>

#include "irc.h"
#include "log.h"
#include "config.h"
#include "extern.h"


/* Certain variables we don't want to allocate memory for over and over again
 * so global scope is given */


char                IRC_RAW[513];     /* Buffer to read data into              */
char                IRC_SENDBUFF[513];/* Send buffer                           */
int                 IRC_RAW_LEN = 0;  /* Position of IRC_RAW                   */

int                 IRC_FD = -1;      /* File descriptor for IRC client        */
struct sockaddr_in  IRC_SVR;          /* Sock Address Struct for IRC server    */
struct hostent     *IRC_HOST;         /* Hostent struct for IRC server         */
fd_set              IRC_FDSET;        /* fd_set for IRC data for select()      */

struct timeval      IRC_TIMEOUT;      /* timeval struct for select() timeout   */




/* Give one cycle to the IRC client, which
 * will allow it to poll for data and handle
 * that data if need be.
 */

void irc_cycle()
{
   
	
      if(IRC_FD <= 0)                 /* No socket open */
          irc_connect();

      IRC_TIMEOUT.tv_sec  = 0;
      IRC_TIMEOUT.tv_usec = 500000;   /* block .5 seconds to avoid excessive CPU use on select() */
           
      FD_ZERO(&IRC_FDSET);
      FD_SET(IRC_FD, &IRC_FDSET);
      
      
                                
      switch(select((IRC_FD + 1), &IRC_FDSET, 0, 0, &IRC_TIMEOUT))
       {
            case -1:
		  return;    /* Generate ERROR here */
		  break;
            case 0:
		  break;
            default:
		   if(FD_ISSET(IRC_FD, &IRC_FDSET))     /* Check if IRC data is available */
		        irc_read();

        }
      
      
}


/* Create socket and connect to IRC server
 * specificied in config file (CONF_SERVER)
 * with port CONF_PORT
 */


void irc_connect()
{
       memset(&IRC_SVR, 0, sizeof(IRC_SVR));
   
       /* Resolve IRC host */
       if(!(IRC_HOST = gethostbyname(CONF_SERVER)))
          {
               switch(h_errno)
		{


                    case HOST_NOT_FOUND:
                              log("IRC -> gethostbyname(): The specified host (%s) is unknown", CONF_SERVER);
			      exit(1);
	            case NO_ADDRESS:
			      log("IRC -> gethostbyname(): The specified name (%s) exists, but does not have an IP", CONF_SERVER);
		              exit(1);
		    case NO_RECOVERY:
			      log("IRC -> gethostbyname(): An unrecoverable error occured resolving (%s)", CONF_SERVER);
			      exit(1);
                    case TRY_AGAIN:
			      log("IRC -> gethostbyname(): Temporary error occured with authoritive name server (%s)", CONF_SERVER);
			      exit(1);
                    default:
			      log("IRC -> gethostbyname(): Unknown error resolving (%s)", CONF_SERVER);

		}

          }

       IRC_SVR.sin_family      = AF_INET;
       IRC_SVR.sin_port        = htons(CONF_PORT);
       IRC_SVR.sin_addr = *((struct in_addr *) IRC_HOST->h_addr);

       if(IRC_SVR.sin_addr.s_addr == INADDR_NONE)
            return;   /* Generate ERROR here */

   
       IRC_FD = socket(PF_INET, SOCK_STREAM, 0);  /* Request file desc for IRC client socket */

       if(IRC_FD == -1)
            return;  /* Generate ERROR here */

      
                                                  /* Connect to IRC server as client */
       if(connect(IRC_FD, (struct sockaddr *) &IRC_SVR , sizeof(IRC_SVR)) == -1)
            return; /* Generate ERROR here */

       /* for debug use only, will be removed later */
       send(IRC_FD, "NICK Bopm\n",10,0);
       send(IRC_FD, "USER Bopm Bopm Bopm :Bopm\n",26,0);

}


/* Read one character at a time until an
 * endline is hit, at which time control
 * is passed to irc_parse() to parse that
 * line.
 */
 
void irc_read()
{
      char c;
      
      while(read(IRC_FD, &c, 1))
	{
	   
	   if(c == '\r')
	       continue;

	   if(c == '\n')       
	   {
	       IRC_RAW[IRC_RAW_LEN] = 0;  /* Null string */
	       irc_parse();               /* Parse Line */
	       IRC_RAW_LEN = 0;           /* Reset counter */
	       
	       continue;
	   }

	   if(c != '\r' && c != '\n' && c != 0)
               IRC_RAW[IRC_RAW_LEN++] = c;
        }
      
}

/* A full line has been read by irc_read();
 * this function begins parsing it.
 */

void irc_parse()
{

   /* This function will be rewritten with better
    * structure later on. 
    */

    char *cmd;   
    char *second; 

    printf("%s\n", IRC_RAW);      /* Will be removed */

    cmd = strtok(IRC_RAW, " ");   /* Parse First Token */
    second = strtok(NULL, " ");   /* Parse Second Token */

    if(!strcasecmp(cmd, "PING"))
       {
            snprintf(IRC_SENDBUFF, 512, "PONG %s\n", second);
            send(IRC_FD, IRC_SENDBUFF, 512, 0);
	    return;
       }

    if(!strcasecmp(second, "001"))
       {
            printf("Hit perform\n");
            do_perform();
       }  
    
}


void do_perform()
{
   
      struct perform_hash *pf;

      for(pf = CONF_PERFORM; pf; pf = pf->next)
       {       
           printf("SENDING %s\n", pf->perform);
           snprintf(IRC_SENDBUFF, 512, "%s\n", pf->perform);
           send(IRC_FD, IRC_SENDBUFF, 512, 0);
       }
}
