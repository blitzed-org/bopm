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


%{
#include <stdio.h>
#include <string.h>
#include "malloc.h"
#include "config.h"

//int yydebug=1; 
void *tmp;        /* Variable to temporarily hold nodes before insertion to list */

%}

%token AWAY
%token CHANNEL
%token IRC
%token KEY
%token MASK
%token MODE
%token NAME
%token NEGCACHE
%token NICK
%token OPER
%token OPTIONS
%token PIDFILE
%token PASSWORD
%token PORT
%token REALNAME
%token SCANNER
%token SERVER
%token USERNAME
%token USER
%token VHOST

%union 
{
        int number;
        char *string;
}

%token <number> NUMBER
%token <string> STRING

%%

config:     /* empty */
          |config config_items
          ;

config_items: irc_entry |
              options_entry |
              user_entry;


/*************************** OPTIONS BLOCK ***********************/

options_entry: OPTIONS '{' options_items '}' ';';

options_items: options_items options_item |
               options_item;

options_item: options_negcache |
              options_pidfile |
              error;

options_negcache: NEGCACHE '=' NUMBER ';'
{
   OptionsItem->negcache = $3;
};

options_pidfile: PIDFILE '=' STRING ';'
{
   MyFree(OptionsItem->pidfile);
   OptionsItem->pidfile = DupString($3);
};

/*************************** IRC BLOCK ***************************/

irc_entry: IRC '{' irc_items  '}' ';';

irc_items: irc_items irc_item |
           irc_item;

irc_item: irc_away     |
          irc_nick     |
          irc_mode     |
          irc_oper     |
          irc_password |
          irc_port     | 
          irc_realname |
          irc_server   |
          irc_username |
          irc_vhost    |
          channel_entry |
          error;

irc_away: AWAY '=' STRING ';'
{
   MyFree(IRCItem->away);
   IRCItem->away = DupString($3);
};

irc_mode: MODE '=' STRING ';'
{
   MyFree(IRCItem->mode);
   IRCItem->mode = DupString($3);
};

irc_nick: NICK '=' STRING ';'
{
   MyFree(IRCItem->nick);
   IRCItem->nick = DupString($3);
};

irc_oper: OPER '=' STRING ';'
{
   MyFree(IRCItem->oper);
   IRCItem->oper = DupString($3);
};

irc_password: PASSWORD '=' STRING ';'
{
   MyFree(IRCItem->password);
   IRCItem->password = DupString($3);
};

irc_port: PORT '=' NUMBER ';'
{
   IRCItem->port = $3;
};

irc_realname: REALNAME '=' STRING ';'
{
   MyFree(IRCItem->realname);
   IRCItem->realname = DupString($3);
};

irc_server: SERVER '=' STRING ';'
{
   MyFree(IRCItem->server);
   IRCItem->server = DupString($3);
};

irc_username: USERNAME '=' STRING ';'
{
   MyFree(IRCItem->username);
   IRCItem->username = DupString($3);
};

irc_vhost: VHOST '=' STRING ';'
{
   MyFree(IRCItem->vhost);
   IRCItem->vhost = DupString($3);
};


/************************** CHANNEL BLOCK *************************/

channel_entry: 
{
   node_t *node;
   struct ChannelConf *item;

   item = MyMalloc(sizeof(struct UserConf));

   item->name = DupString("");
   item->key = DupString("");

   node = node_create(item);
   list_add(IRCItem->channels, node);

   tmp = (void *) item;
}
CHANNEL '{' channel_items '}' ';';

channel_items: channel_items channel_item |
               channel_item;

channel_item:  channel_name |
               channel_key;

channel_name: NAME '=' STRING ';'
{
   struct ChannelConf *item = tmp;

   MyFree(item->name);
   item->name = DupString($3);
};

channel_key: KEY '=' STRING ';'
{
   struct ChannelConf *item = tmp;

   MyFree(item->name);
   item->name = DupString($3);
};

/*************************** USER BLOCK ***************************/

user_entry: 
{
   node_t *node;
   struct UserConf *item;

   item = MyMalloc(sizeof(struct UserConf));

   item->masks = list_create();
   item->scanners = list_create();

   node = node_create(item);
   list_add(UserItemList, node);

   tmp = (void *) item; 
} 
USER '{' user_items  '}' ';' ;

user_items: user_items user_item |
           user_item;

user_item: user_mask     |
           user_scanner  |
          error;

user_mask: MASK '=' STRING ';'
{
   struct UserConf *item = (struct UserConf *) tmp;

   node_t *node;
   node = node_create((void *) DupString($3));

   list_add(item->masks, node);
};

user_scanner: SCANNER '=' STRING ';'
{
   struct UserConf *item = (struct UserConf *) tmp;

   node_t *node;
   node = node_create((void *) DupString($3));

   list_add(item->scanners, node);
};


%%


