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
#include <sys/time.h>

#include "config.h"
#include "irc.h"
#include "log.h"
#include "opercmd.h"
#include "scan.h"
#include "stats.h"
#include "dnsbl.h"
#include "extern.h"


struct scan_struct *CONNECTIONS = 0;  /* Linked list head for connections */

char SENDBUFF[513];
char RECVBUFF[513];



/*    Protocol Name, Port, Write Handler, Read Handler */ 

protocol_hash SCAN_PROTOCOLS[] = {

       {"HTTP"      , 8080, &(scan_w_squid),  &(scan_r_squid)   ,0 ,0 },
       {"HTTP"      , 3128, &(scan_w_squid),  &(scan_r_squid)   ,0 ,0 },
       {"HTTP"      ,   80, &(scan_w_squid),  &(scan_r_squid)   ,0 ,0 },
       {"Socks4"    , 1080, &(scan_w_socks4), &(scan_r_socks4)  ,0 ,0 },
       {"Socks5"    , 1080, &(scan_w_socks5), &(scan_r_socks5)  ,0 ,0 },
       {"Wingate"   ,   23, &(scan_w_wingate),&(scan_r_wingate) ,0 ,0 }

};

size_t SCAN_NUMPROTOCOLS;

void do_scan_init()
{
   SCAN_NUMPROTOCOLS = sizeof(SCAN_PROTOCOLS) / sizeof(protocol_hash);
}


void scan_memfail()
{
    log("SCAN -> Error allocating memory.");
    exit(1);
}




/* IRC client has receieved a +c notice from
 * the remote server. scan_connect is called
 * with the connecting IP, where we will begin
 * to establish the proxy testing */

