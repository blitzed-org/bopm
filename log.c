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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <time.h>

#include "log.h"


FILE *logfile;


void log_open(char *filename)
{
    logfile = fopen(filename, "a");
    
    if(!logfile)
      {
           perror("Cannot open log file. Aborting.");
           exit(1);
      }
    
}

void log_close()
{
     fclose(logfile);
}

void log(char *data, ...)
{
        time_t present;
 
        va_list arglist;
        char data2[513];
        char tolog[513];	


        if(!logfile)
            return;

        time(&present);
 
        va_start(arglist, data);
        vsprintf(data2, data, arglist);
        va_end(arglist);

        snprintf(tolog, 512, "[%d] %s\n", (int) present, data2);
        fwrite(tolog,1,strlen(tolog),logfile);

        fflush(logfile);
}
