#ifndef STATS_H
#define STATS_H

#include "irc.h"

struct StatsHash
{
   int type;
   unsigned int count;
   char *name;
};

extern void stats_init(void);
extern void stats_openproxy(int);
extern void stats_connect(void);
extern void stats_dnsblrecv(void);
extern void stats_dnsblsend(void);
extern void stats_output(char *);

extern void fdstats_output(char *);

#endif /* STATS_H */
