#ifndef EXTERN_H
#define EXTERN_H

    #include "config.h"

    extern int h_errno;

    extern char *CONF_SERVER;
    extern char *CONF_USER;
    extern char *CONF_NICK;
    extern char *CONF_OPER;
    extern char *CONF_REASON;
    extern char *CONF_SCANIP;
    extern char *CONF_BINDIRC;
    extern char *CONF_BINDSCAN;
    extern char *CONF_CHANNELS;
    extern char *CONF_NICKSERV_IDENT;
    extern char *CONF_CHANSERV_INVITE;
    extern char *CONF_KLINE_COMMAND;
    extern char *CONF_DNSBL_ZONE;
    extern char *CONF_DNSBL_FROM;
    extern char *CONF_DNSBL_TO;
    extern char *CONF_SENDMAIL;
    extern char *CONF_HELP_EMAIL;
    extern char *CONF_AWAY;

    extern int   CONF_PORT;
    extern int   CONF_SCANPORT;

    extern int   OPT_DEBUG;
 
    extern    time_t STAT_START_TIME;
    extern    int STAT_NUM_CONNECTS;
#endif    