void scan_connect(char *addr, char *irc_addr, char *irc_nick,
		  char *irc_user, int verbose, char *conn_notice)
{

      int i;                
      scan_struct *newconn; 
      struct sockaddr_in  SCAN_LOCAL; /* For local bind() */ 

      if(OPT_DEBUG)
	      log("SCAN -> checking user %s!%s@%s", irc_nick, irc_user, irc_addr);

      memset(&SCAN_LOCAL, 0, sizeof(struct sockaddr_in));

      /* Setup SCAN_LOCAL for local bind() */
      if(CONF_BINDSCAN)
        {      

               if(!inet_aton(CONF_BINDSCAN, &(SCAN_LOCAL.sin_addr)))
                   {
                       log("SCAN -> bind(): %s is an invalid address", CONF_BINDSCAN);
                       exit(1);
                   }

               SCAN_LOCAL.sin_family = AF_INET;
               SCAN_LOCAL.sin_port = 0;
        }

 
     /* Loop through the protocols creating a 
       * seperate connection struct for each 
       * port/protocol */

      for(i = 0; i < SCAN_NUMPROTOCOLS; i++)
        {
            newconn = malloc(sizeof(scan_struct));

            if(!newconn)
               scan_memfail();               

            newconn->addr = strdup(addr);
            newconn->irc_addr = strdup(irc_addr);
            newconn->irc_nick = strdup(irc_nick);
            newconn->irc_user = strdup(irc_user);
	    newconn->verbose = verbose;
                 
	    if(conn_notice)
	       newconn->conn_notice = strdup(conn_notice);
	    else
	       newconn->conn_notice = 0;

            newconn->protocol = &(SCAN_PROTOCOLS[i]); /* Give struct a link to information about the protocol
                                                         it will be handling. */

            memset(&(newconn->sockaddr), 0, sizeof(struct sockaddr_in));

            newconn->sockaddr.sin_family = AF_INET;                    /* Fill in sockaddr with information about remote host */
            newconn->sockaddr.sin_port = htons(newconn->protocol->port); 
            newconn->sockaddr.sin_addr.s_addr = inet_addr(addr);
      
            newconn->fd = socket(PF_INET, SOCK_STREAM, 0);             /* Request file descriptor for socket */

            if(newconn->fd == -1)                                      /* If error, free memory for this struct and continue */
              {
                 log("SCAN -> Error allocating file descriptor.");
                 free(newconn->addr);
                 free(newconn->irc_addr);
                 free(newconn->irc_user);
                 free(newconn->irc_nick);
		 if(newconn->conn_notice)
		    free(newconn->conn_notice);
                 free(newconn);
                 continue;
              }


            /* Bind to specific interface designated in conf file */
            if(CONF_BINDSCAN)
             {
                if(bind(newconn->fd, (struct sockaddr *)&SCAN_LOCAL, sizeof(struct sockaddr_in)) == -1)     
                  {     
                   
                      switch(errno)     
                        {     
                               case EACCES:     
                                 log("SCAN -> bind(): No access to bind to %s", CONF_BINDSCAN);     
                                 exit(1);     
                               default:     
                                 log("SCAN -> bind(): Error binding to %s", CONF_BINDSCAN);     
                                 exit(1);     
     
                        }     
     
     
                  }      

             }

            time(&(newconn->create_time));                               /* Log create time of connection for timeouts */
            newconn->state = STATE_ESTABLISHED;                          /* Connection is just established             */
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

    struct timeval scan_timeout;
    struct scan_struct *ss;
    struct scan_struct *tss;

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

    scan_timeout.tv_sec      = 0;  /* No timeout */
    scan_timeout.tv_usec     = 0;

    switch(select((highfd + 1), &r_fdset, &w_fdset, 0, &scan_timeout)) 
     {

        case -1:
           return;  /* error in select */
        case 0:
           break;
                        /* Pass pointer to connection to handler */
        default:
             for(ss = CONNECTIONS; ss; ss = ss->next)
               {
                  if(FD_ISSET(ss->fd, &r_fdset) && (ss->state == STATE_SENT))
                    {
                        if((*ss->protocol->r_handler)(ss)) /* If read returns true, flag socket for closed and kline*/
                         {
                           irc_kline(ss->irc_addr, ss->addr);

			   if(CONF_DNSBL_FROM && CONF_DNSBL_TO &&
			      CONF_SENDMAIL && !ss->verbose)
			    {
			      dnsbl_report(ss);
			    }

                           log("SCAN -> %s: %s!%s@%s (%d)", ss->protocol->type , ss->irc_nick, ss->irc_user, 
                                         ss->irc_addr, ss->protocol->port);

                           irc_send("PRIVMSG %s :%s (%d): OPEN PROXY -> "
				    "%s!%s@%s", CONF_CHANNELS,
				    ss->protocol->type, ss->protocol->port,
				    ss->irc_nick, ss->irc_user,
				    ss->irc_addr);
                           ss->protocol->stat_numopen++; /* Increase number OPEN (insecure) of this type */

                           ss->state = STATE_CLOSED;
                           
                           if(!ss->verbose)                     
                            {
                               for(tss = CONNECTIONS;tss;tss = tss->next)
                                {
                                 if(!strcmp(ss->irc_addr, tss->irc_addr))
                                     tss->state = STATE_CLOSED;
                                }
                            }               
                         }
                      else      
                          {
                            /* Read returned false, we discard the connection as a closed proxy
                             * to save CPU. */
                            if(ss->verbose)
                             {
                               irc_send("PRIVMSG %s :%s (%d): Negotiation "
					"to %s failed.", CONF_CHANNELS,
					ss->protocol->type,
					ss->protocol->port, ss->irc_addr);
		             }
                            ss->state = STATE_CLOSED; 
                          }
                     }

                  if(FD_ISSET(ss->fd, &w_fdset))
                    {                                   
                      if((*ss->protocol->w_handler)(ss)) /* If write returns true, flag STATE_SENT */  
                           ss->state = STATE_SENT;
                      ss->protocol->stat_num++;     /* Increase number attempted negotiated of this type */
                    }
                        
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
                         CONNECTIONS = ss->next;
                         free(ss->addr);
                         free(ss->irc_addr);
                         free(ss->irc_nick);
                         free(ss->irc_user);
			 if(ss->conn_notice)
			   free(ss->conn_notice);
                         free(ss);
                     }
                   else
                     {
                         lastss->next = ss->next;
                         free(ss->addr);
                         free(ss->irc_addr);
                         free(ss->irc_nick);
                         free(ss->irc_user);
			 if(ss->conn_notice)
			   free(ss->conn_notice);
                         free(ss);
                     }
                   break;
               }

             lastss = ss;
       }
 
}

