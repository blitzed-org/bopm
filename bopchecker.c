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
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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

int OPT_DEBUG = 1;

int main(int argc, char **argv)
{
	struct hostent *he;
	char *ip;
	struct scan_struct *ss;
	
	if (argc != 2) {
		usage(argv);
		return(EXIT_FAILURE);
	}

	signal(SIGPIPE, SIG_IGN);

	config_load(LOGFILE);
	do_scan_init();

	if(!(he = gethostbyname(argv[1]))) {
		switch(h_errno) {
			case HOST_NOT_FOUND:
				fprintf(stderr, "Host '%s' is unknown.",
					argv[1]);
				return(1);
			case NO_ADDRESS:
				fprintf(stderr, "The specified name '%s' "
					"exists, but has no address.",
					argv[1]);
				return(1);
			case NO_RECOVERY:
				fprintf(stderr, "An unrecoverable error "
					"occured whilst resolving '%s'.",
					argv[1]);
				return(1);
			case TRY_AGAIN:
				fprintf(stderr, "A temporary error "
					"occurred on an authoritative name "
					"server.");
				return(1);
			default:
				fprintf(stderr, "Unknown error resolving "
					"'%s'.", argv[1]);
				return(EXIT_FAILURE);
		}
	}

	ip = inet_ntoa(*((struct in_addr *) he->h_addr));

	fprintf(stderr, "Checking %s [%s] for open proxies\n", argv[1], ip);

	scan_connect(ip, argv[1], "*", "*", 1, 0);    /* Scan using verbose */

	do {
		int still_alive = 0;
		
		scan_cycle();

		for (ss = CONNECTIONS; ss; ss = ss->next) {
			if (ss->protocol->stat_numopen)
				return(EXIT_SUCCESS);
			if (ss->state != STATE_CLOSED)
				still_alive++;
		}

		if (!still_alive)
			return(EXIT_FAILURE);
	} while(1);
}

void usage(char **argv)
{
	fprintf(stderr, "Usage: %s <host>\n", argv[0]);
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

void irc_kline(char *addr)
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
