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

#include "setup.h"

#include <stdio.h>
#include <unistd.h>

#ifdef STDC_HEADERS
# include <stdlib.h>
# include <string.h>
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

#include "irc.h"
#include "log.h"
#include "config.h"
#include "opercmd.h"
#include "scan.h"
#include "dnsbl.h"
#include "stats.h"
#include "extern.h"
#include "options.h"
#include "version.h"
#include "match.h"

static void irc_init(void);
static void irc_connect(void);
static void irc_reconnect(void);
static void irc_read(void);
static void irc_parse(void);
static void do_perform(void);
static void do_connect(char *addr, char *irc_nick, char *irc_user,
    char *irc_addr, char *conn_notice);
static void do_hybrid_connect(int tokens, char **token);
static void do_trircd_connect(int tokens, char **token);
static void do_ultimateircd_connect(int tokens, char **token);
static void do_xnet_connect(int tokens, char **token);
static char *get_chan_key(const char *channel);
static char *check_channel(const char *channel);

extern char *CONFFILE;

/*
 * Certain variables we don't want to allocate memory for over and over
 * again so global scope is given.
 */

char                IRC_RAW[MSGLENMAX];      /* Buffer to read data into              */
char                IRC_SENDBUFF[MSGLENMAX]; /* Send buffer                           */
int                 IRC_RAW_LEN = 0;         /* Position of IRC_RAW                   */

int                 IRC_FD = -1;      /* File descriptor for IRC client        */
struct sockaddr_in  IRC_SVR;          /* Sock Address Struct for IRC server    */
struct sockaddr_in  IRC_LOCAL;        /* Sock Address Struct for Bind          */
struct hostent     *IRC_HOST;         /* Hostent struct for IRC server         */
fd_set              IRC_READ_FDSET;   /* fd_set for IRC (read) data for select()*/

struct timeval      IRC_TIMEOUT;      /* timeval struct for select() timeout   */
time_t              IRC_NICKSERV_LAST = 0; /* Last notice from nickserv        */
time_t              IRC_LAST = 0;     /* Last full line of data from irc server*/

/* Give one cycle to the IRC client, which
 * will allow it to poll for data and handle
 * that data if need be.
 */

