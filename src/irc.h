#ifndef IRC_H
#define IRC_H

#define NICKMAX 32
#define MSGLENMAX 513

extern void irc_send(char *data, ...);
extern void irc_kline(char *addr, char *ip);
extern void irc_cycle(void);
extern void irc_timer(void);

#endif
