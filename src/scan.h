#ifndef SCAN_H
#define SCAN_H
    
typedef struct protocol_hash protocol_hash;
typedef struct scan_struct scan_struct;
typedef int (*scan_function) (scan_struct *);

#define STATE_UNESTABLISHED 1
#define STATE_ESTABLISHED   2
#define STATE_SENT          3
#define STATE_CLOSED        4   
 
struct protocol_hash
{
	char *type;                 /* Plaintext name of protocol to scan   */
	int port;                   /* Port to scan protocol on             */
	scan_function w_handler;    /* Function to handle specific protocol */
	unsigned int stat_num;
	unsigned int stat_numopen;
};

struct scan_struct
{
	scan_struct *next;   
	char *addr;                  /* Address of remote host (IP)                      */
	char *irc_addr;              /* Hostname of user on IRC (for kline)              */ 
	char *irc_nick;              /* Nickname of user on IRC (for logging)            */
	char *irc_user;              /* Username of user on IRC (for logging)            */
	char *conn_notice;           /* original server notice for this connect, used
				      * for evidence                                     */
 
	char *data;                  /* Buffered data                                    */
	int  datasize;               /* Length of buffered data                          */

	int fd;                      /* File descriptor of socket                        */
	struct sockaddr_in sockaddr; /* holds information about remote host for socket() */
	time_t create_time;          /* Creation time, for timeout                       */         
	int state;                   /* Status of scan                                   */
	unsigned int bytes_read;     /* Number of bytes received                         */
	protocol_hash *protocol;     /* Pointer to protocol type                         */
	int verbose;                 /* report progress to channel verbosely?            */
};

extern void do_scan_init(void);
extern void scan_connect(char *addr, char *irc_addr, char *irc_nick,
    char *irc_user, int verbose, char *conn_notice);
extern void scan_cycle(void);
extern void scan_timer(void);    
extern void do_manual_check(struct command *c);

#endif 
