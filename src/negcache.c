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
 * A Negative caching implementation for IPv4 addresses.  The idea is that
 * every time an IP address is seen, it is checked against a patricia trie.  If
 * the IP address was previously seen and within an acceptable period of time,
 * it is not scanned again.  Otherwise, the address is scanned as normal.  If
 * it is proven to be OK (i.e.  it doesn't run an open proxy) then it is added
 * to the trie.
 *
 * I'm using a patricia trie as described on p253 of Sedgewick.  The reason for
 * this is that it can find any IP address with only log_2 N bit comparisons
 * and only requires as many nodes as we have IP addresses to store.
 *
 * I would have liked to have made the trie implementation generic but couldn't
 * see how to do it easily given that the trie structure depends on the data
 * within it.
 *  --grifferz
 */

#include "setup.h"

#include <stdio.h>

#ifdef STDC_HEADERS
#include <stdlib.h>
#endif

#ifdef TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include "inet.h"
#include "irc.h"
#include "negcache.h"
#include "config.h"
#include "malloc.h"

extern unsigned int OPT_DEBUG;

static struct cnode *nc_search(struct cnode *head, const unsigned long ip);
static struct cnode *nc_insert(struct cnode *head, const unsigned long ip);
static void nc_rebuild(struct cnode *old_head, struct cnode *new_head,
                       struct cnode *n, time_t now);

time_t last_nc_expire;
unsigned int maxb;
struct cnode *nc_head = NULL;

/*
 * Return the bit which appears k bits from the right in x.
 */
#define GETBIT(x,k)	((x >> k) & 1)


/*
 * Initialise the patricia trie we use for storing our negative cache.
 */
void nc_init(struct cnode **head)
{
   if (*head)
   {
      /* Cache already exists */
      return;
   }

   *head = MyMalloc(sizeof(**head));

   maxb = (sizeof((*head)->ip) * 8);
   (*head)->ip = 0;
   (*head)->b = maxb;
   (*head)->l = (*head)->r = *head;
   last_nc_expire = time(NULL);
}

/*
 * Find and return a pointer to the trie node that corresponds to a given IP
 * address as expressed in network form, or return NULL if the IP isn't
 * present.
 */
static struct cnode *nc_search(struct cnode *head, const unsigned long ip)
{
   struct cnode *p, *x;

   p = head;
   x = head->l;

   while (p->b > x->b)
   {
      p = x;
      x = GETBIT(ip, x->b) ? x->r : x->l;
   }

   if (ip == x->ip)
      return(x);
   else
      return(NULL);
}

/*
 * Insert a new node into the trie, and return a pointer to it.
 */
static struct cnode *nc_insert(struct cnode *head, const unsigned long ip)
{
   int i;
   struct cnode *p, *t, *x;

   i = maxb;
   p = head;
   t = head->l;

   while (p->b > t->b)
   {
      p = t;
      t = GETBIT(ip, t->b) ? t->r : t->l;
   }

   if (ip == t->ip)
   {
      /* Node already exists. */
      return(t);
   }

   while (GETBIT(t->ip, i) == GETBIT(ip, i))
      i--;

   p = head;
   x = head->l;

   while (p->b > x->b && x->b > i)
   {
      p = x;
      x = GETBIT(ip, x->b) ? x->r : x->l;
   }

   t = MyMalloc(sizeof(*t));
   t->ip = ip;
   t->b = i;
   t->l = GETBIT(ip, t->b) ? x : t;
   t->r = GETBIT(ip, t->b) ? t : x;

   if (GETBIT(ip, p->b))
      p->r = t;
   else
      p->l = t;

   return(t);
}

/*
 * Check whether an IP address is in our negative cache and was added
 * recently enough.  Return a pointer to its node if so, NULL otherwise.
 */
struct cnode *check_neg_cache(const unsigned long ip)
{
   time_t now;
   struct cnode *n;

   if (!OptionsItem->negcache == 0)
      return(NULL);

   n = nc_search(nc_head, ip);

   if (n)
   {
      /* Check it is recent enough. */
      now = time(NULL);

      if (now - n->seen <= OptionsItem->negcache)
         return(n);
   }

   return(NULL);
}

/*
 * Prepare an ASCII string representing an IPv4 address for inserting into
 * our negative cache.
 */
void negcache_insert(const char *ipstr)
{
   struct bopm_sockaddr ip;
   struct cnode *n;

   if (!inet_pton(AF_INET, ipstr, &(ip.sa4.sin_addr)))
   {
      log("NEGCACHE -> Invalid IPv4 address '%s'", ipstr);
      return;
   }

   n = nc_insert(nc_head, ip.sa4.sin_addr.s_addr);

   if (n)
      n->seen = time(NULL);
}

/*
 * Recursively walk the cache and insert the nodes into a new patricia trie,
 * skipping nodes that are too old.
 */
static void nc_rebuild(struct cnode *old_head, struct cnode *new_head,
                       struct cnode *n, time_t now)
{
   struct cnode *new;

   if (!n)
   {
      /* Start at head. */
      n = old_head->l;
   }

   if (n == old_head)
   {
      /* Trie is empty. */
      return;
   }

   if (n->b > n->l->b)
   {
      /*
       * If the trie extends via the left branch, follow it
       * recursively.
       */
      nc_rebuild(old_head, new_head, n->l, now);
   }

   if (n->b > n->r->b)
   {
      /*
       * If the trie extends via the right branch, follow it
       * recursively.
       */
      nc_rebuild(old_head, new_head, n->r, now);
   }

   if ((now - n->seen) < OptionsItem->negcache)
   {
      /*
       * We want to keep this node, so insert it into the new
       * trie.
       */
      new = nc_insert(new_head, n->ip);
      new->seen = n->seen;
   }
   else if (OPT_DEBUG >= 2)
   {
      log("NEGCACHE -> Deleting negcache node for %lu added at %lu", n->ip,
          n->seen);
   }

   /* Safe to free() this node now. */
   MyFree(n);
}

/*
 * Wrapper for recursive rebuild function.
 */
void negcache_rebuild(void)
{
   time_t now;
   struct cnode *new_head;

   now = time(NULL);
   new_head = NULL;

   nc_init(&new_head);
   nc_rebuild(nc_head, new_head, NULL, now);
   MyFree(nc_head);
   nc_head = new_head;
}
