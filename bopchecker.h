#ifndef BOPCHECKER_H
#define BOPCHECKER_H

void usage(char **argv);
void log(char *data,...);
void irc_kline(char *addr);
void dnsbl_report(struct scan_struct *ss);
void irc_send(char *data, ...);
int dnsbl_check(const char *addr, const char *irc_nick,
		const char *irc_user, char *irc_addr);
#endif
