#ifndef EXTERN_H
#define EXTERN_H

extern int h_errno;

extern char *CONF_SERVER;
extern char *CONF_PASSWORD;
extern char *CONF_USER;
extern char *CONF_NICK;
extern char *CONF_REALNAME;
extern char *CONF_OPER;
extern char *CONF_OPER_MODES;
extern char *CONF_REASON;
extern char *CONF_SCANIP;
extern char *CONF_BINDIRC;
extern char *CONF_BINDSCAN;
extern char *CONF_CHANNELS;
extern char *CONF_KEYS;
extern char *CONF_NICKSERV_IDENT;
extern char *CONF_CHANSERV_INVITE;
extern char *CONF_KLINE_COMMAND;
extern char *CONF_DNSBL_ZONE;
extern char *CONF_DNSBL_FROM;
extern char *CONF_DNSBL_TO;
extern char *CONF_SENDMAIL;
extern char *CONF_HELP_EMAIL;
extern char *CONF_AWAY;
extern char *CONF_TARGET_STRING;
extern char *CONF_PIDFILE;
extern string_list *CONF_EXCLUDE;
extern string_list *CONF_SCAN_WARNING;

extern unsigned int CONF_PORT;
extern unsigned int CONF_SCANPORT;
extern unsigned int CONF_FDLIMIT;
extern unsigned int CONF_TIMEOUT;

extern unsigned int OPT_DEBUG;
 
extern time_t STAT_START_TIME;
extern unsigned int STAT_NUM_CONNECTS;

#endif    
