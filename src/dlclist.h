#ifndef DLCLIST_H
#define DLCLIST_H

struct dlclist
{
   struct dlclist *next;
   struct dlclist *prev;
   void *val;
};

extern struct dlclist *dlclist_init(void);
extern void dlclist_delete_node(struct dlclist *n);
extern void dlclist_destroy(struct dlclist *head);
extern struct dlclist *dlclist_insert_before(struct dlclist *n, void *v);
extern struct dlclist *dlclist_insert_after(struct dlclist *n, void *v);

#endif
