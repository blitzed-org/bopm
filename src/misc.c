/* vim: set shiftwidth=3 softtabstop=3 expandtab: */

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

#include "setup.h"

#include <stdio.h>
#include <time.h>

#ifdef STDC_HEADERS
# include <string.h>
#endif

#include "compat.h"
#include "config.h"
#include "extern.h"
#include "misc.h"


/*
 * Split a time_t into an English-language explanation of how
 * much time it represents, e.g. "2 hours 45 minutes 8 seconds"
 */
char *dissect_time(time_t time)
{
   static char buf[64];
   unsigned int years, weeks, days, hours, minutes, seconds;

   years = weeks = days = hours = minutes = seconds = 0;

   while (time >= 60 * 60 * 24 * 365)
   {
      time -= 60 * 60 * 24 * 365;
      years++;
   }

   while (time >= 60 * 60 * 24 * 7)
   {
      time -= 60 * 60 * 24 * 7;
      weeks++;
   }

   while (time >= 60 * 60 * 24)
   {
      time -= 60 * 60 * 24;
      days++;
   }

   while (time >= 60 * 60)
   {
      time -= 60 * 60;
      hours++;
   }

   while (time >= 60)
   {
      time -= 60;
      minutes++;
   }

   seconds = time;

   if (years)
   {
      snprintf(buf, sizeof(buf),
               "%d year%s, %d week%s, %d day%s, %02d:%02d:%02d",
               years, years == 1 ? "" : "s", weeks,
               weeks == 1 ? "" : "s", days, days == 1 ? "" : "s",
               hours, minutes, seconds);
   }
   else if (weeks)
   {
      snprintf(buf, sizeof(buf),
               "%d week%s, %d day%s, %02d:%02d:%02d", weeks,
               weeks == 1 ? "" : "s", days, days == 1 ? "" : "s",
               hours, minutes, seconds);
   }
   else if (days)
   {
      snprintf(buf, sizeof(buf), "%d day%s, %02d:%02d:%02d",
               days, days == 1 ? "" : "s", hours, minutes, seconds);
   }
   else if (hours)
   {
      if (minutes || seconds)
      {
         snprintf(buf, sizeof(buf),
                  "%d hour%s, %d minute%s, %d second%s", hours,
                  hours == 1 ? "" : "s", minutes,
                  minutes == 1 ? "" : "s", seconds,
                  seconds == 1 ? "" : "s");
      }
      else
      {
         snprintf(buf, sizeof(buf), "%d hour%s", hours,
                  hours == 1 ? "" : "s");
      }
   }
   else if (minutes)
   {
      snprintf(buf, sizeof(buf), "%d minute%s, %d second%s",
               minutes, minutes == 1 ? "" : "s", seconds,
               seconds == 1 ? "" : "s");
   }
   else
   {
      snprintf(buf, sizeof(buf), "%d second%s", seconds,
               seconds == 1 ? "" : "s");
   }

   return(buf);
}

/*
 * Strip leading/tailing characters from null terminated str and return a
 * pointer to the new string.
 */

char *clean(char *str)
{
   size_t i;
   /* Position of last non space. */
   int ln;
   /* Position of first non space. */
   int fn;
   int gf;

   ln = 0;
   fn = 0;
   gf = 0;

   /* Dont need to deal with 1 character */
   if (strlen(str) <= 1)
      return str;

   for (i = 0; i < strlen(str); i++)
   {
      if (fn == 0 && str[i] != ' ' && !gf)
      {
         fn = i;
         gf = 1;
      }
      if (str[i] != ' ')
         ln = i;
   }

   /* Null terminate before the tailing spaces. */
   str[ln + 1] = 0;

   /* Return pointer to point after leading spaces. */
   return(str + (fn));
}
