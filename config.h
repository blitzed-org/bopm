#ifndef CONFIG_H
#define CONFIG_H

      typedef int (*config_function) (char *);
      typedef struct config_hash config_hash;
      typedef struct perform_hash perform_hash;

      struct config_hash
       {
           char *key;
           config_function function;
       };


       struct perform_hash
       {
           perform_hash *next;
           char *perform;
       };


       /* Config Functions */

       void config_load(char *filename);

       int param_server(char *args);
       int param_port(char *args);
       int param_user(char *args);
       int param_nick(char *args);
       int param_perform(char *args);

#endif
