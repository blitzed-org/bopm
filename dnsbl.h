#ifndef DNSBL_H
#define DNSBL_H

	int dnsbl_check(const char *addr, const char *irc_nick,
			const char *irc_user, char *irc_addr);
	void dnsbl_report(struct scan_struct *ss);
#endif
