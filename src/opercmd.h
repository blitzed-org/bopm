#ifndef OPERCMD_H
#define OPERCMD_H

struct command
{
   /* Types defined below. */
   /* THIS MUST BE CMD_NONE FOR AN EMPTY COMMAND! */
   unsigned int type;

   /* Command parameter.
    * <erik> but i cant think of any commands bopm will ever have that is
    *   multiple parameters
    */
   char *param;

   /* Who ordered it. */
   char nick[NICKMAX];

   /* Where the reply is to be sent. */
   char *target;

   /*
    * When it was added, because we might need to remove it if it does
    * not get executed.
    */
   time_t added;
};

/* Allow 5 outstanding commands. */
#define MAXCMD 5

/* One for each oper command we support. */
#define CMD_NONE  0
#define CMD_CHECK 1

extern void do_oper_cmd(const char *nick, const char *cmd,
                           const char *param, const char *target);
extern void check_userhost(const char *userhost);
extern void reap_commands(time_t present);

#endif
