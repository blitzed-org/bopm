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
char *CONF_SCANIP          = 0;
char *CONF_BINDIRC         = 0;
char *CONF_BINDSCAN        = 0;
char *CONF_CHANNELS        = 0;
char *CONF_NICKSERV_IDENT  = 0;
char *CONF_CHANSERV_INVITE = 0;
char *CONF_KLINE_COMMAND   = 0;

int  CONF_SCANPORT         = 0;
int  CONF_PORT             = 0;


/* Configuration Hash , Hashes Config Params to their Function Handlers*/

config_hash hash[] = {
       {"SERVER",              TYPE_STRING,     &CONF_SERVER             },
       {"PORT",                TYPE_INT   ,     &CONF_PORT               },
       {"USER",                TYPE_STRING,     &CONF_USER               },
       {"NICK",                TYPE_STRING,     &CONF_NICK               },
       {"OPER",                TYPE_STRING,     &CONF_OPER               },
       {"SCANIP",              TYPE_STRING,     &CONF_SCANIP             },
       {"SCANPORT",            TYPE_INT   ,     &CONF_SCANPORT           },
       {"BINDIRC",             TYPE_STRING,     &CONF_BINDIRC            },
       {"BINDSCAN",            TYPE_STRING,     &CONF_BINDSCAN           },
       {"CHANNELS",            TYPE_STRING,     &CONF_CHANNELS           },
       {"NICKSERV_IDENT",      TYPE_STRING,     &CONF_NICKSERV_IDENT     },
       {"CHANSERV_INVITE",     TYPE_STRING,     &CONF_CHANSERV_INVITE    },
       {"KLINE_COMMAND",       TYPE_STRING,     &CONF_KLINE_COMMAND      },
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
                      switch(hash[i].type)
                        {
                            case TYPE_STRING:
                                 if(( *(char**) hash[i].var))
                                   {
                                    free(*(char**)hash[i].var);
                                    printf("FREE!\n");
                                   }
                                 *(char**) hash[i].var = strdup(args);
                                 break;
                            case TYPE_INT:
                                 *(int *) hash[i].var = atoi(args);
                                 break;
                        }
 
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


