#ifndef BISON_CONFIG_TAB_H
# define BISON_CONFIG_TAB_H

#ifndef YYSTYPE
typedef union 
{
        int number;
        char *string;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	AWAY	257
# define	CHANNEL	258
# define	CONNREGEX	259
# define	FD	260
# define	IRC	261
# define	KEY	262
# define	MASK	263
# define	MAX_READ	264
# define	MODE	265
# define	NAME	266
# define	NEGCACHE	267
# define	NICK	268
# define	OPER	269
# define	OPTIONS	270
# define	PIDFILE	271
# define	PASSWORD	272
# define	PORT	273
# define	PROTOCOL	274
# define	PROTOCOLTYPE	275
# define	REALNAME	276
# define	SCANNER	277
# define	SERVER	278
# define	TARGET_IP	279
# define	TARGET_PORT	280
# define	TARGET_STRING	281
# define	TIMEOUT	282
# define	USERNAME	283
# define	USER	284
# define	VHOST	285
# define	NUMBER	286
# define	STRING	287


extern YYSTYPE yylval;

#endif /* not BISON_CONFIG_TAB_H */
