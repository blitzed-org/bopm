#ifndef IRC_H
#define IRC_H

#define NICKMAX 32
#define MSGLENMAX 513
      
      void irc_cycle();
      void irc_init();
      void irc_connect();
      void irc_reconnect();
      void irc_read();
      void irc_parse();
      void irc_kline(char *addr, char *ip);
      void irc_timer();
      void irc_send(char *data, ...);       

      void do_invite();
      void do_privmsg();
      void do_perform();
      void do_connect(char *addr, char *irc_nick, char *irc_user,
		      char *irc_addr, char *conn_notice);
#endif
