#ifndef DNSBL_H
#define DNSBL_H

#include "firedns.h"
#include "scan.h"

extern void dnsbl_add(struct scan_struct *);
extern void dnsbl_result(struct firedns_result *);
extern void dnsbl_cycle(void);
extern void dnsbl_report(struct scan_struct *ss);

/* Bitmasks used in final octet of IP address */
#define DNSBL_TYPE_WG 1
#define DNSBL_TYPE_SOCKS 2
#define DNSBL_TYPE_HTTP 4
#define DNSBL_TYPE_CISCO 8
#define DNSBL_TYPE_HTTPPOST 16

#define DNSBL_LOOKUPLEN 82

#endif
