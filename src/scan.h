#ifndef SCAN_H
#define SCAN_H

#include "libopm/src/opm.h"

struct scan_struct
{
   char *irc_nick;
   char *irc_username;
   char *irc_hostname;

   char *ip;
   char *proof;
   OPM_REMOTE_T *remote;

   unsigned short scans;
   unsigned short positive;

   struct ChannelConf *manual_target;
};


struct mask_struct
{
   char  *mask;
   struct scanner_struct *scs;
};

struct scanner_struct
{
   char *name;
   OPM_T *scanner;
};

struct protocol_assoc
{
   int type;
   char *name;
};

extern void scan_init();
extern char *scan_gettype(int);
extern void scan_cycle();
extern void scan_connect(char **, char *);
extern void scan_checkfinished(struct scan_struct *);
extern void scan_manual(char *, struct ChannelConf *);

struct kline_format_assoc
{
   char key;
   void *data;
   int type;
};

#define FORMATTYPE_STRING 1

#endif /* SCAN_H */
