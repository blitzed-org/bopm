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
void scan_connect(char **user, char *msg);

#endif /* SCAN_H */
