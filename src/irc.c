/*
 * Copyright (C) 2002  Erik Fears
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to
 *
 *       The Free Software Foundation, Inc.
 *       59 Temple Place - Suite 330
 *       Boston, MA  02111-1307, USA.
 *
 *
 */

#include "setup.h"

#include <stdio.h>
#include <unistd.h>

#ifdef STDC_HEADERS
# include <stdlib.h>
# include <string.h>
#endif

#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include <errno.h>
#include <stdarg.h>
#include <regex.h>

#include "config.h"
#include "irc.h"
#include "log.h"
#include "opercmd.h"
#include "scan.h"
#include "dnsbl.h"
#include "stats.h"
#include "extern.h"
#include "options.h"
#include "match.h"
#include "compat.h"
#include "negcache.h"
#include "malloc.h"

static void irc_init(void);
static void irc_connect(void);
static void irc_reconnect(void);
static void irc_read(void);
static void irc_parse(void);

static struct ChannelConf *get_channel(const char *);

static struct UserInfo *userinfo_create(char *);
static void userinfo_free(struct UserInfo *source);

static void m_ping(char **, unsigned int, char *, struct UserInfo *);
static void m_invite(char **, unsigned int, char *, struct UserInfo *);
static void m_privmsg(char **, unsigned int, char *, struct UserInfo *);
static void m_notice(char **, unsigned int, char *, struct UserInfo *);
static void m_perform(char **, unsigned int, char *, struct UserInfo *);

extern time_t LAST_REAP_TIME;
extern struct cnode *nc_head;

/*
 * Certain variables we don't want to allocate memory for over and over
 * again so global scope is given.
 */

char                IRC_RAW[MSGLENMAX];         /* Buffer to read data into              */
char                IRC_SENDBUFF[MSGLENMAX];    /* Send buffer                           */
int                 IRC_RAW_LEN    = 0;         /* Position of IRC_RAW                   */

int                 remote_is_ipv6 = 0;
int                 bindto_ipv6    = 0;

int                 IRC_FD         = -1;        /* File descriptor for IRC client        */

struct bopm_sockaddr IRC_SVR;                   /* Sock Address Struct for IRC server    */
struct bopm_ircaddr IRC_LOCAL;                  /* Sock Address Struct for Bind          */

unsigned int ssize = -1;
unsigned int isize = -1;

struct hostent     *IRC_HOST;                   /* Hostent struct for IRC server         */
fd_set              IRC_READ_FDSET;             /* fd_set for IRC (read) data for select()*/

struct timeval      IRC_TIMEOUT;                /* timeval struct for select() timeout   */
time_t              IRC_NICKSERV_LAST = 0;      /* Last notice from nickserv             */
time_t              IRC_LAST = 0;               /* Last full line of data from irc server*/

/* Table should be ordered with most occuring (or priority)
   commands at the top of the list. */

struct CommandHash COMMAND_TABLE[] = {
   {"NOTICE",               m_notice  },
   {"PRIVMSG",              m_privmsg },
   {"PING",                 m_ping    },
   {"INVITE",               m_invite  },
   {"001",                  m_perform },
};

/* irc_cycle
 *
 *    Pass control to the IRC portion of BOPM to handle any awaiting IRC events.
 *
 * Parameters:
 *    None
 *
 * Return:
 *    None
 *
 */

void irc_cycle(void)
{
    if (IRC_FD <= 0)
    {
        /* Initialise negative cache. */
        if (OptionsItem->negcache > 0)
           nc_init(&nc_head);

        /* Resolve remote host. */
        irc_init();

        /* Connect to remote host. */
        irc_connect();
    }

    IRC_TIMEOUT.tv_sec  = 0;
    /* Block .05 seconds to avoid excessive CPU use on select(). */
    IRC_TIMEOUT.tv_usec = 50000;

    FD_ZERO(&IRC_READ_FDSET);
    FD_SET(IRC_FD, &IRC_READ_FDSET);

    switch (select((IRC_FD + 1), &IRC_READ_FDSET, 0, 0, &IRC_TIMEOUT))
    {
    case -1:
        return;
        break;
    case 0:
        break;
    default:
        /* Check if IRC data is available. */
        if (FD_ISSET(IRC_FD, &IRC_READ_FDSET))
            irc_read();
        break;
    }
}




