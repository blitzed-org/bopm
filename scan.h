#ifndef SCAN_H
#define SCAN_H

    typedef int (*scan_function) (int);
    typedef struct protocol_hash protocol_hash;
    typedef struct scan_struct scan_struct;
    
 
    struct protocol_hash
     {
          char *type;               /* Plaintext name of protocol to scan   */
          int port;                 /* Port to scan protocol on             */
          scan_function handler;    /* Function to handle specific protocol */
     };

    struct scan_struct
     {
          scan_struct *next;   
          char *addr;                  /* Address of remote host (IP) */ 
          int fd;                      /* File descriptor of socket   */
          struct sockaddr_in sockaddr; /* holds information about remote host for socket() */
          time_t create_time;          /* Creation time, for timeout  */         
          protocol_hash *protocol;     /* Pointer to protocol type    */
     };


     void scan_connect(char *ip);
     void scan_add(scan_struct *newconn);


     int scan_squid(int fd);

#endif 
