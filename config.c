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


/* Global Configuration Variables */

char *CONF_SERVER = 0;
char *CONF_USER   = 0;
char *CONF_NICK   = 0;
int  CONF_PORT    = 0;


/* Configuration Hash , Hashes Config Params to their Function Handlers*/

config_hash hash[] = {

       {"SERVER",   &(param_server)   },
       {"PORT",     &(param_port)     },
       {"USER",     &(param_user)     },
       {"NICK",     &(param_nick)     }
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
                 (int*) hash[i].function(args);                 
              
      }
    
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
	 
	return 1;
}

int param_nick(char *args)
{
	if(CONF_NICK)
	    free(CONF_NICK);

	CONF_NICK = strdup(args);

	return 1;
}
