#ifndef CONFIG_H
#define CONFIG_H

      typedef struct config_hash config_hash;

#define TYPE_STRING 1
#define TYPE_INT 2
#define TYPE_LIST 3

      struct config_hash
       {
           char *key;
           int type;
           int req;     /* Item is required */
           int reqmet;  /* Req met          */
           void *var;
       };

      typedef struct string_list string_list;
      
      struct string_list
       {
         string_list *next;
	 char *text;
       };
       	
       /* Config Functions */

       void config_load(char *filename);
       void config_checkreq();
       void config_memfail();

#endif
