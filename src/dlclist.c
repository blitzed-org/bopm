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
 * Functions for manipulating generic doubly-linked-circular lists.
 */

#include <stdio.h>
#include <stdlib.h>

#include "setup.h"
#include "dlclist.h"
#include "malloc.h"

/*
 * Create a new DLC list and return a pointer to it, or NULL on failure.
 */
struct dlclist *dlclist_init(void)
{
   struct dlclist *l;

   l = MyMalloc(sizeof(*l));

   if (!l)
      return(NULL);

   l->next = l;
   l->prev = l;
   l->val = NULL;

   return(l);
}

/*
 * Delete a specific node.  It's left to the caller to free() n->val first,
 * if necessary.
 */
void dlclist_delete_node(struct dlclist *n)
{
   n->prev->next = n->next;
   n->next->prev = n->prev;
   /* MyFree(n); */
}

/*
 * Delete an entire DLC list.  It's left to the caller to walk the list
 * and free() the val member first, if necessary.
 */
void dlclist_destroy(struct dlclist *head)
{
   while (head->next != head)
      dlclist_delete_node(head->next);

   /* MyFree(head); */
}

/*
 * Insert a new node before the given node and return pointer to it, or
 * NULL on failure.
 */
struct dlclist *dlclist_insert_before(struct dlclist *n, void *v)
{
   struct dlclist *t;

   t = MyMalloc(sizeof(*t));

   if (!t)
      return(NULL);

   t->val = v;
   t->next = n;
   t->prev = n->prev;
   t->next->prev = t;
   t->prev->next = t;

   return(t);
}

/*
 * Insert a new node after the given node and return a pointer to it, or
 * NULL on failure.  This function can be generalised from the above.
 */
struct dlclist *dlclist_insert_after(struct dlclist *n, void *v)
{
   return(dlclist_insert_before(n->next, v));
}
