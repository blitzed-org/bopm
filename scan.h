#ifndef SCAN_H
#define SCAN_H
    
    typedef struct protocol_hash protocol_hash;
    typedef struct scan_struct scan_struct;
    typedef int (*scan_function) (scan_struct *);

    #define STATE_ESTABLISHED 0
    #define STATE_SENT        1
    #define STATE_CLOSED      3   
 
    struct protocol_hash
     {
          char *type;               /* Plaintext name of protocol to scan   */
          int port;                 /* Port to scan protocol on             */
          scan_function w_handler;    /* Function to handle specific protocol */
          scan_function r_handler;
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
          int fd;                      /* File descriptor of socket                        */
          struct sockaddr_in sockaddr; /* holds information about remote host for socket() */
          time_t create_time;          /* Creation time, for timeout                       */         
          int state;                   /* Status of scan                                   */
          protocol_hash *protocol;     /* Pointer to protocol type                         */
	  int verbose;                 /* report progress to channel verbosely?            */
     };


     void scan_connect(char *addr, char *irc_addr, char *irc_nick, char *irc_user, int verbose);
     void scan_add(scan_struct *newcon);
     void scan_del(scan_struct *ss);
     void scan_cycle();
     void scan_check();
     void scan_timer();    
    
     int scan_w_squid(scan_struct *ss);
     int scan_r_squid(scan_struct *ss);
     
     int scan_w_socks4(scan_struct *ss);
     int scan_r_socks4(scan_struct *ss);

     int scan_w_socks5(scan_struct *ss);
     int scan_r_socks5(scan_struct *ss);

     int scan_w_wingate(scan_struct *ss);
     int scan_r_wingate(scan_struct *ss);

     void do_manual_check(struct command *c);
#endif 
