#ifndef CONFIG_H
#define CONFIG_H

      typedef int (*config_function) (char *);
      typedef struct config_hash config_hash;

      struct config_hash
       {
           char *key;
           config_function function;
       };





       /* Config Functions */

       void config_load(char *filename);

       int param_server(char *args);
       int param_port(char *args);
       int param_user(char *args);
       int param_nick(char *args);

#endif