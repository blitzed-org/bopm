#ifndef DNSBL_H
#define DNSBL_H

extern int dnsbl_check(const char *addr, const char *irc_nick,
    const char *irc_user, char *irc_addr);
extern void dnsbl_report(struct scan_struct *ss);

/* Bitmasks used in final octet of IP address */
#define DNSBL_TYPE_WG 1
#define DNSBL_TYPE_SOCKS 2
#define DNSBL_TYPE_HTTP 4
#define DNSBL_TYPE_CISCO 8
#define DNSBL_TYPE_HTTPPOST 16

#endif