/* irc_init
 *
 *    Resolve IRC host and perform other initialization.
 *
 * Parameters: 
 *    None
 * 
 * Return:
 *    None
 *
 */

static void irc_init(void)
{
    struct bopm_sockaddr bsaddr;

    ssize = sizeof(struct bopm_sockaddr);
    isize = sizeof(struct bopm_ircaddr);

    if (IRC_FD)
        close(IRC_FD);

    memset(&IRC_SVR, 0, ssize);
    memset(&IRC_LOCAL, 0, isize);
    memset(&bsaddr, 0, sizeof(struct bopm_sockaddr));

    /* Resolve IRC host. */
    if ((IRC_HOST = bopm_gethostbyname(IRCItem->server)) == NULL)
    {
        switch(h_errno)
        {
        case NO_ADDRESS:
        case HOST_NOT_FOUND:
            log("IRC -> bopm_gethostbyname(): The specified host (%s) is unknown", IRCItem->server);
            break;
        case NO_RECOVERY:
            log("IRC -> bopm_gethostbyname(): An unrecoverable error occured resolving (%s)", IRCItem->server);
            break;
        case TRY_AGAIN:
            log("IRC -> bopm_gethostbyname(): Error occured with authoritive name server (%s)", IRCItem->server);
            break;
        default:
            log("IRC -> bopm_gethostbyname(): Unknown error resolving (%s)", IRCItem->server);
            break;
        }
        exit(EXIT_FAILURE);
    }

#ifdef IPV6
    if (remote_is_ipv6)
    {
        IRC_SVR.sas.sa6.sin6_family = AF_INET6;
        IRC_SVR.sas.sa6.sin6_port = htons(IRCItems->port);
        IRC_SVR.sas.sa6.sin6_addr = *((struct in6_addr *) IRC_HOST->h>addr_list[0]);

        if (IN6_ARE_ADDR_EQUAL(&(IRC_SVR.sas.sa6.sin6_addr) & in6addr_any))
        {
            log("IRC -> Unknown error resolving remote host (%s)",
                IRCItem->server);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        IRC_SVR.sas.sa4.sin_family = AF_INET;
        IRC_SVR.sas.sa4.sin_port = htons(IRCItem->port);
        IRC_SVR.sas.sa4.sin_addr =
            *((struct in_addr *) IRC_HOST->h_addr);

        if (IRC_SVR.sas.sa4.sin_addr.s_addr == INADDR_NONE)
        {
            log("IRC -> Unknown error resolving remote host (%s)",
                IRCItem->server);
            exit(EXIT_FAILURE);
        }
    }
#else
    IRC_SVR.sas.sa4.sin_family = AF_INET;
    IRC_SVR.sas.sa4.sin_port = htons(IRCItem->port);
    IRC_SVR.sas.sa4.sin_addr = *((struct in_addr *) IRC_HOST->h_addr);

    if (IRC_SVR.sas.sa4.sin_addr.s_addr == INADDR_NONE)
    {
        log("IRC -> Unknown error resolving remote host (%s)",
            IRCItem->server);
        exit(EXIT_FAILURE);
    }
#endif

    IRC_FD = socket(remote_is_ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);

    /* Request file desc for IRC client socket */

    if (IRC_FD == -1)
    {
        switch(errno)
        {
        case EINVAL:
        case EPROTONOSUPPORT:
            log("IRC -> socket(): SOCK_STREAM is not "
                "supported on this domain");
            break;
        case ENFILE:
            log("IRC -> socket(): Not enough free file "
                "descriptors to allocate IRC socket");
            break;
        case EMFILE:
            log("IRC -> socket(): Process table overflow when "
                "requesting file descriptor");
            break;
        case EACCES:
            log("IRC -> socket(): Permission denied to create "
                "socket of type SOCK_STREAM");
            break;
        case ENOMEM:
            log("IRC -> socket(): Insufficient memory to "
                "allocate socket");
            break;
        default:
            log("IRC -> socket(): Unknown error allocating "
                "socket");
            break;
        }
        exit(EXIT_FAILURE);
    }

    if (strlen(IRCItem->vhost) > 0)
    {
        int bindret = 0;
#ifdef IPV6

        if (bindto_ipv6)
        {
            if (!inetpton(AF_INET6, IRCItem->vhost, &(IRC_LOCAL.ins.in6.s6_addr)))
            {
                log("IRC -> bind(): %s is an invalid address", IRCItem->vhost);
                exit(EXIT_FAILURE);
            }
            copy_s_addr(bsaddr.sas.sa6.sin6_addr.s6_addr,
                        IRC_LOCAL.ins.in6.s6_addr);
            bsaddr.sas.sa6.sin6_family = AF_INET6;
            bsaddr.sas.sa6.sin6_port = htons(0);
            bindret = bind(IRC_FD, (struct sockaddr *) &(bsaddr.sas.sa6),
                           sizeof(bsaddr));
        }
        else
        {
#endif
            if (!inetpton(AF_INET, IRCItem->vhost, &(IRC_LOCAL.ins.in4.s_addr)))
            {
                log("IRC -> bind(): %s is an invalid address", IRCItem->vhost);
                exit(EXIT_FAILURE);
            }
            bsaddr.sas.sa4.sin_addr.s_addr = IRC_LOCAL.ins.in4.s_addr;
            bsaddr.sas.sa4.sin_family = AF_INET;
            bsaddr.sas.sa4.sin_port = htons(0);
            bindret = bind(IRC_FD, (struct sockaddr *) &(bsaddr.sas.sa4),
                           sizeof(bsaddr));
#ifdef IPV6

        }
#endif

        if (bindret)
        {
            switch(errno)
            {
            case EACCES:
                log("IRC -> bind(): No access to bind to %s",
                    IRCItem->vhost);
                break;
            default:
                log("IRC -> bind(): Error binding to %s (%d)",
                    IRCItem->vhost, errno);
                break;
            }
            exit(EXIT_FAILURE);
        }
    }
}


/*
 * Send data to remote IRC host.
 */


void irc_send(char *data, ...)
{
    va_list arglist;
    char    data2[MSGLENMAX];
    char    tosend[MSGLENMAX];

    va_start(arglist, data);
    vsnprintf(data2, MSGLENMAX - 1, data, arglist);
    va_end(arglist);

    if (OPT_DEBUG >= 2)
        log("IRC SEND -> %s", data2);

    snprintf(tosend, MSGLENMAX - 1, "%s\n", data2);

    if (send(IRC_FD, tosend, strlen(tosend), 0) == -1)
    {

        /* Return of -1 indicates error sending data; we reconnect. */
        log("IRC -> Error sending data to server\n");
        irc_reconnect();
    }
}

/*
 * K:line given ip for given reason.
 */

void irc_kline(char *addr, char *ip)
{
    //irc_send(IRCItem->kline, addr, ip);
}

/*
 * Create socket and connect to IRC server specificied in config file
 * (IRCItem->server) with port (IRCItem->port).
 */

static void irc_connect(void)
{
    /* Connect to IRC server as client. */
    if (connect(IRC_FD, (struct sockaddr *) &IRC_SVR,
                sizeof(IRC_SVR)) == -1)
    {
        switch(errno)
        {
        case EISCONN:
            /* Already connected */
            return;
        case ECONNREFUSED:
            log("IRC -> connect(): Connection refused by (%s)",
                IRCItem->server);
            break;
        case ETIMEDOUT:
            log("IRC -> connect(): Timed out connecting to (%s)",
                IRCItem->server);
            break;
        case ENETUNREACH:
            log("IRC -> connect(): Network unreachable");
            break;
        case EALREADY:
            /* Previous attempt not complete */
            return;
        default:
            log("IRC -> connect(): Unknown error connecting to (%s)",
                IRCItem->server);

            if (OPT_DEBUG >= 1)
                log(strerror(errno));
        }
        exit(EXIT_FAILURE);
    }

#ifdef WITH_UNREAL
    irc_send("PROTOCTL HCN");
#endif /* WITH_UNREAL */

    irc_send("NICK %s", IRCItem->nick);

    if(strlen(IRCItem->password) > 0)
        irc_send("PASS %s", IRCItem->password);

    irc_send("USER %s %s %s :%s", 
             IRCItem->username, IRCItem->username, IRCItem->username,
             IRCItem->realname);
}


/* Disconnect from IRC. Reconnection will happen when the next irc_cycle.

   !!! This function should be used for reconnection rather than calling irc_connect
   directly. !!!

*/

static void irc_reconnect(void)
{

    if(IRC_FD > 0)
        close(IRC_FD);

    /* Set IRC_FD 0 for reconnection on next irc_cycle(). */
    IRC_FD = 0;

    log("IRC -> Connection to (%s) lost, reconnecting.", IRCItem->server);
}

/*
 * Read one character at a time until an endline is hit, at which time control
 * is passed to irc_parse() to parse that line.
 */

static void irc_read(void)
{
    int len;
    char c;

    while ((len = read(IRC_FD, &c, 1)) > 0)
    {
        if (c == '\r')
            continue;

        if (c == '\n')
        {
            /* Null string. */
            IRC_RAW[IRC_RAW_LEN] = '\0';
            /* Parse line. */
            irc_parse();
            /* Reset counter. */
            IRC_RAW_LEN = 0;
            break;
        }

        if (c != '\r' && c != '\n' && c != '\0')
            IRC_RAW[IRC_RAW_LEN++] = c;
    }

    if(len == -1 && errno != EAGAIN)
    {
       log("IRC -> Read error.");
       irc_reconnect();
       IRC_RAW_LEN = 0;
       return;
    }
}

/*
 * A full line has been read by irc_read(); this function breaks the line
 * into parv[]. 
 */

static void irc_parse(void)
{
   struct UserInfo *source_p;
   char *pos;
   int i;

   /* 
      parv stores the parsed token, parc is the count of the parsed 
      tokens
     
      parv[0] is ALWAYS the source, and is the server name of the source
      did not exist 
   */

   static char            *parv[15];
   static unsigned int     parc;
   static char             msg[MSGLENMAX];    /* Temporarily stores IRC msg to pass to handlers */

   parc = 1;

   if(IRC_RAW_LEN <= 0)
      return;

   if (OPT_DEBUG >= 2)
      log("IRC READ -> %s", IRC_RAW);

   time(&IRC_LAST);

   /* Store a copy of IRC_RAW for the handlers (for functions that need PROOF) */
   strcpy(msg, IRC_RAW);

   /* parv[0] is always the source */
   if(IRC_RAW[0] == ':')
      parv[0] = IRC_RAW + 1;
   else
   {
      parv[0] = IRCItem->server;
      parv[parc++] = IRC_RAW;
   }

   pos = IRC_RAW;
 
   while(pos = strchr(pos, ' '))
   {

      /* Avoid excessive spaces and end of IRC_RAW */
      if(*(pos + 1) == ' ' && *(pos + 1) == '\0')
      {
         pos++;
         continue;
      }

      /* Anything after a : is considered the final string of the
            message */
      if(*(pos + 1) == ':')
      {
         parv[parc++] = pos + 2;
         *pos = '\0';
         break;
      }
        
      /* Set the next parv at this position and replace the space with a 
         \0 for the previous parv */
      parv[parc++] = pos + 1;
      *pos = '\0';
      pos++;
   }

   /* Generate a UserInfo struct from the source */

   source_p = userinfo_create(parv[0]);

   /* Determine which command this is from the command table 
      and let the handler for that command take control */

   for(i = 0; i < (sizeof(COMMAND_TABLE) / sizeof(struct CommandHash)); i++) 
      if(strcasecmp(COMMAND_TABLE[i].command, parv[1]) == 0)
      {
         (*COMMAND_TABLE[i].handler)(parv, parc, msg, source_p);
         break;
      }

   userinfo_free(source_p);
}


/*
 * Functions we need to perform ~1 seconds.
 */

void irc_timer(void)
{
    time_t present, delta;

    time(&present);

    delta = present - IRC_LAST;

    /* No data in NODATA_TIMEOUT minutes (set in options.h). */
    if (delta >= NODATA_TIMEOUT)
    {
        log("IRC -> Timeout awaiting data from server.");
        irc_reconnect();
        /* Make sure we dont do this again for a while */
        time(&IRC_LAST);
    }
    else if (delta >= NODATA_TIMEOUT / 2)
    {
        /*
         * Generate some data so high ping times or bugs in certain
         * ircds (*cough* unreal *cough*) don't cause uneeded
         * reconnections
         */
        irc_send("PING :BOPM");
    }

    /* Get rid of old command structures. */
    if ((present - LAST_REAP_TIME) >= 120)
    {
        reap_commands(present);
        time(&LAST_REAP_TIME);
    }
}

/*
 * Check if channel is one of our configured report channels, and return a
 * pointer to it if so, or NULL if not.
 */
static struct ChannelConf *get_channel(const char *channel)
{
    node_t *node;
    struct ChannelConf *item;

    LIST_FOREACH(node, IRCItem->channels->head)
    {
       item = node->data;
       
       if(strcasecmp(item->name, channel) == 0)
          return item;
    }

    return NULL;
}


/* userinfo_create
 *
 *    Parse a nick!user@host into a UserInfo struct
 *    and return a pointer to the new struct.
 *
 * Parameters:
 *    source: nick!user@host to parse
 *
 * Return:
 *    pointer to new UserInfo struct, or NULL if parsing failed
 *
 */

static struct UserInfo *userinfo_create(char *source)
{
   struct UserInfo *ret;

   char *nick;
   char *username;
   char *hostname;
   char *tmp;

   int i, len;

   nick = username = hostname = NULL;
   tmp = DupString(source);
   len = strlen(tmp);

   nick = tmp;

   for(i = 0; i < len; i++)
   {
      if(tmp[i] == '!')
      {
         tmp[i] = '\0';
         username = tmp + i + 1;
      }
      if(tmp[i] == '@')
      {
         tmp[i] = '\0';
         hostname = tmp + i + 1;
      } 
   }
  
   if(nick == NULL || username == NULL || hostname == NULL)
      return NULL;

   ret = MyMalloc(sizeof(struct UserInfo));

   ret->irc_nick     = DupString(nick);
   ret->irc_username = DupString(username);
   ret->irc_hostname = DupString(hostname);

   MyFree(tmp);

   return ret;
};




/* userinfo_free
 *
 *    Free a UserInfo struct created with userinfo_create.
 *
 * Parameters:
 *    source: struct to free
 * 
 * Return: None
 *
 */

static void userinfo_free(struct UserInfo *source_p)
{
   if(source_p == NULL)
      return;

   MyFree(source_p->irc_nick);
   MyFree(source_p->irc_username);
   MyFree(source_p->irc_hostname);
   MyFree(source_p);
}


/*  do_perform
 *
 *     Actions to perform when successfully connected to IRC.
 */

static void m_perform(char **parv, unsigned int parc, char *msg, struct UserInfo *notused)
{
    node_t *node;
    struct ChannelConf *channel;

    log("IRC -> Connected to %s:%d", IRCItem->server, IRCItem->port);

    //FIXME
    //if (CONF_NICKSERV_IDENT) {
    /* Identify to nickserv. */
    //  irc_send(CONF_NICKSERV_IDENT);
    //  }

    /* Oper */
    irc_send("OPER %s", IRCItem->oper);

    /* Set modes */
    irc_send("MODE %s %s", IRCItem->nick, IRCItem->mode);

    /* Set Away */
    irc_send("AWAY :%s", IRCItem->away);

    /* Join all listed channels. */
    LIST_FOREACH(node, IRCItem->channels->head)
    {
       channel = (struct ChannelConf *) node->data;

       if(strlen(channel->name) == 0)
          continue;

       if(strlen(channel->key) > 0)
          continue;

       if(strlen(channel->key) > 0)
          irc_send("JOIN %s %s", channel->name, channel->key);
       else
          irc_send("JOIN %s", channel->name);
    }
}



/* m_ping
 *
 * parv[0]  = source
 * parv[1]  = PING
 * parv[2]  = PING TS/Package
 *
 * source_p: UserInfo struct of the source user, or NULL if
 * the source (parv[0]) is a server.
 */
static void m_ping(char **parv, unsigned int parc, char *msg, struct UserInfo *source_p)
{
   if(parc < 3)
      return;

   if(OPT_DEBUG >= 2)
      log("IRC -> PING? PONG!");

   irc_send("PONG %s", parv[2]);
}



/* m_invite
 *
 * parv[0]  = source
 * parv[1]  = INVITE
 * parv[2]  = target
 * parv[3]  = channel
 *
 * source_p: UserInfo struct of the source user, or NULL if
 * the source (parv[0]) is a server.
 *
 */

static void m_invite(char **parv, unsigned int parc, char *msg, struct UserInfo *source_p)
{
   struct ChannelConf *channel;

   if(parc < 4)
      return;

   log("IRC -> Invited to %s by %s", parv[3], parv[0]);

   if((channel = get_channel(parv[3])) == NULL)
      return;

   irc_send("JOIN %s %s", channel->name, channel->key);
}




/* m_privmsg
 *
 * parv[0]  = source
 * parv[1]  = PRIVMSG
 * parv[2]  = target (channel or user)
 * parv[3]  = message
 *
 * source_p: UserInfo struct of the source user, or NULL if
 * the source (parv[0]) is a server.
 *
 */

static void m_privmsg(char **parv, unsigned int parc, char *msg, struct UserInfo *source_p)
{
   struct ChannelConf channel;

   if(source_p == NULL)
      return;

}


/* m_notice
 *
 * parv[0]  = source
 * parv[1]  = NOTICE
 * parv[2]  = target
 * parv[3]  = message
 *
 *
 * source_p: UserInfo struct of the source user, or NULL if
 * the source (parv[0]) is a server.
 *
 */

static void m_notice(char **parv, unsigned int parc, char *msg, struct UserInfo *source_p)
{


   static regex_t *preg = NULL;
   regmatch_t pmatch[5];

   static char errmsg[256];
   int errnum, i;

   char *user[4];

   if(parc < 4)
      return;

   /* Not interested in notices from users */
   if(source_p != NULL)
      return;
  
   /* Compile the regular expression if it has not been already */
   if(preg == NULL)
   {
      preg = MyMalloc(sizeof(regex_t));

      if((errnum = regcomp(preg, IRCItem->connregex, REG_ICASE | REG_EXTENDED)) != 0)
      {

         regerror(errnum, preg, errmsg, 256);
         log("IRC REGEX -> Error when compiling regular expression");
         log("IRC REGEX -> %s", errmsg);
         preg = NULL;

         return;
      }
   }

   /* Match the expression against the possible connection notice */
   if(regexec(preg, parv[3], 5, pmatch, 0) != 0)
      return;

   if(OPT_DEBUG > 0)
      log("IRC REGEX -> Regular expression caught connection notice. Parsing.");
 
   if(pmatch[4].rm_so == -1)
   {
      log("IRC REGEX -> pmatch[4].rm_so is -1 while parsing??? Aborting.");
      return;
   }

   /*
       Offsets for data in the connection notice:

       NICKNAME: pmatch[1].rm_so  TO  pmatch[1].rm_eo 
       USERNAME: pmatch[2].rm_so  TO  pmatch[2].rm_eo
       HOSTNAME: pmatch[3].rm_so  TO  pmatch[3].rm_eo
       IP      : pmatch[4].rm_so  TO  pmatch[4].rm_eo

    */ 

   for(i = 0; i < 4; i++)
   {
      user[i] = (parv[3] + pmatch[i + 1].rm_so);
      *(parv[3] + pmatch[i + 1].rm_eo) = '\0';
   }

   if(OPT_DEBUG > 0)
      log("IRC REGEX -> Parsed %s!%s@%s [%s] from connection notice.", 
               user[0], user[1], user[2], user[3]);
 
   /*FIXME (reminder) In the case of any rehash to the regex, preg MUST be freed first. 
   regfree(preg);
   */

   /* Pass this information off to scan.c */
   scan_connect(user, msg);
}
