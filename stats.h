#ifndef STATS_H
#define STATS_H
	time_t STAT_START_TIME;
	unsigned int STAT_NUM_CONNECTS;
	unsigned int STAT_NUM_WINGATE;
	unsigned int STAT_NUM_WINGATE_OPEN;
	unsigned int STAT_NUM_SOCKS4;
	unsigned int STAT_NUM_SOCKS4_OPEN;
	unsigned int STAT_NUM_SOCKS5;
	unsigned int STAT_NUM_SOCKS5_OPEN;
	unsigned int STAT_NUM_HTTP;
	unsigned int STAT_NUM_HTTP_OPEN;

	void do_stats_init();
	void do_stats(const char *target);
#endif
