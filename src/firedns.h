/*
firedns.h - firedns library declarations
Copyright (C) 2002 Ian Gulliver

This file has been edited for use in BOPM - see the real library at
http://ares.penguinhosting.net/~ian/ before you judge firedns based
on this..

This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _FIREDNS_H
#define _FIREDNS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef AF_INET6
struct in6_addr {
   unsigned char   s6_addr[16];
};
#endif

#define FDNS_MAX              8                    /* max number of nameservers used */
#define FDNS_CONFIG_PREF     "/etc/firedns.conf"   /* preferred firedns config file */
#define FDNS_CONFIG_FBCK     "/etc/resolv.conf"    /* fallback config file */
#define FDNS_PORT            53                    /* DNS well known port */
#define FDNS_QRY_A            1                    /* name to IP address */
#define FDNS_QRY_AAAA        28                    /* name to IP6 address */

#define FDNS_ERR_NONE        0                     /* Success */
#define FDNS_ERR_FORMAT      1                     /* Format error */
#define FDNS_ERR_SERVFAIL    2                     /* Server failure */
#define FDNS_ERR_NXDOMAIN    3                     /* Name error */
#define FDNS_ERR_NOIMPT      4                     /* Not implemented */
#define FDNS_ERR_REFUSED     5                     /* Refused */
#define FDNS_ERR_TIMEOUT     6                     /* Timeout - local */
#define FDNS_ERR_OTHER       7                     /* Other error */

/* Used with the above error values */
extern int fdns_errno;
extern int fdns_fdinuse;

void firedns_init();

struct firedns_result {
   char text[1024];
   char lookup[256];
   void *info;
};

/* non-blocking functions */
int firedns_getip4(const char * const name, void *info);
int firedns_getip6(const char * const name, void *info);
struct firedns_result *firedns_getresult(const int fd);

/* low-timeout blocking functions */
struct in_addr *firedns_resolveip4(const char * const name);
struct in6_addr *firedns_resolveip6(const char * const name);

void firedns_cycle(void);

#endif
