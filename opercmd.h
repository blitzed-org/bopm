#ifndef OPERCMD_H
#define OPERCMD_H

	struct command {
		/* types defined below */
		/* THIS MUST BE CMD_NONE FOR AN EMPTY COMMAND! */
		unsigned int type;

		/* command parameter
		 * <erik> but i cant think of any commands bopm will ever
		 *   have that is multiple parameters
		 */
		char *param;
	
		/* who ordered it */	
		char nick[NICKMAX];

		/* where the reply is to be sent */
		char *target;

		/* when it was added, because we might need to remove
		 * it if it does not get executed */
		time_t added;
	};

	/* allow 5 outstanding commands */
	#define MAXCMD 5

	struct command cmd_stack[MAXCMD];

	/* one for each oper command we support */
	#define CMD_NONE  0
	#define CMD_CHECK 1

	time_t LAST_REAP_TIME;

	void do_oper_cmd(const char *nick, const char *cmd,
			 const char *param, const char *target);
	void checkoper(const char *nick, const char *cmd,
		       const char *param, const char *target,
		       unsigned int cmd_type);
	void check_userhost(const char *userhost);
	void delete_command(unsigned int index);
	void reap_commands(time_t present);
#endif
