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
#include <fcntl.h>

#ifdef HAVE_SYS_POLL_H
# include <sys/poll.h>
#endif

#include "config.h"
#include "irc.h"
#include "log.h"
#include "opercmd.h"
#include "scan.h"
#include "stats.h"
#include "dnsbl.h"
#include "extern.h"
#include "options.h"

static void scan_memfail(void);
static void scan_establish(scan_struct *conn);
static void scan_check(void);
static void scan_negfail(scan_struct *conn);
static void scan_readready(scan_struct *conn);
static void scan_read(scan_struct *conn);
static void scan_openproxy(scan_struct *conn);
static void scan_writeready(scan_struct *conn);
static void scan_add(scan_struct *newconn);
static void scan_del(scan_struct *delconn);
static int scan_w_squid(struct scan_struct *conn);
static int scan_w_socks4(struct scan_struct *conn);
static int scan_w_socks5(struct scan_struct *conn);
static int scan_w_cisco(struct scan_struct *conn);
static int scan_w_wingate(struct scan_struct *conn);

/* Linked list head for connections. */
struct scan_struct *CONNECTIONS = 0;

char SENDBUFF[513];

/* Keep track of numbers of open FD's, for use with FDLIMIT. */
unsigned int FD_USE = 0;


/*
 * Protocol Name, Port, Write Handler, Read Handler
 *
 * Always scan Cisco before Wingate, because cisco routers only allow 4
 * connects at once.
 */

protocol_hash SCAN_PROTOCOLS[] = {

       {"HTTP"      , 8080, &(scan_w_squid),    0 ,0 },
/*     {"HTTP"      , 8001, &(scan_w_squid),    0 ,0 },    */
/*     {"HTTP"      , 8000, &(scan_w_squid),    0 ,0 },    */
       {"HTTP"      , 3128, &(scan_w_squid),    0 ,0 },
       {"HTTP"      ,   80, &(scan_w_squid),    0 ,0 },
       {"Socks4"    , 1080, &(scan_w_socks4),   0 ,0 },
       {"Socks5"    , 1080, &(scan_w_socks5),   0 ,0 },
       {"Cisco"     ,   23, &(scan_w_cisco),    0 ,0 },    
       {"Wingate"   ,   23, &(scan_w_wingate),  0 ,0 },
};

size_t SCAN_NUMPROTOCOLS;

void do_scan_init(void)
{
	SCAN_NUMPROTOCOLS = sizeof(SCAN_PROTOCOLS) / sizeof(protocol_hash);
}


static void scan_memfail(void)
{
	log("SCAN -> Error allocating memory.");
	exit(EXIT_FAILURE);
}



/*
 * IRC client has receieved a +c notice from the remote server. scan_connect
 * is called with the connecting IP, where we will begin to establish the
 * proxy testing.
 */

