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
#include <sys/time.h>
#include <errno.h>
#include <stdarg.h>

#include "irc.h"
#include "log.h"
#include "config.h"
#include "scan.h"
#include "extern.h"


/* Certain variables we don't want to allocate memory for over and over again
 * so global scope is given */


char                IRC_RAW[513];     /* Buffer to read data into              */
char                IRC_SENDBUFF[513];/* Send buffer                           */
int                 IRC_RAW_LEN = 0;  /* Position of IRC_RAW                   */

int                 IRC_FD = -1;      /* File descriptor for IRC client        */
struct sockaddr_in  IRC_SVR;          /* Sock Address Struct for IRC server    */
struct sockaddr_in  IRC_LOCAL;        /* Sock Address Struct for Bind          */
struct hostent     *IRC_HOST;         /* Hostent struct for IRC server         */
fd_set              IRC_FDSET;        /* fd_set for IRC data for select()      */

struct timeval      IRC_TIMEOUT;      /* timeval struct for select() timeout   */
time_t              IRC_NICKSERV_LAST = 0; /* Last notice from nickserv             */

/* Give one cycle to the IRC client, which
 * will allow it to poll for data and handle
 * that data if need be.
 */

void irc_cycle()
{
   
	
      if(IRC_FD <= 0)                 /* No socket open */
        {
          irc_init();                 /* Resolve remote host */
          irc_connect();              /* Connect to remote host */
        }

      IRC_TIMEOUT.tv_sec  = 0;
      IRC_TIMEOUT.tv_usec = 50000;   /* block .05 seconds to avoid excessive CPU use on select() */
           
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


/*  Allocate socket file descriptor for connection,
 *  and resolve remote host.
 *
 */

void irc_init()
{


       if(IRC_FD)
          close(IRC_FD);  


       memset(&IRC_SVR, 0, sizeof(IRC_SVR));
       memset(&IRC_LOCAL, 0, sizeof(IRC_LOCAL));

       /* Resolve IRC host */
       if(!(IRC_HOST = gethostbyname(CONF_SERVER)))
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
                              log("IRC -> gethostbyname(): Error occured with authoritive name server (%s)", CONF_SERVER);
                              exit(1);
                    default:
                              log("IRC -> gethostbyname(): Unknown error resolving (%s)", CONF_SERVER);
                              exit(1);
                }

       if(CONF_BINDIRC)
         {
                if(!inet_aton(CONF_BINDIRC, &(IRC_LOCAL.sin_addr)))
                   {
                       log("IRC -> bind(): %s is an invalid address", CONF_BINDIRC);
                       exit(1);
                   }

                IRC_LOCAL.sin_family = AF_INET;
                IRC_LOCAL.sin_port = 0;                                        
                            
                if(bind(IRC_FD, (struct sockaddr *)&IRC_LOCAL, sizeof(struct sockaddr_in)) == -1)
                  {      
              
                      switch(errno)
                        {
                               case EACCES:
                                 log("IRC -> bind(): No access to bind to %s", CONF_BINDIRC);
                                 exit(1);
                               
                               default:
                                 log("IRC -> bind(): Error binding to %s (%d)", CONF_BINDIRC, errno);
                                 exit(1);
    
                        }


                  }
      
         }



       IRC_SVR.sin_family      = AF_INET;
       IRC_SVR.sin_port        = htons(CONF_PORT);
       IRC_SVR.sin_addr = *((struct in_addr *) IRC_HOST->h_addr);

       if(IRC_SVR.sin_addr.s_addr == INADDR_NONE)
          {
               log("IRC -> Unknown error resolving remote host (%s)", CONF_SERVER);
               exit(1);
          }


       IRC_FD = socket(PF_INET, SOCK_STREAM, 0);  /* Request file desc for IRC client socket */



       if(IRC_FD == -1)
              switch(errno)
                {
                   case EINVAL:
                   case EPROTONOSUPPORT:
                               log("IRC -> socket(): SOCK_STREAM is not supported on this domain");
                               exit(1);
                   case ENFILE:
                               log("IRC -> socket(): Not enough free file descriptors to allocate IRC socket");
                               exit(1);
                   case EMFILE:
                               log("IRC -> socket(): Process table overflow when requesting file descriptor");
                               exit(1);
                   case EACCES:
                               log("IRC -> socket(): Permission denied to create socket of type SOCK_STREAM");
                               exit(1);
                   case ENOMEM:
                               log("IRC -> socket(): Insufficient memory to allocate socket");
                               exit(1);
                   default:
                               log("IRC -> socket(): Unknown error allocating socket");
                               exit(1);

                }


}


