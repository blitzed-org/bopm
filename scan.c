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


struct scan_struct *CONNECTIONS = 0;  /* Linked list head for connections */

char SENDBUFF[513];
char RECVBUFF[513];


/*    Protocol Name, Port, Write Handler, Read Handler */ 

protocol_hash SCAN_PROTOCOLS[] = {

       {"OpenSquid", 8080, &(scan_w_squid), &(scan_r_squid) },
       {"OpenSquid", 3128, &(scan_w_squid), &(scan_r_squid) },
       {"OpenSquid",   80, &(scan_w_squid), &(scan_r_squid) }

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

void scan_connect(char *addr, char *irc_addr)
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

            newconn->addr = strdup(addr);
            newconn->irc_addr = strdup(irc_addr);
                 
            newconn->protocol = &(SCAN_PROTOCOLS[i]); /* Give struct a link to information about the protocol
                                                         it will be handling. */

            memset(&(newconn->sockaddr), 0, sizeof(struct sockaddr_in));

            newconn->sockaddr.sin_family = AF_INET;                       /* Fill in sockaddr with information about remote host */
            newconn->sockaddr.sin_port = htons(newconn->protocol->port); 
            newconn->sockaddr.sin_addr.s_addr = inet_addr(addr);
      
            newconn->fd = socket(PF_INET, SOCK_STREAM, 0);                /* Request file descriptor for socket */

            if(newconn->fd == -1)                                         /* If error, free memory for this struct and continue */
              {
                 log("SCAN -> Error allocating file descriptor.");
                 free(newconn->addr);
                 free(newconn);
                 continue;
              }

            time(&(newconn->create_time));                               /* Log create time of connection for timeouts */
            newconn->state = STATE_ESTABLISHED;                              /* Connection is just established             */
            scan_add(newconn);                                           /* Add struct to list of connections          */                                                             
            fcntl(newconn->fd, F_SETFL, O_NONBLOCK);                     /* Set socket non blocking                    */
            connect(newconn->fd, (struct sockaddr *) &(newconn->sockaddr), sizeof(newconn->sockaddr));  /* Connect !   */
        }    

}

/*  Pass one cycle to the proxy scanner so it can do neccessary functions 
 *  like testing for sockets to be written to and read from.
 */


void scan_cycle()
{
   scan_check();
}


/*  Test for sockets to be written/read to.
 *
 */

void scan_check()
{

    fd_set w_fdset;
    fd_set r_fdset;

    struct timeval timeout;
    struct scan_struct *ss;

    int highfd = 0;
   

    if(!CONNECTIONS)
       return;

    FD_ZERO(&w_fdset);
    FD_ZERO(&r_fdset);

    /* Add connections to appropriate sets */

    for(ss = CONNECTIONS; ss; ss = ss->next)
      {
          if(ss->state == STATE_ESTABLISHED)
            {
               if(ss->fd > highfd)    
                  highfd = ss->fd;

               FD_SET(ss->fd, &w_fdset);
               continue;
            }
         
          if(ss->state == STATE_SENT)
           {
              if(ss->fd > highfd)
                 highfd = ss->fd;
             
               FD_SET(ss->fd, &r_fdset);
           }
      }

    timeout.tv_sec      = 0;  /* No timeout */
    timeout.tv_usec     = 0;

    switch(select((highfd + 1), &r_fdset, &w_fdset, 0, &timeout)) 
     {

        case -1:
           return;  /* error in select */
        case 0:
           break;
                        /* Pass pointer to connection to handler */
        default:
             for(ss = CONNECTIONS; ss; ss = ss->next)
               {
                  if(FD_ISSET(ss->fd, &r_fdset))
                      (*ss->protocol->r_handler)(ss);
                  if(FD_ISSET(ss->fd, &w_fdset))                                                             
                      (*ss->protocol->w_handler)(ss);
                    
               }
     } 
}




/* Link struct to connection list 
 */

void scan_add(scan_struct *newconn)
{

       scan_struct *ss;

       /* Only item in list */
         
       if(!CONNECTIONS)
         {
            newconn->next = 0;
            CONNECTIONS = newconn;
         }
       else       /* Link to end of list */
        {
             for(ss = CONNECTIONS;ss;ss = ss->next)
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


/* Unlink struct from connection list and 
 * free its memory */

void scan_del(scan_struct *delconn)
{

     scan_struct *ss;
     scan_struct *lastss;


     close(delconn->fd);

     lastss = 0;

     for(ss = CONNECTIONS; ss; ss = ss->next)
       {
             if(ss == delconn)
               {     
                        /* Removing the head */
                   if(lastss == 0)
                     {
                         CONNECTIONS = 0;
                         free(ss->addr);
                         free(ss->irc_addr);
                         free(ss);
                     }
                   else
                     {
                         lastss->next = ss->next;
                         free(ss->addr);
                         free(ss->irc_addr);
                         free(ss);
                     }
                   break;
               }

             lastss = ss;
       }
 
}


/* Functions for handling open http data 
 *
 *  Return 1 on success.
 *
 */

int scan_w_squid(struct scan_struct *ss)
{
   
    snprintf(SENDBUFF, 128, "CONNECT %s:%d HTTP/1.0\n\n", CONF_SERVER, CONF_PORT);
    send(ss->fd, SENDBUFF, strlen(SENDBUFF), 0);

    ss->state = STATE_SENT;
    return 1;
}

int scan_r_squid(struct scan_struct *ss)
{

  int len;

  len = recv(ss->fd, RECVBUFF, 512, 0);
  RECVBUFF[len] = 0; /* Make sure data is \0 terminated */
	
  if(len <= 0)
       return 0;
 
  if(!strncasecmp(RECVBUFF, "HTTP/1.0", 8))
   { 
       irc_kline(ss->irc_addr, CONF_REASON);
       return 1;
   }

  return 0;
}

