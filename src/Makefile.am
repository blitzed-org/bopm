bin_PROGRAMS = bopm bopchecker

AM_CPPFLAGS = -DBOPM_ETCDIR="\"$(sysconfdir)\"" -DBOPM_LOGDIR="\"$(localstatedir)\""

bopm_SOURCES = compat.c compat.h config.c config.h config.tab.c config.tab.h \
    dlclist.c dlclist.h dnsbl.c dnsbl.h extern.h inet.c inet.h irc.c irc.h \
    log.c log.h main.c match.c match.h misc.c misc.h malloc.c malloc.h lex.yy.c \
    list.c list.h negcache.c negcache.h opercmd.c opercmd.h options.h scan.c \
    scan.h scanwarn.c scanwarn.h setup.h stats.c stats.h

bopchecker_SOURCES = bopchecker.c bopchecker.h compat.c compat.h config.c \
    config.h inet.c inet.h match.c match.h misc.c misc.h scan.c scan.h \
    setup.h

bopm_LDADD = ../libopm/src/libopm.a @LNSL@ @LSOCKET@
bopchecker_LDADD = @LNSL@ @LSOCKET@
