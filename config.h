#ifndef CONFIG_H
#define CONFIG_H

      typedef struct config_hash config_hash;

      #define TYPE_STRING 1
      #define TYPE_INT 2

      struct config_hash
       {
           char *key;
           int type;
           void *var;
       };

       /* Config Functions */

       void config_load(char *filename);

#endif
