#ifndef STATS_H
#define STATS_H
	time_t STAT_START_TIME;

	void do_stats_init();
	void do_stats(const char *target);
	char *dissect_time(time_t time);
#endif
