/*
Copyright (C) 2002  Erik Fears

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software

      Foundation, Inc.
      59 Temple Place - Suite 330
      Boston, MA  02111-1307, USA.

*/



#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "log.h"

/* Global Configuration Variables */

char *CONF_SERVER          = 0;
char *CONF_USER            = 0;
char *CONF_NICK            = 0;
char *CONF_OPER            = 0;
char *CONF_REASON          = 0;
int  CONF_PORT             = 0;

perform_struct *CONF_PERFORM = 0;

/* Configuration Hash , Hashes Config Params to their Function Handlers*/

config_hash hash[] = {
       {"SERVER",   &(param_server)   },
       {"PORT",     &(param_port)     },
       {"USER",     &(param_user)     },
       {"NICK",     &(param_nick)     },
       {"PERFORM",  &(param_perform)  },
       {"OPER",     &(param_oper)     },
       {"REASON",   &(param_reason)   }
};



/* Parse File */

void config_load(char *filename)
{
    FILE *in;

    char line[1024];  //1k buffer for reading the file

    char *key;
    char *args;   

    int i;

    if(!(in = fopen(filename, "r")))
	return;
    
    while(fgets(line,1023, in))  
      {

	    if(line[0] == '#')
		 continue;
		    
            key = strtok(line, " ");
            args = strtok(NULL, "\n");

            if(!args)
               continue;

            for(i = 0; i < (sizeof(hash) / sizeof(config_hash)); i++)
              if(!strcasecmp(key, hash[i].key))
                {
                  if(!(int*) hash[i].function(args))
                     log("CONFIG -> Bad line: %s",line);               
                }
   
      }
    
}

/*  Called when memory allocation somewhere returns
 *  an error
 */

void config_memfail()
{
     log("CONFIG -> Error allocating memory.");
     exit(1);
}


/* Parameter Handlers */

/* I wrote these all seperately (some duplicated)
 * because there are not that many configuration items
 * to begin with and it will make for easier control
 * over the individual items.
 */

int param_server(char *args)
{
	if(CONF_SERVER)
	   free(CONF_SERVER);
	
        CONF_SERVER = strdup(args);

        if(!CONF_SERVER)
            config_memfail();
            	  	
        return 1;
}

int param_port(char *args)
{
	CONF_PORT = atoi(args);

        if(CONF_PORT < 1024 || CONF_PORT > 65535)
	       CONF_PORT = 6667;
	return 1;
}

int param_user(char *args)
{	
	if(CONF_USER)
	    free(CONF_USER);

	CONF_USER = strdup(args);

        if(!CONF_USER)
	     config_memfail();
	
	return 1;
}

int param_nick(char *args)
{
	if(CONF_NICK)
	    free(CONF_NICK);

	CONF_NICK = strdup(args);

        if(!CONF_NICK)
           config_memfail();
	
	return 1;
}

int param_oper(char *args)
{

         if(CONF_OPER)
             free(CONF_OPER);
     
         CONF_OPER = strdup(args);

         if(!CONF_OPER)
            config_memfail();

         return 1;
}

int param_reason(char *args)
{
          if(CONF_REASON)
              free(CONF_REASON);
     
          CONF_REASON = strdup(args);

          if(!CONF_REASON)
             config_memfail();

          return 1;
}


int param_perform(char *args)
{

        perform_struct *newpf;
        perform_struct *pf;
	
        if(strlen(args) == 0)
            return 0;
    
	newpf = malloc(sizeof(perform_struct));
	
	if(!newpf)
	     config_memfail();

        if(!CONF_PERFORM)            //First perform 
	 {              
	      newpf->perform = strdup(args);
	      newpf->next = 0;
              CONF_PERFORM = newpf;
	      return 1;
         }
	
	for(pf = CONF_PERFORM; pf; pf = pf->next)
	 {
             if(!pf->next)
	       {
                   newpf->perform = strdup(args);
		   pf->next = newpf;
		   newpf->next = 0;
		   break;
	       }
         }

        return 1;
}