void scan_connect(char *addr, char *irc_addr, char *irc_nick,
    char *irc_user, int verbose, int aftype, char *conn_notice)
{
	size_t i;                
	scan_struct *newconn; 

	if (OPT_DEBUG) {
		log("SCAN -> checking user %s!%s@%s", irc_nick, irc_user,
		    irc_addr);
	}

	/*
	 * Loop through the protocols creating a seperate connection struct
	 * for each port/protocol.
	 */

	for (i = 0; i < SCAN_NUMPROTOCOLS; i++) {
		newconn = malloc(sizeof(*newconn));

		if (!newconn)
			scan_memfail();               

		newconn->addr = strdup(addr);
		newconn->irc_addr = strdup(irc_addr);
		newconn->irc_nick = strdup(irc_nick);
		newconn->irc_user = strdup(irc_user);
		/* This is allocated later on in scan_establish to save on
		 * memory.
		 */
		newconn->data = 0;
		newconn->verbose = verbose;
		newconn->bytes_read = 0; 
		newconn->fd = 0;
     		newconn->aftype = aftype;
		if (conn_notice)
			newconn->conn_notice = strdup(conn_notice);
		else
			newconn->conn_notice = 0;

		/*
		 * Give struct a link to information about the protocol it
		 * will be handling.
		 */
		newconn->protocol = &(SCAN_PROTOCOLS[i]);

        memset(&(newconn->sockaddr), 0, sizeof(struct bopm_sockaddr));

#ifdef IPV6
        if (aftype == AF_INET6) {
		/* Fill in sockaddr with information about remote host */
		newconn->sockaddr.sas.sa6.sin6_family = AF_INET6;
		newconn->sockaddr.sas.sa6.sin6_port =
		    htons(newconn->protocol->port);
		inetpton(aftype, addr, &newconn->sockaddr.sas.sa6.sin6_addr);
        } else {
#endif   
		/* Fill in sockaddr with information about remote host */
		newconn->sockaddr.sas.sa4.sin_family = AF_INET;
		newconn->sockaddr.sas.sa4.sin_port =
		    htons(newconn->protocol->port);
		inetpton(aftype, addr, &newconn->sockaddr.sas.sa4.sin_addr);
#ifdef IPV6
        }
#endif
		newconn->state = STATE_UNESTABLISHED;

		/* Add struct to list of connections. */
		scan_add(newconn);

		/* If we have available FD's, overide queue. */
		if (FD_USE < CONF_FDLIMIT)
			scan_establish(newconn);
		else if (OPT_DEBUG >= 3) {
			log("SCAN -> File Descriptor limit (%d) reached, "
			    "queuing scan for %s", CONF_FDLIMIT,
			    newconn->addr);
		}
	}
}

/*
 * Get FD for new socket, bind to interface and connect() (non blocking) then
 * set conn to ESTABLISHED for write check.
 */
static void scan_establish(scan_struct *conn)
{
	/* For local bind() */
	struct bopm_ircaddr SCAN_LOCAL;
 	struct bopm_sockaddr bsaddr;

	memset(&SCAN_LOCAL, 0, sizeof(struct sockaddr_in));
	memset(&bsaddr, 0, sizeof(struct bopm_sockaddr));

	/* Setup SCAN_LOCAL for local bind() */
	if (CONF_BINDSCAN) {
#ifdef IPV6
		if (bindto_ipv6) {
			if (!inetpton(AF_INET6, CONF_BINDSCAN,
			    &(SCAN_LOCAL.ins.in6.s6_addr))) {
				log("IRC -> bind(): %s is an invalid address",
				    CONF_BINDIRC);
				exit(EXIT_FAILURE);
			}
			copy_s_addr(bsaddr.sas.sa6.sin6_addr.s6_addr,
			    SCAN_LOCAL.ins.in6.s6_addr);
			bsaddr.sas.sa6.sin6_family = AF_INET6;
                        bsaddr.sas.sa6.sin6_port = htons(0);
		} else {
#endif
			if (!inetpton(AF_INET, CONF_BINDSCAN,
			    &(SCAN_LOCAL.ins.in4.s_addr))) {
				log("IRC -> bind(): %s is an invalid address",
				    CONF_BINDIRC);
				exit(EXIT_FAILURE);
			}
			bsaddr.sas.sa4.sin_addr.s_addr =
			    SCAN_LOCAL.ins.in4.s_addr;
			bsaddr.sas.sa4.sin_family = AF_INET;
                        bsaddr.sas.sa4.sin_port = htons(0);
		}
#ifdef IPV6
	}
#endif

	/* Request file descriptor for socket. */
	conn->fd = socket(conn->aftype, SOCK_STREAM, 0);

        /* Increase global FD Use counter. */
        FD_USE++;

	/* If error, mark connection for close. */
	if (conn->fd == -1) {
		log("SCAN -> Error allocating file descriptor.");
		conn->state = STATE_CLOSED;
		return;
	}

	/* Bind to specific interface designated in conf file. */
	if (CONF_BINDSCAN) {
		int retbind = 0;
#ifdef IPV6
		if (bindto_ipv6)
			retbind = bind(conn->fd, (struct sockaddr *) &(bsaddr.sas.sa6), sizeof(bsaddr));
		else 
#else
			retbind = bind(conn->fd, (struct sockaddr *) &(bsaddr.sas.sa4), sizeof(bsaddr));
#endif
		if (retbind == -1) {
			switch (errno) {
			case EACCES:
				log("SCAN -> bind(): No access to bind to %s",
				    CONF_BINDSCAN);
				break;
			default:
				log("SCAN -> bind(): Error binding to %s",
				    CONF_BINDSCAN);
				break;

			}
			exit(EXIT_FAILURE);
		}
	}

	/* Log create time of connection for timeouts. */
	time(&(conn->create_time));
	/* Flag conn established (for write). */
	conn->state = STATE_ESTABLISHED;
	/* Set socket non blocking. */
	fcntl(conn->fd, F_SETFL, O_NONBLOCK);
	/* Connect! */
	connect(conn->fd, (struct sockaddr *) &(conn->sockaddr),
	    sizeof(conn->sockaddr));

	/* Allocate memory for the scan buffer. */
	conn->data = malloc((SCANBUFFER + 1) * sizeof(char));
	conn->datasize = 0;

}


