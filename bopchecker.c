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
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef STDC_HEADERS
#include <stdlib.h>
#include <string.h>
#endif

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "irc.h"
#include "opercmd.h"
#include "scan.h"
#include "bopchecker.h"
#include "options.h"
#include "config.h"

extern struct scan_struct *CONNECTIONS;
extern struct config_hash hash[];

int OPT_DEBUG = 1;
char *CONFNAME = DEFAULTNAME;
char *CONFFILE;
int RC = 0;

int main(int argc, char **argv)
{
	struct hostent *he;
	char *ip, *host;
	struct scan_struct *ss;
	int len, c, i, still_alive = 0;

	while (1) {
		c = getopt(argc, argv, "+c:");

		if (c == -1)
			break;

		switch (c) {
			case 'c':
				CONFNAME = strdup(optarg);
				break;
			case '?':
			default:
				/* unknown arg, just do nothing */
				break;
		}
	}

	if (argc - optind != 1) {
		usage(argv);
		exit(0);
	}

	signal(SIGPIPE, SIG_IGN);

	len = strlen(CONFNAME) + strlen(CONFEXT) + 2;
	CONFFILE = (char *) malloc(len * sizeof(char));
	snprintf(CONFFILE, len, "%s.%s", CONFNAME, CONFEXT);

	/* The only things we need in a conf file are SCANIP and
	 * SCANPORT */
	for (i = 0; hash[i].key; i++) {
		if (strcasecmp(hash[i].key, "SCANIP") != 0 &&
		    strcasecmp(hash[i].key, "SCANPORT") != 0 &&
		    strcasecmp(hash[i].key, "TARGET_STRING") != 0) {
			/* nuke the required field */
			hash[i].req = 0;
		}
	}

	config_load(CONFFILE);
	do_scan_init();

	if (optind > 0)
		host = argv[optind];
	else
		host = argv[1];

	if(!(he = gethostbyname(host))) {
		switch(h_errno) {
			case HOST_NOT_FOUND:
				fprintf(stderr, "Host '%s' is unknown.\n",
					host);
				exit(0);
			case NO_ADDRESS:
				fprintf(stderr, "The specified name '%s' "
					"exists, but has no address.\n",
					host);
				exit(0);
			case NO_RECOVERY:
				fprintf(stderr, "An unrecoverable error "
					"occured whilst resolving '%s'.\n",
					host);
				exit(0);
			case TRY_AGAIN:
				fprintf(stderr, "A temporary error "
					"occurred on an authoritative name "
					"server.\n");
				exit(0);
			default:
				fprintf(stderr, "Unknown error resolving "
					"'%s'.\n", host);
				exit(0);
		}
	}

	ip = inet_ntoa(*((struct in_addr *) he->h_addr));

	fprintf(stderr, "Checking %s [%s] for open proxies\n", host, ip);

	scan_connect(ip, host, "*", "*", 1, 0);    /* Scan using verbose */

	do {
		still_alive = 0;

		scan_cycle();
		/* pause 1s */
		sleep(1);

		for (ss = CONNECTIONS; ss; ss = ss->next) {
			if (ss->state != STATE_CLOSED) {
				time_t now = time(NULL);
				if ((now - ss->create_time) >= 30) {
					/* timed out */
					ss->state = STATE_CLOSED;
				} else {
					still_alive = 1;
					break;
				}
			}
		}
	} while (still_alive);

	/* All connections now closed, check what we got */
	for (ss = CONNECTIONS; ss; ss = ss->next) {
		if (ss->protocol->stat_numopen) {
			if (strcasecmp("http", ss->protocol->type) == 0)
				RC |= PROXY_HTTP;
			else if (strcasecmp("socks4", ss->protocol->type) == 0)
				RC |= PROXY_SOCKS4;
			else if (strcasecmp("socks5", ss->protocol->type) == 0)
				RC |= PROXY_SOCKS5;
			else if (strcasecmp("wingate", ss->protocol->type) == 0)
				RC |= PROXY_WINGATE;
			else {
				fprintf(stderr, "Unknown type %s!", ss->protocol->type);
			}
		}
	}
	exit(RC);
}

void usage(char **argv)
{
	fprintf(stderr, "Usage: %s [-c configname] <host>\n", argv[0]);
}

void log(char *data,...)
{
	va_list arglist;

	char data2[513];

	va_start(arglist, data);
	vsnprintf(data2, 512, data, arglist);
	va_end(arglist);

	fprintf(stderr, "%s\n", data2);
}

void irc_kline(char *addr, char *ip)
{
}

void dnsbl_report(struct scan_struct *ss)
{
}

void irc_send(char *data, ...)
{
}

int dnsbl_check(const char *addr, const char *irc_nick,
		const char *irc_user, char *irc_addr)
{
	return(0);
}
