#ifndef IRC_H
#define IRC_H

#include "inet.h"

#define NICKMAX 32
#define MSGLENMAX 513


typedef void (*irc_command) (void);

struct bopm_sockaddr
{
    union {
        struct sockaddr_in sa4;
#ifdef IPV6

        struct sockaddr_in6 sa6;
#endif

    } sas;
};


struct bopm_ircaddr
{
    union {
        struct in_addr in4;
#ifdef IPV6

        struct in6_addr in6;
#endif

    } ins;
};


struct UserInfo 
{
   char *irc_nick;
   char *irc_username;
   char *irc_hostname;
 
   char *ip;
};

struct CommandHash
{
   char       *command;
   irc_command handler;
};

extern int remote_is_ipv6;
extern int bindto_ipv6;

extern void irc_send(char *data, ...);
extern void irc_kline(char *addr, char *ip);
extern void irc_cycle(void);
extern void irc_timer(void);


#define copy_s_addr(a, b)  \
do { \
((uint32_t *)a)[0] = ((uint32_t *)b)[0]; \
((uint32_t *)a)[1] = ((uint32_t *)b)[1]; \
((uint32_t *)a)[2] = ((uint32_t *)b)[2]; \
((uint32_t *)a)[3] = ((uint32_t *)b)[3]; \
} while(0)

#endif
