/*
 * Copyright (C) 2002  Erik Fears
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to
 *
 *       The Free Software Foundation, Inc.
 *       59 Temple Place - Suite 330
 *       Boston, MA  02111-1307, USA.
 *
 *
 */

#include <stdio.h>
#include "extern.h"
#include "config.h"
#include "malloc.h"
#include "log.h"
#include "scan.h"

void config_setup();
void config_init();



/* Configuration     */

struct OptionsConf *OptionsItem = NULL;
struct IRCConf *IRCItem = NULL;
list_t *UserItemList = NULL;
list_t *ScannerItemList = NULL;

/* End Configuration */



/* Rehash or load new configuration from filename, via flex/bison parser */
void config_load(const char *filename)
{

    config_init();
    config_setup(); /* Setup/clear current configuration */

    log("CONFIG -> Loading %s", filename);

    if((yyin = fopen(filename, "r")) == NULL)
    {
        log("CONFIG -> Error opening %s", filename);
        exit(1);
    }

    yyparse();

    scan_init();       /* Initialize the scanners once we have the configuration */
}

/* Malloc and initialize configuration data to NULL */
void config_init()
{
    /* Init IRC block */
    IRCItem = (struct IRCConf *) MyMalloc(sizeof(struct IRCConf));
    memset(IRCItem, 0, sizeof(struct IRCConf));
    IRCItem->channels = list_create();

    /* Init Options block */
    OptionsItem = (struct OptionsConf *) MyMalloc(sizeof(struct OptionsConf));
    memset(OptionsItem, 0, sizeof(struct OptionsConf));

    /* Init list of User blocks */
    UserItemList = list_create();

    /* Init list of Scanner blocks */
    ScannerItemList = list_create();
}

/* Setup structs that hold configuration data and then reset default values */

void config_setup()
{

    /* Setup IRC Block Defaults */
    IRCItem->away = DupString("I'm a bot, don't message me");
    IRCItem->mode = DupString("+cs");
    IRCItem->nick = DupString("bopm");
    IRCItem->password = DupString("");
    IRCItem->port = 6667;
    IRCItem->oper = DupString("undefined");
    IRCItem->username = DupString("bopm");
    IRCItem->realname = DupString("Blitzed Open Proxy Monitor");
    IRCItem->server = DupString("myserver.blitzed.org");
    IRCItem->vhost = DupString("");
    IRCItem->connregex = DupString("\\*\\*\\* Notice -- Client connecting: ([^ ]+) \\(([^@]+)@([^\\)]+)\\) \\[([0-9\\.]+)\\].*");


    /* Setup options block defaults */
    OptionsItem->negcache = 0;   /* 0 disabled negcache */
    OptionsItem->pidfile = DupString("bopm.pid");
}

void yyerror(const char *str)
{
    log("CONFIG -> %s: line %d", str, linenum);
    exit(EXIT_FAILURE);
}