/*  Alarm signaled, loop through connections and 
 *  remove any we don't need anymore.
 * 
 */

void scan_timer()
{

    scan_struct *ss;
    scan_struct *nextss;
 
    time_t present;
   
    time(&present);

    for(ss = CONNECTIONS;ss;)
      {
          if(((present - ss->create_time) >= 30) || (ss->state == STATE_CLOSED)) /* State closed or timed out, remove */ 
            {
                if(ss->verbose && (ss->state != STATE_CLOSED))
                 {
                         irc_send("PRIVMSG %s :%s (%d): Connection "
                                        "to %s timed out.", CONF_CHANNELS,
                                        ss->protocol->type,
                                        ss->protocol->port, ss->irc_addr);
                 }
 
                nextss = ss->next;
                scan_del(ss);
                ss = nextss;
                continue;
            }
   
          ss = ss->next;
      }
}



/* Functions for handling open http data 
 *
 *  Return 1 on success.
 *
 */

int scan_w_squid(struct scan_struct *ss)
{
   
    snprintf(SENDBUFF, 128, "CONNECT %s:%d HTTP/1.0\r\n\r\n", CONF_SCANIP, CONF_SCANPORT);
    send(ss->fd, SENDBUFF, strlen(SENDBUFF), 0);

    return 1;
}

int scan_r_squid(struct scan_struct *ss)
{

  int len;


  len = recv(ss->fd, RECVBUFF, 512, 0);

  if(len <= 0)
    return 0;

  RECVBUFF[len] = 0; /* Make sure data is \0 terminated */
 	
  
  if(!strncasecmp(RECVBUFF, "HTTP/1.0 200", 12) ||
     !strncasecmp(RECVBUFF, "HTTP/1.1 200", 12))   
        return 1;
   
   
  return 0;
}


/*  Functions for handling open socks4 data
 *
 *  Return 1 on success.
 */


/*  CONNECT request byte order for socks4
 *  
 *  		+----+----+----+----+----+----+----+----+----+----+....+----+
 *  		| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
 *  		+----+----+----+----+----+----+----+----+----+----+....+----+
 *   # of bytes:  1    1      2              4           variable       1
 *  						 
 *  VN = Version, CD = Command Code (1 is connect request)
 *
 */

int scan_w_socks4(struct scan_struct *ss)
{

     struct in_addr addr;
     unsigned long laddr;
     int len;
 
     if(inet_aton(CONF_SCANIP, &addr) == 0)
          log("SCAN -> scan_w_socks4 : %s is not a valid IP", CONF_SCANIP);
    
     laddr = htonl(addr.s_addr);
 
     len = snprintf(SENDBUFF, 512, "%c%c%c%c%c%c%c%c%c",  
                                        4,
                                        1,
                                        (((unsigned short) CONF_SCANPORT) >> 8) & 0xFF,
                                        (((unsigned short) CONF_SCANPORT) & 0xff),
                                        (char) (laddr >> 24) & 0xFF,
                                        (char) (laddr >> 16) & 0xFF,
                                        (char) (laddr >> 8) & 0xFF,
                                        (char) laddr & 0xFF,                   
                                        0); 

     send(ss->fd, SENDBUFF, len, 0);
     return 1;
}

/*  REPLY byte order for socks4 
 *
 * 		+----+----+----+----+----+----+----+----+
 * 		| VN | CD | DSTPORT |      DSTIP        |
 * 		+----+----+----+----+----+----+----+----+
 * # of bytes:	   1    1      2              4
 * 
 * 
 * VN is the version of the reply code and should be 0. CD is the result
 * code with one of the following values:
 *
 * 	90: request granted
 *      91: request rejected or failed
 *      92: request rejected becasue SOCKS server cannot connect to
 *          identd on the client
 *      93: request rejected because the client program and identd
 *          report different user-ids
 *
 * 				    		     						 
 *
 */