/*
 * Pass one cycle to the proxy scanner so it can do neccessary functions like
 * testing for sockets to be written to and read from.
 */

void scan_cycle(void)
{
	scan_check();
}


/*
 * Test for sockets to be written/read to.
 */

static void scan_check(void)
{
#ifdef HAVE_SYS_POLL_H
	/* MAX_POLL is defined in options.h */
	static struct pollfd ufds[MAX_POLL];
	unsigned long size, i;
#else /* select() */
	fd_set w_fdset;
	fd_set r_fdset;
	struct timeval scan_timeout;
	int highfd;
#endif /* HAVE_SYS_POLL_H */

	struct scan_struct *ss;
  
	if (!CONNECTIONS)
		return;

#ifdef HAVE_SYS_POLL_H
	size = 0;

	/* Get size of list we're interested in. */
	for (ss = CONNECTIONS; ss; ss = ss->next) {
		if (ss->state != STATE_CLOSED &&
		    ss->state != STATE_UNESTABLISHED)
			size++;
	}
    
	i = 0;

	/* Setup each element now. */
	for (ss = CONNECTIONS; ss; ss = ss->next) {
		if (ss->state == STATE_CLOSED ||
		    ss->state == STATE_UNESTABLISHED)
			continue;

		ufds[i].events = 0;
		ufds[i].revents = 0;
		ufds[i].fd = ss->fd;

		/* Check for HUNG UP. */
		ufds[i].events |= POLLHUP;
		/* Check for INVALID FD */
		ufds[i].events |= POLLNVAL;

		switch (ss->state) {
		case STATE_ESTABLISHED:
			/* Check for NO BLOCK ON WRITE. */
			ufds[i].events |= POLLOUT;
			break;
		case STATE_SENT:
			/* Check for data to be read. */
			ufds[i].events |= POLLIN;
			break;
		}

		if (++i >= MAX_POLL)
			break;
	}

#else /* select() */
	FD_ZERO(&w_fdset);
	FD_ZERO(&r_fdset);
	highfd = 0;

	/* Add connections to appropriate sets. */

	for (ss = CONNECTIONS; ss; ss = ss->next) {
		if (ss->state == STATE_ESTABLISHED) {
			if(ss->fd > highfd)    
				highfd = ss->fd;

			FD_SET(ss->fd, &w_fdset);
			continue;
		}
         
		if (ss->state == STATE_SENT) {
			if (ss->fd > highfd)
				highfd = ss->fd;
             
			FD_SET(ss->fd, &r_fdset);
		}
	}

	/* No timeout. */
	scan_timeout.tv_sec = 0;
	scan_timeout.tv_usec= 0;

#endif /* HAVE_SYS_POLL_H */


#ifdef HAVE_SYS_POLL_H
	switch (poll(ufds, size, 0)) {
#else /* select() */
	switch (select((highfd + 1), &r_fdset, &w_fdset, 0, &scan_timeout)) {
#endif /* HAVE_SYS_POLL_H */
	case -1:
		/* error in select/poll */
		return;
	case 0:
		break;
	default:
		/* Pass pointer to connection to handler. */

#ifdef HAVE_SYS_POLL_H
		for (ss = CONNECTIONS; ss; ss = ss->next) {
			for (i = 0; i < size; i++) {
				if (ufds[i].fd == ss->fd) {
					if (ufds[i].revents & POLLIN)
						scan_readready(ss);

					if (ufds[i].revents & POLLOUT)
						scan_writeready(ss);
             
					if (ufds[i].revents & POLLHUP)
						scan_negfail(ss);

					break;
				}
			}
		}
#else

		for (ss = CONNECTIONS; ss; ss = ss->next) {
			if ((ss->state == STATE_ESTABLISHED) &&
			    FD_ISSET(ss->fd, &w_fdset))
				scan_writeready(ss);

			if ((ss->state == STATE_SENT) &&
			    FD_ISSET(ss->fd, &r_fdset))                    
				scan_readready(ss);     
		}               
#endif /* HAVE_SYS_POLL_H */
	}
}


/*
 * Negotiation failed - Read returned false, we discard the connection as a
 * closed proxy to save CPU.                                                     
 */
static void scan_negfail(scan_struct *conn)
{
	if (conn->verbose) {
		irc_send("PRIVMSG %s :%s (%d): Connection to %s closed, "
		    "negotiation failed (%d bytes read)", CONF_CHANNELS,
		    conn->protocol->type, conn->protocol->port,
		    conn->irc_addr, conn->bytes_read);
	}
	conn->state = STATE_CLOSED;
}

/*
 * Poll or select returned back that this connection is ready for read.
 */
static void scan_readready(scan_struct *conn)
{
	char c;

	while(1) {
		switch (read(conn->fd, &c, 1)) {
		case  0:
		case -1:
			return;

		default:
			conn->bytes_read++;
			if (c == 0 || c == '\r')
				continue;
                          
			if(c == '\n') {
				conn->data[conn->datasize] = 0;
				conn->datasize = 0;
				scan_read(conn);              
				continue;
			}
			
			/* Avoid freezing from reading endless data. */
			if (conn->bytes_read >= MAXREAD) {
				conn->state = STATE_CLOSED;
				return;
			}

			if (conn->datasize < SCANBUFFER) {
				/* -1 to pad for null term. */
				conn->data[(++conn->datasize) - 1] = c;
			}
		}
	}
}

/*
 * Read one line in from remote, check line against target line.
 */
static void scan_read(scan_struct *conn)
{
	if (OPT_DEBUG >= 3)
		log("SCAN -> Checking data from %s [%s:%d] against "
		    "TARGET_STRING: %s", conn->addr, conn->protocol->type,
		    conn->protocol->port, conn->data);
	if (strstr(conn->data, CONF_TARGET_STRING))
		scan_openproxy(conn);
}

/*
 * Test proved positive for open proxy.
 */
static void scan_openproxy(scan_struct *conn)
{
	scan_struct *ss;

	irc_kline(conn->irc_addr, conn->addr);

	if (CONF_DNSBL_FROM && CONF_DNSBL_TO && CONF_SENDMAIL &&
	    !conn->verbose)                
		dnsbl_report(conn);
                
	log("OPEN PROXY -> %s: %s!%s@%s (%d)", conn->protocol->type,
	    conn->irc_nick, conn->irc_user, conn->irc_addr,
	    conn->protocol->port);

	irc_send("PRIVMSG %s :%s (%d): OPEN PROXY -> %s!%s@%s",
	    CONF_CHANNELS, conn->protocol->type, conn->protocol->port,
		conn->irc_nick, conn->irc_user, conn->irc_addr);

	/* Increase number OPEN (insecure) of this type. */
	conn->protocol->stat_numopen++;

	conn->state = STATE_CLOSED;

	/*
	 * Flag connections with the same addr CLOSED aswell, but only if this
	 * is not a verbose check.  When it is verbose/manual, we care about
	 * all types of proxy.  When it is automatic (i.e. when a user
	 * connects) we want them killed quickly sow e can move on.
	 */
#if 0
	if (!conn->verbose) {
		for (ss = CONNECTIONS;ss;ss = ss->next) {
			if (!strcmp(conn->irc_addr, ss->irc_addr)) /* TimeMr14C */
				ss->state = STATE_CLOSED;
		}
	}
#endif
}

/*
 * Poll or select returned back that this connect is ready for write.
 */
static void scan_writeready(scan_struct *conn)
{
	/* If write returns true, flag STATE_SENT. */
	if ((*conn->protocol->w_handler)(conn))
		conn->state = STATE_SENT;

	/* Increase number attempted negotiated of this type. */
	conn->protocol->stat_num++;
}

/*
 * Link struct to connection list.
 */
static void scan_add(scan_struct *newconn)
{
	scan_struct *ss;

	/* Only item in list. */
         
	if (!CONNECTIONS) {
		newconn->next = 0;
		CONNECTIONS = newconn;
	} else {
		/* Link to end of list. */
		for(ss = CONNECTIONS; ss; ss = ss->next) {
			if (!ss->next) {
				newconn->next = 0;
				ss->next = newconn;
				break;
			}
		}
	}
}


/*
 * Unlink struct from connection list and free its memory.
 */
static void scan_del(scan_struct *delconn)
{
	scan_struct *ss;
	scan_struct *lastss;

	if (delconn->fd > 0) 
		close(delconn->fd);

	/* 1 file descriptor freed up for use. */
	if(delconn->fd)
        	FD_USE--;

	lastss = 0;

	for(ss = CONNECTIONS; ss; ss = ss->next) {
		if (ss == delconn) {     
			/* Link around deleted node */                                   
			if (lastss == 0)
				CONNECTIONS = ss->next;                     
			else
				lastss->next = ss->next;
                     
			free(ss->addr);
			free(ss->irc_addr);
			free(ss->irc_nick);
			free(ss->irc_user);

			if (ss->conn_notice)
				free(ss->conn_notice);

			/* If it's established, free the scan buffer. */
			if (delconn->data)
				free(delconn->data);

			free(ss);

			break;
		}

		lastss = ss;
	}
 
}

/*
 * Alarm signaled, loop through connections and remove any we don't need
 * anymore.
 */
void scan_timer()
{
	scan_struct *ss;
	scan_struct *nextss;
 
	time_t present;
	time(&present);

	/*
	 * Check for timed out connections and also check if queued
	 * (UNESTABLISHED) connections can be established now.
	 */

	for (ss = CONNECTIONS; ss;) {
		if (ss->state == STATE_UNESTABLISHED) { 
			if (FD_USE < CONF_FDLIMIT) {
				scan_establish(ss);
				if (OPT_DEBUG >= 3) {
					log("SCAN -> File descriptor free, "
					    "continuing queued scan on %s",
					    ss->addr);
				}
			} else {
				ss = ss->next;

				/*
				 * Continue to avoid timeout checks on an
				 * unestablished connection.
				 */
				continue;
			}
		}

		if (((present - ss->create_time) >= CONF_TIMEOUT) ||
		    (ss->state == STATE_CLOSED)) {
			/* State closed or timed out, remove */ 
			if (ss->verbose && (ss->state != STATE_CLOSED)) {
				if (ss->bytes_read) {
					irc_send("PRIVMSG %s :%s (%d): "
					    "Negotiation to %s timed out "
					    "(%d bytes read).",
					    CONF_CHANNELS,
					    ss->protocol->type,
					    ss->protocol->port,
					    ss->irc_addr, ss->bytes_read);
				} else {
					irc_send("PRIVMSG %s :%s (%d): "
					    "Negotiation to %s timed out "
					    "(No response).",
					    CONF_CHANNELS,
					    ss->protocol->type,
					    ss->protocol->port, 
					    ss->irc_addr);
				}
			}
 
			nextss = ss->next;
			scan_del(ss);
			ss = nextss;
			continue;
		}
   		ss = ss->next;
	}
}



/*
 * Function for handling open HTTP data.
 *
 * Return 1 on success.
 */
static int scan_w_squid(struct scan_struct *conn)
{
	snprintf(SENDBUFF, 128, "CONNECT %s:%d HTTP/1.0\r\n\r\n",
	    CONF_SCANIP, CONF_SCANPORT);
	send(conn->fd, SENDBUFF, strlen(SENDBUFF), 0);

    return(1);
}


/*
 * CONNECT request byte order for socks4
 *  
 *  		+----+----+----+----+----+----+----+----+----+----+....+----+
 *  		| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
 *  		+----+----+----+----+----+----+----+----+----+----+....+----+
 *   # of bytes:  1    1      2              4           variable       1
 *  						 
 *  VN = Version, CD = Command Code (1 is connect request)
 */
static int scan_w_socks4(struct scan_struct *conn)
{
	struct in_addr addr;
	unsigned long laddr;
	int len;
 
	if (inet_aton(CONF_SCANIP, &addr) == 0) {
		log("SCAN -> scan_w_socks4 : %s is not a valid IP",
		    CONF_SCANIP);
	}
    
	laddr = htonl(addr.s_addr);
 
	len = snprintf(SENDBUFF, 512, "%c%c%c%c%c%c%c%c%c",  4, 1,
	    (((unsigned short) CONF_SCANPORT) >> 8) & 0xFF,
	    (((unsigned short) CONF_SCANPORT) & 0xFF),
	    (char) (laddr >> 24) & 0xFF, (char) (laddr >> 16) & 0xFF,
	    (char) (laddr >> 8) & 0xFF, (char) laddr & 0xFF, 0);

	send(conn->fd, SENDBUFF, len, 0);
	return(1);
}

/*
 * Send version authentication selection message to socks5
 *
 *       +----+----------+----------+
 *       |VER | NMETHODS | METHODS  |
 *       +----+----------+----------+
 *       | 1  |    1     | 1 to 255 |
 *       +----+----------+----------+
 *                                                                                               
 *  VER always contains 5, for socks version 5
 *  Method 0 is 'No authentication required'
 *
 *
 *
 *  The SOCKS request is formed as follows:
 *
 *        +----+-----+-------+------+----------+----------+
 *       |VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
 *       +----+-----+-------+------+----------+----------+
 *       | 1  |  1  | X'00' |  1   | Variable |    2     |
 *       +----+-----+-------+------+----------+----------+
 *
 *     Where:
 *
 *         o  VER    protocol version: X'05'
 *         o  CMD
 *            o  CONNECT X'01'
 *            o  BIND X'02'
 *            o  UDP ASSOCIATE X'03'
 *         o  RSV    RESERVED
 *         o  ATYP   address type of following address
 *            o  IP V4 address: X'01'
 *            o  DOMAINNAME: X'03'
 *            o  IP V6 address: X'04'
 *         o  DST.ADDR       desired destination address
 *         o  DST.PORT desired destination port in network octet
 *            order
 *
 *
 */

static int scan_w_socks5(struct scan_struct *conn)
{
	
        struct in_addr addr;
        unsigned long laddr;
        int len;

        if (inet_aton(CONF_SCANIP, &addr) == 0) {
                log("SCAN -> scan_w_socks4 : %s is not a valid IP",
                    CONF_SCANIP);
        }

        laddr = htonl(addr.s_addr);

        /* Form authentication string */
	/* Version 5, 1 number of methods, 0 method (no auth). */
	len = snprintf(SENDBUFF, 512, "%c%c%c", 5, 1, 0);
	send(conn->fd, SENDBUFF, len, 0);
  
        /* Form request string */

        /* Will need to write ipv6 support here in future 
         * as socks5 is ipv6 compatible
         */

        len = snprintf(SENDBUFF, 512, "%c%c%c%c%c%c%c%c%c%c", 5, 1, 0, 1,
            (char) (laddr >> 24) & 0xFF, (char) (laddr >> 16) & 0xFF,
            (char) (laddr >> 8) & 0xFF, (char) laddr & 0xFF, 
            (((unsigned short) CONF_SCANPORT) >> 8) & 0xFF,
            (((unsigned short) CONF_SCANPORT) & 0xFF)
                      );

        send(conn->fd, SENDBUFF, len, 0);
	return(1);
}

/*
 * Cisco scanning
 *
 * Some cisco routers have 'cisco' set as password which allow open telnet
 * relay. Attempt to connect using cisco as a password, then give command for
 * telnet to the scanip/scanport
 */
static int scan_w_cisco(struct scan_struct *conn)
{
	int len;

	len = snprintf(SENDBUFF, 512, "cisco\r\n");
	send(conn->fd, SENDBUFF, len, 0); 

	len = snprintf(SENDBUFF, 512, "telnet %s %d\r\n", CONF_SCANIP,
	    CONF_SCANPORT);
	send(conn->fd, SENDBUFF, len, 0);

	return(1);
}


/*
 * Open wingates require no authentication, they will send a prompt when
 * connect. No need to send any data.
 */
static int scan_w_wingate(struct scan_struct *conn)
{
     int len;
 
	len = snprintf(SENDBUFF, 512, "%s:%d\r\n", CONF_SCANIP,
	    CONF_SCANPORT);
	send(conn->fd, SENDBUFF, len, 0);

	return(1);
}


/*
 * Manually check a host for proxies.
 */
void do_manual_check(struct command *c)
{
	struct hostent *he;
	char *ip;

	if (!(he = gethostbyname(c->param))) {
		switch (h_errno) {
		case HOST_NOT_FOUND:
			irc_send("PRIVMSG %s :Host '%s' is unknown, dummy.",
			    c->target, c->param);
			return;
		case NO_ADDRESS:
			irc_send("PRIVMSG %s :The specified name '%s' "
			    "exists, but has no address.", c->target,
			    c->param);
			return;
		case NO_RECOVERY:
			irc_send("PRIVMSG %s :An unrecoverable error "
			    "occured whilst resolving '%s'.", c->target,
			    c->param);
			return;
		case TRY_AGAIN:
			irc_send("PRIVMSG %s :A temporary nameserver "
			    "error occurred.", c->target);
			return;
		default:
			irc_send("PRIVMSG %s :Unknown error resolving "
			    "'%s' (sorry!)", c->target, c->param);
			return;
		}
	}

	ip = inet_ntoa(*((struct in_addr *) he->h_addr));

	irc_send("PRIVMSG %s :Checking %s [%s] for open proxies at "
	    "request of %s...", CONF_CHANNELS, c->param, ip, c->nick);

    	if (CONF_DNSBL_ZONE && he->h_addrtype != AF_INET6)
            dnsbl_check(ip, "*", "*", c->param);

	/* Scan using verbose. */
	scan_connect(ip, c->param, "*", "*", 1, he->h_addrtype, 0);
}

