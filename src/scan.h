#ifndef SCAN_H
#define SCAN_H

struct scan_struct
{
   char *irc_nick;
   char *irc_username;
   char *irc_hostname;

   char *ip;
   /* OPM_REMOTE_T *remote; */

   unsigned short scans;
};


void scan_connect(char **user, char *msg);

#endif /* SCAN_H */
