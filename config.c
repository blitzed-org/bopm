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
char *CONF_SCANIP          = 0;
char *CONF_BINDIRC         = 0;
char *CONF_BINDSCAN        = 0;
char *CONF_CHANNELS        = 0;

int  CONF_SCANPORT         = 0;
int  CONF_PORT             = 0;


/* Configuration Hash , Hashes Config Params to their Function Handlers*/

config_hash hash[] = {
       {"SERVER",   &(param_server)   },
       {"PORT",     &(param_port)     },
       {"USER",     &(param_user)     },
       {"NICK",     &(param_nick)     },
       {"OPER",     &(param_oper)     },
       {"REASON",   &(param_reason)   },
       {"SCANIP",   &(param_scanip)   },
       {"SCANPORT", &(param_scanport) },
       {"BINDIRC",  &(param_bindirc)  },
       {"BINDSCAN", &(param_bindscan) },
       {"CHANNELS", &(param_channels) }
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


int param_channels(char *args)
{
        if(CONF_CHANNELS)
           free(CONF_CHANNELS);
   
        CONF_CHANNELS = strdup(args);

        if(!CONF_CHANNELS)
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

int param_scanport(char *args)
{
        CONF_SCANPORT = atoi(args);

        if(CONF_SCANPORT < 1024 || CONF_SCANPORT > 65535)
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

int param_bindirc(char *args)
{
        if(CONF_BINDIRC)
            free(CONF_BINDIRC);

        CONF_USER = strdup(args);

        if(!CONF_BINDIRC)
             config_memfail();

        return 1;
}


int param_bindscan(char *args)
{
        if(CONF_BINDSCAN)
            free(CONF_BINDSCAN);

        CONF_BINDSCAN = strdup(args);

        if(!CONF_BINDSCAN)
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

int param_scanip(char *args)
{
          if(CONF_SCANIP)
              free(CONF_SCANIP);

          CONF_SCANIP = strdup(args);

          if(!CONF_SCANIP)
             config_memfail();

          return 1;
}