int scan_r_socks4(struct scan_struct *ss)
{

   int len;

   len = recv(ss->fd, RECVBUFF, 512, 0);

   if(len < 8)
       return 0;

   RECVBUFF[len] = 0; /* Make sure data is \0 terminated */
   
  
   if(RECVBUFF[0] == 0 && RECVBUFF[1] == 90)
       return 1; 

   return 0;
}


/*
 *   Functions for handling socks5 data
 *
 */


/*  Send version authentication selection message to socks5
 *
 *      +----+----------+----------+
 *      |VER | NMETHODS | METHODS  |
 *      +----+----------+----------+
 *      | 1  |    1     | 1 to 255 |
 *      +----+----------+----------+
 *                                                                                               
 *  VER always contains 5, for socks version 5
 *  Method 0 is 'No authentication required'
 *  
 */

int scan_w_socks5(struct scan_struct *ss)
{
   int len;

                              /* Version 5, 1 number of methods, 0 method (no auth) */
   len = snprintf(SENDBUFF, 512, "%c%c%c", 5, 1, 0);
   send(ss->fd, SENDBUFF, len, 0);

   return 1;
}

/*  Authentication selection reply message.
 *       +----+--------+
 *       |VER | METHOD |
 *       +----+--------+
 *       | 1  |   1    |
 *       +----+--------+
 *                                                                                                                              
 *   Version should always be 5, method should
 *   return back method selected by server for  
 *   authentication. Method 0 indicates no authentication
 *   which indicates an open proxy.
 */

int scan_r_socks5(struct scan_struct *ss)
{

   int len;

   len = recv(ss->fd, RECVBUFF, 512, 0);

   if(len <= 0)
      return 0;

   RECVBUFF[len] = 0; /* Make sure data is \0 terminated */

   /* Version is 5 and method is 0 (no auth) */
   if(RECVBUFF[0] == 5 && RECVBUFF[1] == 0)
      return 1;

   return 0;
}



/*
 *  Functions to handle wingates
 *
 *
 */

/*  Open wingates require no authentication, they
 *  will send a prompt when connect. No need to
 *  send any data.
 */

int scan_w_wingate(struct scan_struct *ss)
{
    return 1;
}


int scan_r_wingate(struct scan_struct *ss)
{
   int len;

   len = recv(ss->fd, RECVBUFF, 512, 0);

   if(len <= 0)
      return 0;

   RECVBUFF[len] = 0; /* Make sure data is \0 terminated */
   
   if(!strncasecmp(RECVBUFF, "WinGate>", 8) ||
      !strncasecmp(RECVBUFF, "Too many connected users - try again later", 42))
          return 1;

   return 0;
}

/* manually check a host for proxies */
void do_manual_check(struct command *c)
{
   struct hostent *he;
   char *ip;

   if(!(he = gethostbyname(c->param)))
    {
      switch(h_errno)
       {
         case HOST_NOT_FOUND:
	    irc_send("PRIVMSG %s :Host '%s' is unknown, dummy.",
		     c->target, c->param);
	    return;
	 case NO_ADDRESS:
	    irc_send("PRIVMSG %s :The specified name '%s' exists, but has "
		     "no address.", c->target, c->param);
	    return;
	 case NO_RECOVERY:
	    irc_send("PRIVMSG %s :An unrecoverable error occured "
		     "whilst resolving '%s'.", c->target, c->param);
	    return;
	 case TRY_AGAIN:
	    irc_send("PRIVMSG %s :A temporary error occurred on an "
		     "authoritative name server.", c->target);
	 default:
	    irc_send("PRIVMSG %s :Unknown error resolving '%s' (sorry!)",
		     c->target, c->param);
	    return;
       }
    }

   ip = inet_ntoa(*((struct in_addr *) he->h_addr));

   irc_send("PRIVMSG %s :Checking %s [%s] for open proxies at request of %s...",
            c->target, c->param, ip, c->nick);

   if(CONF_DNSBL_ZONE)
      dnsbl_check(ip, "*", "*", c->param);

   scan_connect(ip, c->param, "*", "*", 1, 0);    /* Scan using verbose */
                                           
}

