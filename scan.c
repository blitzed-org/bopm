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
#include <fcntl.h>

#include "irc.h"
#include "log.h"
#include "config.h"
#include "scan.h"
#include "extern.h"


struct scan_struct *scans = 0;  /* Linked list head for connections */


                                /* Table hashes protocols to functions which
                                 * handle the specifics of those protocols */

protocol_hash SCAN_PROTOCOLS[] = {

       {"OpenSquid", 8080, scan_squid },
       {"OpenSquid", 3128, scan_squid },
       {"OpenSquid",   80, scan_squid }

};




void scan_memfail()
{
    log("SCAN -> Error allocating memory.");
    exit(1);
}




/* IRC client has receieved a +c notice from
 * the remote server. scan_connect is called
 * with the connecting IP, where we will begin
 * to establish the proxy testing */

void scan_connect(char *ip)
{

      int i;                
      scan_struct *newconn; 

      /* Loop through the protocols creating a 
       * seperate connection struct for each 
       * port/protocol */

      for(i = 0; i < sizeof(SCAN_PROTOCOLS) / sizeof(protocol_hash); i++)
        {
            newconn = malloc(sizeof(scan_struct));

            if(!newconn)
               scan_memfail();               

            newconn->addr = strdup(ip);                 
            newconn->protocol = &(SCAN_PROTOCOLS[i]); /* Give struct a link to information about the protocol
                                                         it will be handling. */

            memset(&(newconn->sockaddr), 0, sizeof(struct sockaddr_in));

            newconn->sockaddr.sin_family = AF_INET;                       /* Fill in sockaddr with information about remote host */
            newconn->sockaddr.sin_port = htons(newconn->protocol->port); 
            newconn->sockaddr.sin_addr.s_addr = inet_addr(ip);
      
            newconn->fd = socket(PF_INET, SOCK_STREAM, 0);                /* Request file descriptor for socket */

            if(newconn->fd == -1)                                         /* If error, free memory for this struct and continue */
              {
                 log("SCAN -> Error allocating file descriptor.");
                 free(newconn->addr);
                 free(newconn);
                 continue;
              }

            time(&(newconn->create_time));                               /* Log create time of connection for timeouts */

            scan_add(newconn);                                           /* Add struct to list of connections */                                                             
            fcntl(newconn->fd, F_SETFL, O_NONBLOCK);                     /* Set socket non blocking */
            connect(newconn->fd, (struct sockaddr *) &(newconn->sockaddr), sizeof(newconn->sockaddr));  /* Connect ! */
        }    

}


/* Link struct to connection list 
 */


void scan_add(scan_struct *newconn)
{

       scan_struct *ss;

       /* Only item in list */
         
       if(!scans)
         {
            newconn->next = 0;
            scans = newconn;
         }
       else       /* Link to end of list */
        {
             for(ss = scans;ss;ss = ss->next)
                {
                     if(!ss->next)
                       {
                          newconn->next = 0;
                          ss->next = newconn;
                          break;
                       }              
                }

        }
}



/* Functions used to SEND specific data to
 * operate possible proxies. */

int scan_squid(int fd)
{


      return 1;
}
