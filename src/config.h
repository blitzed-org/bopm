#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include "list.h"

extern FILE *yyin;

extern char linebuf[512];
extern int  linenum;

extern void yyerror(const char *);
extern void config_load(const char *);


/* structs to hold data */

struct IRCConf
{
    char *nick;
    char *username;
    char *realname;

    char *server;
    int   port;
    char *password;

    char *vhost;

    char *channels;
    char *keys;

    char *oper;
    char *mode;

    char *away;
};

struct UserConf
{
    list_t *masks;
    list_t *scanners;
};

struct ScannerConf
{
    char   *name;

    list_t *protocols;
    char   *vhost;

    int     fd;

    char   *target_ip;
    int    *target_port;
    char   *target_string;
};

/* Extern to actual config data declared in config.c */
extern struct IRCConf *IRCItem;
extern list_t *UserItemList;

#endif /* CONFIG_H */