void irc_cycle(void)
{	
	if (IRC_FD <= 0) {
		/* No socket open. */
		config_load(CONFFILE);	/* Reload config. */
		irc_init();		/* Resolve remote host. */
		irc_connect();		/* Connect to remote host. */
        }

	IRC_TIMEOUT.tv_sec  = 0;
	/*
	 * Block .05 seconds to avoid excessive CPU use on select(). */
	IRC_TIMEOUT.tv_usec = 50000;
           
	FD_ZERO(&IRC_READ_FDSET);

	FD_SET(IRC_FD, &IRC_READ_FDSET);
            
	switch (select((IRC_FD + 1), &IRC_READ_FDSET, 0, 0, &IRC_TIMEOUT)) {
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


/*  Allocate socket file descriptor for connection,
 *  and resolve remote host.
 *
 */

static void irc_init(void)
{
	if (IRC_FD)
		close(IRC_FD);  

	memset(&IRC_SVR, 0, sizeof(IRC_SVR));
	memset(&IRC_LOCAL, 0, sizeof(IRC_LOCAL));

	/* Resolve IRC host. */
	if (!(IRC_HOST = gethostbyname(CONF_SERVER))) {
		switch(h_errno) {
		case HOST_NOT_FOUND:
			log("IRC -> gethostbyname(): The specified host "
			    "(%s) is unknown", CONF_SERVER);
			break;
		case NO_ADDRESS:
			log("IRC -> gethostbyname(): The specified name "
			    "(%s) exists, but does not have an IP",
			    CONF_SERVER);
			break;
		case NO_RECOVERY:
			log("IRC -> gethostbyname(): An unrecoverable "
			    "error occured resolving (%s)", CONF_SERVER);
			break;
		case TRY_AGAIN:
			log("IRC -> gethostbyname(): Error occured with "
			    "authoritive name server (%s)", CONF_SERVER);
			break;
		default:
			log("IRC -> gethostbyname(): Unknown error "
			    "resolving (%s)", CONF_SERVER);
			break;
                }
		exit(EXIT_FAILURE);
	}

	IRC_SVR.sin_family      = AF_INET;
	IRC_SVR.sin_port        = htons(CONF_PORT);
	IRC_SVR.sin_addr = *((struct in_addr *) IRC_HOST->h_addr);

	if (IRC_SVR.sin_addr.s_addr == INADDR_NONE) {
		log("IRC -> Unknown error resolving remote host (%s)",
		    CONF_SERVER);
		exit(EXIT_FAILURE);
	}

	/* Request file desc for IRC client socket. */
	IRC_FD = socket(PF_INET, SOCK_STREAM, 0);

	if (IRC_FD == -1) {
		switch(errno) {
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

	if (CONF_BINDIRC) {
		if (!inet_aton(CONF_BINDIRC, &(IRC_LOCAL.sin_addr))) {
			log("IRC -> bind(): %s is an invalid address",
			    CONF_BINDIRC);
			exit(EXIT_FAILURE);
		}

		IRC_LOCAL.sin_family = AF_INET;
		IRC_LOCAL.sin_port = 0;

		if (bind(IRC_FD, (struct sockaddr *)&IRC_LOCAL,
		    sizeof(struct sockaddr_in)) == -1) {
			switch(errno) {
			case EACCES:
				log("IRC -> bind(): No access to bind to %s",
				    CONF_BINDIRC);
				break;
			default:
				log("IRC -> bind(): Error binding to %s (%d)",
				    CONF_BINDIRC, errno);
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

	if (send(IRC_FD, tosend, strlen(tosend), 0) == -1) {
		/* Return of -1 indicates error sending data; we reconnect. */
		irc_reconnect();
	}
}

/*
 * K:line given ip for given reason.
 */

void irc_kline(char *addr, char *ip)
{
	irc_send(CONF_KLINE_COMMAND, addr, ip);
}

/*
 * Create socket and connect to IRC server specificied in config file
 * (CONF_SERVER) with port CONF_PORT.
 */

static void irc_connect(void)
{
	/* Connect to IRC server as client. */
	if (connect(IRC_FD, (struct sockaddr *) &IRC_SVR,
	    sizeof(IRC_SVR)) == -1) {
		switch(errno) {
		case EISCONN: 
			/* Already connected */
			return;
		case ECONNREFUSED:
			log("IRC -> connect(): Connection refused by (%s)",
			    CONF_SERVER);
			break;
		case ETIMEDOUT:
			log("IRC -> connect(): Timed out connecting to (%s)",
			    CONF_SERVER);
			break;
		case ENETUNREACH:
			log("IRC -> connect(): Network unreachable");
			break;
		case EALREADY:
			/* Previous attempt not complete */
			return;
		default:
			log("IRC -> connect(): Unknown error connecting to (%s)",
			    CONF_SERVER);
		}	
		exit(EXIT_FAILURE);
	}

#ifdef WITH_UNREAL
	irc_send("PROTOCTL HCN");
#endif /* WITH_UNREAL */

	irc_send("NICK %s",CONF_NICK);
	if (CONF_PASSWORD)
		irc_send("PASS %s",CONF_PASSWORD);
	irc_send("USER %s %s %s :Blitzed Open Proxy Monitor", CONF_USER,
	    CONF_USER, CONF_USER);
}


static void irc_reconnect(void)
{
	if(IRC_FD > 0)
		close(IRC_FD);

	/* Set IRC_FD 0 for reconnection on next irc_cycle(). */
	IRC_FD = 0;

	log("IRC -> Connection to (%s) lost, rehashing and reconnecting.",
	    CONF_SERVER);
}

/*
 * Read one character at a time until an endline is hit, at which time control
 * is passed to irc_parse() to parse that line.
 */
 
static void irc_read(void)
{
	int len;
	char c;

	while ((len = read(IRC_FD, &c, 1))) {
		if (len <= 0) {
			irc_reconnect();
			return;
		}
            
		if (c == '\r')
			continue;
		
		if (c == '\n') {
			/* Null string. */
			IRC_RAW[IRC_RAW_LEN] = 0;
			/* Parse line. */
			irc_parse();
			/* Reset counter. */
			IRC_RAW_LEN = 0;
			break;
		}

		if (c != '\r' && c != '\n' && c != 0)
			IRC_RAW[IRC_RAW_LEN++] = c;
	}
       
	if (len <= 0) {
		irc_reconnect();
		return;
	} 
}

/*
 * A full line has been read by irc_read(); this function begins parsing it.
 */

static void irc_parse(void)
{
	char nick[NICKMAX];
	char *token[32];
	time_t present;
	size_t prefixlen;
	unsigned int tokens;
	char *irc_channel, *key, *user, *target, *msg;

	tokens = 0;

	/* Update timeout tracking. */ 
	time(&IRC_LAST);

	if(OPT_DEBUG >= 2)
		log("IRC READ -> %s", IRC_RAW);

	/*
	 * Tokenize the first 32 words in the incoming data, we really don't
	 * need to worry about anything else and we don't need the original
	 * string for anything.
	 */

	token[tokens] = strtok(IRC_RAW, " ");

	while (++tokens < 32 && (token[tokens] = strtok(NULL, " ")))
		; 

	/* Anything with less than 1 token is useless to us. */  

	if (tokens <= 1)
		return;

	if (!strcasecmp(token[0], "PING")) {
		irc_send("PONG %s", token[1]);
		return;
	}

	/* 001 is sent on initial connect to the IRC host. */

	if (!strcasecmp(token[1], "001")) { 
		irc_send("OPER %s", CONF_OPER);
		irc_send("MODE %s %s", CONF_NICK, CONF_OPER_MODES);      
		if (CONF_AWAY)
			irc_send("AWAY :%s (/msg %s INFO)", CONF_AWAY, CONF_NICK);
		do_perform();
		return;
	}   
    
   	/* 471, 473, 474, 475 are 'Cannot Join' messages. */

	if (!strcasecmp(token[1], "471") ||
	    !strcasecmp(token[1], "473") ||
	    !strcasecmp(token[1], "474") ||
	    !strcasecmp(token[1], "475")) { 
		if(CONF_CHANSERV_INVITE) {
			/* 4th token is channel we can't join. */             
			irc_send(CONF_CHANSERV_INVITE, token[3]);
		}
		return;
	}

	/*
	 * Handle invites, complicated code ahead is due to a decision not to
	 * use strtok() or strstr() in the following block
	 */

	if (!strcasecmp(token[1], "INVITE")) {
		/* token 4 is the channel, + 1 to shift past ':'. */
		irc_channel = check_channel(token[3] + 1);

		if (irc_channel) {
			key = get_chan_key(irc_channel);

			if (key)
				irc_send("JOIN %s %s", irc_channel, key);
			else
				irc_send("JOIN %s", irc_channel);

			return;
		}
	}
    
	/* Handle nickserv identification. */
            
	if (!strcasecmp(token[1], "NOTICE") && strchr(token[0], '@')) {
		if (CONF_NICKSERV_IDENT &&
		    !strcasecmp(strtok(token[0] + 1, "!") , "NICKSERV")) {
			time(&present);
			/*
			 * If last used notice was greater than/equal to
			 * 10 sec ago
			 */
			if ((present - IRC_NICKSERV_LAST) >= 10) {
				/* Identify to nickserv. */  
				irc_send(CONF_NICKSERV_IDENT);
				/* Record last ident. */
				time(&IRC_NICKSERV_LAST);
			}
			return;
		}
          }

	/* Handle rejoining when kicked. */

	/* :grifferz!goats@pc-62-30-219-54-pb.blueyonder.co.uk KICK #wg penguinBopm :test */
	if (!strcasecmp(token[1], "KICK") &&
	    !strcasecmp(token[3], CONF_NICK)) {
		/*
		 * Someone kicked us from channel token[2] so let's
		 * rejoin.
		 */
		log("IRC -> Kicked from %s by %s! (%s)", token[2],
		    token[0], token[4]);
		key = get_chan_key(token[2]);

		if (key)
			irc_send("JOIN %s %s", token[2], key);
		else
			irc_send("JOIN %s", token[2]);
		return;
	}

	/* Any messages from users that we need to respond to. */
	if (!strcasecmp(token[1], "PRIVMSG") && token[0][0] == ':') {
		/* work out who it was from */
		strncpy(nick, token[0] + 1, NICKMAX);
		user = index(nick, '!');

		if(user) {
			/*
			 * Nick is currently the first 32 chars of a userhost,
			 * so null terminate at !
			 */
			*user = '\0';

			msg = token[3];

			if (msg && msg[0] == ':')
				msg++;

			prefixlen = strlen(msg);

			if (token[2][0] == '#' || token[2][0] == '&')
				target = CONF_CHANNELS;
			else
				target = nick;

			/* CTCP VERSION */
			if (strncasecmp(msg, "\001VERSION\001", 9) == 0) {
				irc_send("NOTICE %s :\001VERSION Blitzed "
				    "Open Proxy Monitor %s\001", nick,
				    VERSION);
                  		return;
                	}

			if (strncasecmp(msg, "INFO", 4) == 0) {
				irc_send("NOTICE %s :This bot is designed "
				    "to scan incoming connections for the "
				    "presence of open SOCKS, HTTP and "
				    "other similar servers.", nick);
				irc_send("NOTICE %s :These misconfigured "
				    "servers allow anyone to abuse them "
				    "to 'bounce' through, and are "
				    "frequently used to harass.  As a "
				    "result, use of such proxies is not "
				    "permitted on this IRC network.", nick);
				irc_send("NOTICE %s :If you found this bot "
				    "because of NukeNabber or other "
				    "firewall software on your computer, "
				    "please be aware that this is not "
				    "a nuke or any other form of abusive "
				    "activity.", nick);
				irc_send("NOTICE %s :You can get more "
				    "information about this bot and what "
				    "it does by contacting %s.", nick,
				    CONF_HELP_EMAIL);
				return;
			}

			if (strncasecmp(msg, CONF_NICK,
			    prefixlen > 3 ? prefixlen : 3) &&
			    strcasecmp(msg, "!all")) {
				/*
				 * Not in the form we accept, ignore this
				 * message
				 */
				return;
			}

			if (!token[4]) {
				irc_send("PRIVMSG %s :Some form of "
				    "command would be nice.", target);
				return;
			}
	       
			if (strncasecmp(token[4], "STAT", 4) == 0) {
				do_stats(target);
				return;
			}

			/* Otherwise it might be an oper command. */
			do_oper_cmd(nick, token[4], token[5], target);
		}
	}

	if (!strcasecmp(token[1], "302")) {
		check_userhost(token[3]);
		return;
	}

	/* Search for +c notices. */
 	/* Some ircd (just hybrid/xnet/df?) don't send a server name. */
	if (tokens >= 11 && strcasecmp(token[0], "NOTICE") == 0 &&
	    strcasecmp(token[6], "connecting:") == 0)
		do_xnet_connect(tokens, token);

	if (token[0][0] == ':') {
		/* Toss any notices NOT from a server. */

		if (strchr(token[0], '@'))
			return;

		if (tokens >= 11 && strcmp(token[7], "connecting:") == 0)
			do_hybrid_connect(tokens, token);
		else if (tokens >= 9 && strcmp(token[4], "connecting:") == 0)
			do_trircd_connect(tokens, token);
		else if (tokens >= 17 && strcmp(token[9], "connecting") == 0)
			do_ultimateircd_connect(tokens, token);
	}
}

/*
 * Perform on connect functions.
 */

static void do_perform(void)
{    
	log("IRC -> Connected to %s:%d", CONF_SERVER, CONF_PORT);

	if (CONF_NICKSERV_IDENT) {
		/* Identify to nickserv. */
		irc_send(CONF_NICKSERV_IDENT);
	}
	
	/* Join all listed channels. */
	if (CONF_KEYS)
		irc_send("JOIN %s %s", CONF_CHANNELS, CONF_KEYS);
	else
		irc_send("JOIN %s", CONF_CHANNELS);
}

/*
 * Functions we need to perform ~1 seconds.
 */

void irc_timer(void)
{
	time_t present;
   
	time(&present);
   
	/* No data in NODATA_TIMEOUT minutes (set in options.h). */
	if ((present - IRC_LAST) >= NODATA_TIMEOUT) {
		irc_reconnect();
		/* Make sure we dont do this again for another 5 minutes */
		time(&IRC_LAST);
	}

	/* Get rid of old command structures. */
	if ((present - LAST_REAP_TIME) >= 120) {
		reap_commands(present);
		time(&LAST_REAP_TIME);
	}
}

static void do_connect(char *addr, char *irc_nick, char *irc_user,
    char *irc_addr, char *conn_notice)
{
	string_list *list;

	/*
	 * Check that neither the user's IP nor host matches anything in our
	 * exclude list.
	 */
	for (list = (string_list *) CONF_EXCLUDE; list; list = list->next) {
		if (match(list->text, addr) || match(list->text, irc_addr)) {
			if (OPT_DEBUG) {
				log("SCAN -> excluded user %s!%s@%s",
				    irc_nick, irc_user, irc_addr);
			}
			return;
		}
	}
   
	if (CONF_DNSBL_ZONE &&
	    dnsbl_check(addr, irc_nick, irc_user, irc_addr))
		return;

	scan_connect(addr, irc_addr, irc_nick, irc_user, 0, conn_notice);
}

/*
 * :porkscratchings.pa.us.blitzed.org NOTICE grifferz :*** Notice -- Client connecting: griff (goats@pc-62-30-219-54-pb.blueyonder.co.uk) [62.30.219.54] {1}
 */
static void do_hybrid_connect(int tokens, char **token)
{
	char conn_notice[MSGLENMAX];
	char *addr;	/* IP of remote host in connection notices */
	char *irc_addr;	/* IRC host address of the remote host     */
	char *irc_user;     
	char *irc_nick;

	/* Paranoia. */
	if (tokens < 11)
		return;

	STAT_NUM_CONNECTS++;

	/*
	 * Take a copy of the original connect notice now in case we need it
	 * for evidence later.
	 */
	snprintf(conn_notice, sizeof(conn_notice),
	    "%s %s %s %s %s %s %s %s %s %s %s", token[0], token[1],
	    token[2], token[3], token[4], token[5], token[6], token[7],
	    token[8], token[9], token[10]);

	/* Make sure it is null terminated. */
	conn_notice[MSGLENMAX - 1] = '\0';

	/*
	 * Token 11 is the IP of the remote host enclosed in [ ]. We need
	 * to remove it from [ ] and pass it to the scanner.
	 */

	/* Shift over 1 byte to pass over [. */
	addr = token[10] + 1;
        /* Replace ] with a \0. */
	addr = strtok(addr, "]");

	/* Token 9 is the nickname of the connecting client. */
	irc_nick = token[8];

	/*
	 * Token 10 is (user@host), we want to parse the user/host out for
	 * future reference in case we need to kline the host.
	 */
	
        /* Shift one byte over to discard '('. */
	irc_user = token[9] + 1;
	/* Username is everything before the '@'. */
	irc_user = strtok(irc_user, "@");
	/* irc_addr is everything between '@' and closing ')'. */
	irc_addr = strtok(NULL , ")");
	
	do_connect(addr, irc_nick, irc_user, irc_addr, conn_notice);
}

/*
 * :test.teklan.com.tr NOTICE &CONNECTS :Client connecting: griff (andy@pc-62-30-219-54-pb.blueyonder.co.uk) [62.30.219.54] {1}
 */
static void do_trircd_connect(int tokens, char **token)
{
	char conn_notice[MSGLENMAX];
	char *addr;	/* IP of remote host in connection notices */
	char *irc_addr;	/* IRC host address of the remote host     */
	char *irc_user;     
	char *irc_nick;

	/* Paranoia. */
	if (tokens < 9)
		return;

	STAT_NUM_CONNECTS++;

	/*
	 * Take a copy of the original connect notice now in case we need it
	 * for evidence later.
	 */
	snprintf(conn_notice, sizeof(conn_notice),
	    "%s %s %s %s %s %s %s %s %s", token[0], token[1], token[2],
	    token[3], token[4], token[5], token[6], token[7], token[8]);

	/* Make sure it is null terminated. */
	conn_notice[MSGLENMAX - 1] = '\0';

	/*
	 * Token 8 is the IP of the remote host enclosed in [ ]. We need
	 * to remove it from [ ] and pass it to the scanner.
	 */

	/* Shift over 1 byte to pass over [. */
	addr = token[7] + 1;
        /* Replace ] with a \0. */
	addr = strtok(addr, "]");

	/* Token 6 is the nickname of the connecting client */
	irc_nick = token[5];

	/*
	 * Token 7 is (user@host), we want to parse the user/host out for
	 * future reference in case we need to kline the host.
	 */
	
        /* Shift one byte over to discard '('. */
	irc_user = token[6] + 1;
	/* username is everything before the '@' */
	irc_user = strtok(irc_user, "@");
	/* irc_addr is everything between '@' and closing ')' */
	irc_addr = strtok(NULL , ")");
	
	do_connect(addr, irc_nick, irc_user, irc_addr, conn_notice);
}

/*
 * :CurCuNa.NeT NOTICE AndyBopm :*** Connect/Exit -- from CurCuNa.NeT: Client connecting on port 6667: Misafir (jirc@213.14.40.51) [213.14.40.51] {1} [JPilot jIRC applet User]
 */
static void do_ultimateircd_connect(int tokens, char **token)
{
	char conn_notice[MSGLENMAX];
	char *addr;	/* IP of remote host in connection notices */
	char *irc_addr;	/* IRC host address of the remote host     */
	char *irc_user;     
	char *irc_nick;

	/* Paranoia. */
	if (tokens < 17)
		return;

	STAT_NUM_CONNECTS++;

	/*
	 * Take a copy of the original connect notice now in case we need it
	 * for evidence later.
	 */
	snprintf(conn_notice, sizeof(conn_notice),
	    "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s", token[0],
	    token[1], token[2], token[3], token[4], token[5], token[6],
	    token[7], token[8], token[9], token[10], token[11], token[12],
	    token[13], token[14], token[15], token[16]);

	/* Make sure it is null terminated. */
	conn_notice[MSGLENMAX - 1] = '\0';

	/*
	 * Token 16 is the IP of the remote host enclosed in [ ]. We need
	 * to remove it from [ ] and pass it to the scanner.
	 */

	/* Shift over 1 byte to pass over [. */
	addr = token[15] + 1;
        /* Replace ] with a \0. */
	addr = strtok(addr, "]");

	/* Token 14 is the nickname of the connecting client. */
	irc_nick = token[13];

	/*
	 * Token 15 is (user@host), we want to parse the user/host out for
	 * future reference in case we need to kline the host.
	 */
	
        /* Shift one byte over to discard '('. */
	irc_user = token[14] + 1;
	/* Username is everything before the '@'. */
	irc_user = strtok(irc_user, "@");
	/* irc_addr is everything between '@' and closing ')' */
	irc_addr = strtok(NULL , ")");
	
	do_connect(addr, irc_nick, irc_user, irc_addr, conn_notice);
}



/*
 * NOTICE BopmMirage :*** Notice -- Client connecting: Iain (iain@modem-449.gacked.dialup.pol.co.uk) [62.25.241.193] {1} (6667)
 */
static void do_xnet_connect(int tokens, char **token)
{
	char conn_notice[MSGLENMAX];
	char *addr;	/* IP of remote host in connection notices */
	char *irc_addr;	/* IRC host address of the remote host     */
	char *irc_user;     
	char *irc_nick;

	/* Paranoia. */
	if (tokens < 11)
		return;

	STAT_NUM_CONNECTS++;

	/*
	 * Take a copy of the original connect notice now in case we need
	 * it for evidence later.
	 */
	snprintf(conn_notice, sizeof(conn_notice),
	    "%s %s %s %s %s %s %s %s %s %s %s", token[0], token[1],
	    token[2], token[3], token[4], token[5], token[6], token[7],
	    token[8], token[9], token[10]);

	/* Make sure it is null terminated. */
	conn_notice[MSGLENMAX - 1] = '\0';
	  
	/*
	 * Token 10 is the IP of the remote host enclosed in [ ]. We need to
	 * remove it from [ ] and pass it to the scanner.
	 */
	
	/* Shift over 1 byte to pass over [. */
	addr = token[9] + 1;
	/* Replace ] with a \0. */
        addr = strtok(addr, "]");

	/* Token 8 is the nickname of the connecting client. */
	irc_nick = token[7];

	/*
	 * Token 9 is (user@host), we want to parse the user/host out for
	 * future reference in case we need to kline the host.
	 */

	/* Shift one byte over to discard '('. */
	irc_user = token[8] + 1;
	/* Username is everything before the '@'. */
	irc_user = strtok(irc_user, "@");
	/* irc_addr is everything between '@' and closing ')'. */
	irc_addr = strtok(NULL , ")");

	do_connect(addr, irc_nick, irc_user, irc_addr, conn_notice);
}

/*
 * Return a pointer into CONF_KEYS for the given channel, or NULL if there
 * is no key.
 */
static char *get_chan_key(const char *channel)
{
	int ci, ki;
	char *kp;
	size_t i, h, len;

	ci = 0;
	
	if (!CONF_KEYS || !channel)
		return(NULL);

	len = strlen(CONF_CHANNELS);

	for (i = 0; i < len; i++) {
		if (CONF_CHANNELS[i] != '#') 
			continue;
		
		for (h = (i + 1); h < len; h++) {
			if (CONF_CHANNELS[h] == ',' ||
			    CONF_CHANNELS[h] == ' ' || h == (len - 1)) {
				if (CONF_CHANNELS[h] == ',')
					ci++;

				if (h == (len - 1))
					h++;

				if (strlen(channel) != (h - i))
					break; 

				if (!strncasecmp(&(CONF_CHANNELS[i]),
				    channel, strlen(channel))) {
					for (kp = CONF_KEYS, ki = 0;
					    (ki < ci) && (kp = strchr(kp, ','));
					    ki++) ; /* empty loop */

					if (kp && *kp == ',')
						kp++;

					if (kp && *kp)
						return(kp);
					else
						return(NULL);
				}
			}
		}
	}
	return(NULL);
}

/*
 * Check if channel is one of our configured report channels, and return a
 * pointer to it if so, or NULL if not.
 */
static char *check_channel(const char *channel)
{
	size_t i, len, h;
	
	len = strlen(CONF_CHANNELS);

	for (i = 0; i < len; i++) {
		if (CONF_CHANNELS[i] != '#') 
			continue;

		for (h = (i + 1); h < len; h++) {
			if (CONF_CHANNELS[h] == ',' ||
			    CONF_CHANNELS[h] == ' ' || h == (len - 1)) {
				if(h == (len - 1))
					h++;
                                   
				if (strlen(channel) != (h - i))
					break; 
                                 
				if (!strncasecmp(&(CONF_CHANNELS[i]),
				    channel, strlen(channel))) {
					return(&(CONF_CHANNELS[i]));
				}
			}
		}
	}
	return(NULL);
}
