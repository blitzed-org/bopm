#ifndef COMPAT_H
#define COMPAT_H

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

#ifndef HAVE_INET_ATON
extern int inet_aton(const char *cp, struct in_addr *inp);
#endif

#endif
