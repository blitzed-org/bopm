#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifndef AF_INET6
#define AF_INET6 10
#endif

extern int inetpton(int, const char *, void *);
extern char *inetntop(int, const void *, char *, unsigned int);
extern struct hostent *bopm_gethostbyname(const char *);
