/*
Copyright (C) 2002 Andy Smith
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to:
 
      the Free Software Foundation, Inc.
      59 Temple Place - Suite 330
      Boston, MA  02111-1307, USA.
 
*/

/*
 * Send a warning to each user as they connect, about port scanning for open
 * proxies that we are doing.  We maintain a doubly-linked circular (DLC) list
 * and use it as a queue: we add our notice text (which is stored in the
 * CONF_SCAN_WARNING list) to the end and periodically remove nodes from the
 * start, sending them as notices as we do.
 *
 * Code for maintaining the DLC list can be found in dlclist.[ch].
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "setup.h"
#include "irc.h"
#include "config.h"
#include "options.h"
#include "scanwarn.h"
#include "malloc.h"

extern int OPT_DEBUG;

struct dlclist *warnq = NULL;

/*
 * Set up the structures we need.
 */
void do_scanwarn_init(void)
{
}

/*
 * Add a set of notices to the warn queue for a given nick.
 */
void add_warning(const char *nick)
{
}


/*
 * Do one warning loop.  Each time we're given chance to run we will take at
 * most NOTICES_PER_LOOP off the warnq and send them.
 */
void scanwarn_timer(void)
{
}
