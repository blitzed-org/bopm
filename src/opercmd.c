/*
Copyright (C) 2002  Erik Fears
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
 
      Foundation, Inc.
      59 Temple Place - Suite 330
      Boston, MA  02111-1307, USA.
 
*/

#include "setup.h"

#ifdef STDC_HEADERS
# include <string.h>
# include <stdlib.h>
#endif

#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>

#include "irc.h"
#include "log.h"
#include "misc.h"
#include "opercmd.h"
#include "scan.h"
#include "config.h"
#include "extern.h"
#include "malloc.h"

static void checkoper(const char *nick, const char *param,
                      const char *target, unsigned int cmd_type);
static void delete_command(unsigned int index);
static void cmd_check(char **, unsigned int, char *, const struct ChannelConf *, const struct UserInfo *);

struct OperCommandHash COMMANDS[] = 
{
   {"CHECK",  cmd_check}
};


void command_parse(char *command, char *msg, const struct ChannelConf *target, struct UserInfo *source_p)
{
   if(OPT_DEBUG)
      log("COMMAND -> Parsing command (%s) from %s [%s]", command, source_p->irc_nick, target->name);
}

static void cmd_check(char **commandv, unsigned int commandc, char *msg, 
                         const struct ChannelConf *channel, const struct UserInfo *source_p)
{

}
