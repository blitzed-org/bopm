#ifndef BOPCHECKER_H
#define BOPCHECKER_H

#define PROXY_HTTP    0x10
#define PROXY_WINGATE 0x20
#define PROXY_SOCKS4  0x40
#define PROXY_SOCKS5  0x80

void usage(char **argv);
void log(char *data, ...);
void irc_kline(char *addr, char *ip);
void dnsbl_report(struct scan_struct *ss);
void irc_send(char *data, ...);
int dnsbl_check(const char *addr, const char *irc_nick,
    const char *irc_user, char *irc_addr);

#endif
