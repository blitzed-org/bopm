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
#include "extern.h"

void do_oper_cmd(const char *nick, const char *cmd, const char *param,
		 const char *target)
{
   if(strncasecmp(cmd, "CHECK", 5) == 0)
    {
      if(!param)
       {
         irc_send("PRIVMSG %s :*ring* Hello, cluephone for %s, you need to "
		  "specify a host or IP address if you want me to do any "
		  "checking, yo.", target, nick);
	 return;
       }
      checkoper(nick, cmd, param, target, CMD_CHECK);
    }
   else
    {
      irc_send("PRIVMSG %s :Sorry, I don't know how to %s, %s.", target,
	       cmd, nick);
    }
   return;
}

void checkoper(const char *nick, const char *cmd, const char *param,
	       const char *target, unsigned int cmd_type)
{
   int i;

   for(i = 0; i < MAXCMD; i++)
    {
      if(cmd_stack[i].type == CMD_NONE)
       {
         cmd_stack[i].type = cmd_type;
         cmd_stack[i].param = strdup(param);
         cmd_stack[i].target = strdup(target);
         strncpy(cmd_stack[i].nick, nick, NICKMAX);
	 break;
       }
    }

   if(i == MAXCMD)
      irc_send("PRIVMSG %s :Too many queued commands, try later.", target);
   else
      irc_send("USERHOST %s", nick);
}

void check_userhost(const char *userhost)
{
   /* check a userhost to see if it is from an oper, we're looking for
    * the asterisk (*) between the nick and the ident as below:
    * :grifferz*=+goats@pc-62-30-219-54-pb.blueyonder.co.uk */
   char *tmp;
   int c, oper = 0;
   
   tmp = strchr(userhost, '=');

   if(!tmp)
    {
      /* looks like they quit, oh well, just ignore it */
      return;
    }

   if(*(tmp - 1) == '*')
    {
      oper = 1;
      tmp--;
    }

   /* null terminate userhost so we have a nickname there now */
   *tmp = '\0';

   /* go through the command list looking for commands by this person */
   for(c = 0; c < MAXCMD; c++)
    {
      if(!strcasecmp(userhost + 1, cmd_stack[c].nick))
       {
         if(oper)
	  {
            /* do the command */
	    if(cmd_stack[c].type == CMD_CHECK)
	     {
	       do_manual_check(&cmd_stack[c]);
	     }
	  }
	 else
	  {
	    irc_send("PRIVMSG %s :You are not an IRC Operator.  Go away.",
		     cmd_stack[c].target);
	  }

	 delete_command(c);
       }
    }
}

/* set a command to CMD_NONE and free all resources it used, so that it
 * can be reused later */
void delete_command(unsigned int index)
{
   if(cmd_stack[index].type == CMD_NONE)
      return;
   
   cmd_stack[index].type = CMD_NONE;
   cmd_stack[index].added = 0;
   free(cmd_stack[index].param);
   free(cmd_stack[index].target);
}

/* delete any commands which are more than 2 minutes old, they are almost certainly
 * no longer relevant. */
void reap_commands(time_t present)
{
   int c;

   for(c = 0; c < MAXCMD; c++)
    {
      if(cmd_stack[c].type != CMD_NONE &&
         (present - cmd_stack[c].added >= 120))
       {
         irc_send("PRIVMSG %s :Reaping dead command from %s of type %u "
		  "with param '%s', added %s ago.", cmd_stack[c].nick,
		  cmd_stack[c].type, cmd_stack[c].param,
		  dissect_time(present - cmd_stack[c].added));
	 delete_command(c);
       }
    }
}
