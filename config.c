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
#include <time.h>

#include "config.h"
#include "log.h"
#include "misc.h"

/* Global Configuration Variables */

char *CONF_SERVER          = 0;
char *CONF_PASSWORD        = 0;
char *CONF_USER            = 0;
char *CONF_NICK            = 0;
char *CONF_OPER            = 0;
char *CONF_OPER_MODES      = 0;
char *CONF_SCANIP          = 0;
char *CONF_BINDIRC         = 0;
char *CONF_BINDSCAN        = 0;
char *CONF_CHANNELS        = 0;
char *CONF_NICKSERV_IDENT  = 0;
char *CONF_CHANSERV_INVITE = 0;
char *CONF_KLINE_COMMAND   = 0;
char *CONF_DNSBL_ZONE      = 0;
char *CONF_DNSBL_FROM      = 0;
char *CONF_DNSBL_TO        = 0;
char *CONF_SENDMAIL        = 0;
char *CONF_HELP_EMAIL      = 0;
char *CONF_AWAY            = 0;

int  CONF_SCANPORT         = 0;
int  CONF_PORT             = 0;


/* Configuration Hash , Hashes Config Params to their Function Handlers*/
/*      NAME                  , TYPE   , REQ, REQMET, PTR TO VAR        */
config_hash hash[] = {
       {"SERVER",              TYPE_STRING, 1,0,    &CONF_SERVER             },
       {"PORT",                TYPE_INT   , 1,0,    &CONF_PORT               },
       {"PASSWORD",            TYPE_STRING, 0,0,    &CONF_PASSWORD           },
       {"USER",                TYPE_STRING, 1,0,    &CONF_USER               },
       {"NICK",                TYPE_STRING, 1,0,    &CONF_NICK               },
       {"OPER",                TYPE_STRING, 1,0,    &CONF_OPER               },
       {"OPER_MODES",          TYPE_STRING, 1,0,    &CONF_OPER_MODES         },
       {"SCANIP",              TYPE_STRING, 1,0,    &CONF_SCANIP             },
       {"SCANPORT",            TYPE_INT   , 1,0,    &CONF_SCANPORT           },
       {"BINDIRC",             TYPE_STRING, 0,0,    &CONF_BINDIRC            },
       {"BINDSCAN",            TYPE_STRING, 0,0,    &CONF_BINDSCAN           },
       {"CHANNELS",            TYPE_STRING, 1,0,    &CONF_CHANNELS           },
       {"NICKSERV_IDENT",      TYPE_STRING, 0,0,    &CONF_NICKSERV_IDENT     },
       {"CHANSERV_INVITE",     TYPE_STRING, 0,0,    &CONF_CHANSERV_INVITE    },
       {"KLINE_COMMAND",       TYPE_STRING, 1,0,    &CONF_KLINE_COMMAND      },
       {"DNSBL_ZONE",          TYPE_STRING, 0,0,    &CONF_DNSBL_ZONE         },
       {"DNSBL_FROM",          TYPE_STRING, 0,0,    &CONF_DNSBL_FROM         },
       {"DNSBL_TO",            TYPE_STRING, 0,0,    &CONF_DNSBL_TO           },
       {"SENDMAIL",            TYPE_STRING, 0,0,    &CONF_SENDMAIL           },
       {"HELP_EMAIL",          TYPE_STRING, 1,0,    &CONF_HELP_EMAIL         },
       {"AWAY",                TYPE_STRING, 1,0,    &CONF_AWAY               },
       {0,                     0,           0,0,    0                        },
};



/* Parse File */

void config_load(char *filename)
{
    FILE *in;

    char line[1024];  /* 1k buffer for reading the file */

    char *key;
    char *args;   

    int i;

    if(!(in = fopen(filename, "r")))
     {
	log("CONFIG -> No config file found, aborting.");
	exit(1);
     }
    
    /* Clear anything we have already */
    for(i = 0; i < (sizeof(hash) / sizeof(config_hash)); i++)
      {
         switch(hash[i].type)
          { 
              case TYPE_STRING:
                  if(( *(char**) hash[i].var))
                       free(*(char**)hash[i].var);
                  *(char**)hash[i].var = 0;
                  break;
              case TYPE_INT:
                  *(int *) hash[i].var = 0;
          }
         hash[i].reqmet = 0;
      }

    while(fgets(line,1023, in))  
      {

	    if(line[0] == '#')
		 continue;
		    
            key = strtok(line, " ");
            args = strtok(NULL, "\n");

            if(!args)
               continue;


            args = clean(args); /* Strip leading and tailing spaces */

            for(i = 0; i < (sizeof(hash) / sizeof(config_hash)); i++)
              if(!strcasecmp(key, hash[i].key))
                {
                      switch(hash[i].type)
                        {
                            case TYPE_STRING: 
                                 *(char**) hash[i].var = strdup(args);
                                 break;
                            case TYPE_INT:
                                 *(int *) hash[i].var = atoi(args);
                                 break;
                        }
                       hash[i].reqmet = 1;
                }

      }


  fclose(in);
  config_checkreq(); /* Check required parameters */
}

void config_checkreq()
{
      int i;
      int errfnd = 0;

      for(i = 0; i < (sizeof(hash) / sizeof(config_hash)); i++)
        if(hash[i].req && !hash[i].reqmet)
         {               
            log("CONFIG -> Parameter [%s] required but not defined in config.", hash[i].key);
            errfnd++;
         }

      if(errfnd)
       {
          log("CONFIG -> %d parameters missing from config file, aborting.", errfnd);
          exit(1);
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