/* Send data to remote IRC host
 */


void irc_send(char *data,...)
{

  va_list         arglist;
  char            data2[513];
  char            tosend[513];
 
  va_start(arglist, data);
  vsprintf(data2, data, arglist);
  va_end(arglist);

  snprintf(tosend, (strlen(data2) + 2), "%s\n",data2);

  if(send(IRC_FD, tosend, strlen(tosend), 0) == -1) /* Return of -1 indicates error sending data; we reconnect. */
   {
       irc_init();    /* Rerequest IRC socket */         
       irc_connect(); /* Reconnect to IRC host */
   }

}

/*   kline given ip for given reason
 * 
 */

void irc_kline(char *addr)
{
     irc_send(CONF_KLINE_COMMAND, addr);
}

/* Create socket and connect to IRC server
 * specificied in config file (CONF_SERVER)
 * with port CONF_PORT
 */


void irc_connect()
{
                                                  /* Connect to IRC server as client */
       if(connect(IRC_FD, (struct sockaddr *) &IRC_SVR , sizeof(IRC_SVR)) == -1)
              switch(errno)
		{
                                 
                   case EISCONN:       
			   return;  /* Already connected */
                   case ECONNREFUSED:
			   log("IRC -> connect(): Connection refused by (%s)", CONF_SERVER);
                           exit(1);
                   case ETIMEDOUT:
			   log("IRC -> connect(): Timed out connecting to (%s)", CONF_SERVER);
                           exit(1);
                   case ENETUNREACH:
			   log("IRC -> connect(): Network unreachable");
			   exit(1);
	           case EALREADY:
			   return;  /* Previous attempt not complete */
	           default:
			   log("IRC -> connect(): Unknown error connecting to (%s)", CONF_SERVER);

	       }	

       /* for debug use only, will be removed later */
       irc_send("NICK %s",CONF_NICK);
       irc_send("USER %s %s %s :Blitzed Open Proxy Monitor", CONF_USER, CONF_USER, CONF_USER);

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
	       
	       break;
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

   char *addr;           /* IP of remote host in connection notices */
   char *irc_addr;       /* IRC host address of the remote host     */
   char *irc_user;     
   char *irc_nick;
   char *irc_channel; 

   char *token[16];
   int   tokens = 0;
   int   i, h, len;
   time_t present;

 
    /* Tokenize the first 16 words in the incoming data, we really don't need to worry
      about anything else and we don't need the original string for anything. */

    token[tokens] = strtok(IRC_RAW, " ");

    while(++tokens < 16 && (token[tokens] = strtok(NULL, " "))); 


   /* Anything with less than 1 token is useless to us */  

    if(tokens <= 1)
        return;
   
    if(!strcasecmp(token[0], "PING"))
     {
         irc_send("PONG %s", token[1]);
	 return;
     }

    /* 001 is sent on initial connect to the IRC host */

    if(!strcasecmp(token[1], "001"))
     { 
         irc_send("OPER %s", CONF_OPER);
         irc_send("MODE %s +c", CONF_NICK);      
         do_perform();   
         return;    
     }   
    
   
    /* 471, 473, 474, 475 are 'Cannot Join' messages */

    if(!strcasecmp(token[1], "471") ||
       !strcasecmp(token[1], "473") ||
       !strcasecmp(token[1], "474") ||
       !strcasecmp(token[1], "475"))

       { 
            if(!CONF_CHANSERV_INVITE)            
                return;

            irc_send(CONF_CHANSERV_INVITE, token[3]); /* 4th token is channel we can't join */             
            return;
       }

    /* Handle invites, complicated code ahead is due to a decision
     * not to use strtok() or strstr() in the following block */

    if(!strcasecmp(token[1], "INVITE"))
       {

           len = strlen(CONF_CHANNELS);
           irc_channel = token[3] + 1; /* token 4 is the channel, + 1 to shift past ':' */

           for(i = 0; i < len; i++)
             {
                 if(CONF_CHANNELS[i] == '#')
                    {
                        for(h = (i + 1); h < len; h++)
                           {
                               if(CONF_CHANNELS[h] == ',' || CONF_CHANNELS[h] == ' ' || h == (len - 1))
                                 {
                                     if(h == (len - 1))
                                        h++;
                                   
                                     if(strlen(irc_channel) != (h - i))
                                         break; 
                                 
                                     if(!strncasecmp(&(CONF_CHANNELS[i]), irc_channel, strlen(irc_channel)))
                                       {
                                           irc_send("JOIN %s", irc_channel);
                                           return;
                                       }
                                 }
                           }
                    }
             }
                         
       }
    
    /* Handle nickserv identification */

    if(!strcasecmp(token[1], "NOTICE"))
      {
          if(!strchr(token[0], '@'))  /* Ignore server notices */
               return ;

          if(!strcasecmp(strtok(token[0] + 1, "!") , "NICKSERV"))
            {
                  time(&present);
                  if(((int) present - (int) IRC_NICKSERV_LAST) >= 10) /* If last used notice was greater than/equal to 10 sec ago */
                    {
                         irc_send(CONF_NICKSERV_IDENT);              /* Identify to nickserv */  
                         time(&IRC_NICKSERV_LAST);                   /* Record last ident    */
                    }
                  return;
            }
          
      }

    /* Search for +c notices */

    if(token[0][0] == ':')
     {
          /* Toss any notices NOT from a server */

          if(strchr(token[0], '@'))
               return ;

          /* Notice is too short to be a connect notice, 
           * toss it to save on execution time */

          if(tokens < 11)
                return;
             


          if(!strcmp(token[7], "connecting:"))
            { 
                 /* Token 11 is the IP of the remote host 
                  * enclosed in [ ]. We need to remove it from
                  * [ ] and pass it to the scanner. */

                  addr = token[10] + 1;          /* Shift over 1 byte to pass over [ */
                  addr = strtok(addr, "]");        /* Replace ] with a /0              */


                  /* Token 9 is the nickname of the connecting client */
                  irc_nick = token[8];

                 /* Token 10 is (user@host), we want to parse the user/host out
                  * for future reference in case we need to kline the host */
                  
                  irc_user = token[9] + 1;          /* Shift one byte over to discard '(' */
                  irc_user = strtok(irc_user, "@"); /* username is everything before the '@' */
                     
                  irc_addr = strtok(NULL , ")");    /* irc_addr is everything between '@' and closing ')' */
                                
                  scan_connect(addr, irc_addr, irc_nick, irc_user);
            }
     }


}

/*
 *  Perform on connect functions.
 *
 *
 */


void do_perform()
{    
      log("IRC -> Connected to %s:%d", CONF_SERVER, CONF_PORT);
      irc_send(CONF_NICKSERV_IDENT);       /* Identify to nickserv */
      irc_send("JOIN %s", CONF_CHANNELS);  /* Join all listed channels */

}

/*
 *  Functions we need to perform ~1 seconds.
 *
 */

void irc_timer()
{
}
