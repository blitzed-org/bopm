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
          unsigned int bytes_read;          /* Number of bytes received                         */
          protocol_hash *protocol;     /* Pointer to protocol type                         */
	  int verbose;                 /* report progress to channel verbosely?            */
     };

     void do_scan_init();
     void scan_connect(char *addr, char *irc_addr, char *irc_nick,
		       char *irc_user, int verbose, char *conn_notice);
     void scan_establish(scan_struct *conn);
     void scan_add(scan_struct *newcon);
     void scan_del(scan_struct *ss);
     void scan_cycle();
     void scan_check();
     void scan_read(scan_struct *conn);
     void scan_timer();    
     void scan_readready(scan_struct *conn);
     void scan_writeready(scan_struct *conn);
     void scan_negfail(scan_struct *conn);
     void scan_openproxy(scan_struct *conn);
     
     int scan_w_squid(scan_struct *conn);     
     int scan_w_socks4(scan_struct *conn);
     int scan_w_socks5(scan_struct *conn);
     int scan_w_wingate(scan_struct *conn);

     void do_manual_check(struct command *c);
#endif 
