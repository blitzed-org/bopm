#ifndef NEGCACHE_H
#define NEGCACHE_H

struct cnode
{
    unsigned long ip;	/* IP address, network byte order. */
    time_t seen;		/* When it was last seen. */
    unsigned int b;		/* Index of bit to be tested. */
    struct cnode *l;	/* Node to the left. */
    struct cnode *r;	/* Node to the right. */
};

extern void nc_init(struct cnode **head);
extern struct cnode *check_neg_cache(const unsigned long ip);
extern void negcache_insert(const char *ipstr);

#endif
