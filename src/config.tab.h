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
# define	MODE	264
# define	NAME	265
# define	NEGCACHE	266
# define	NICK	267
# define	OPER	268
# define	OPTIONS	269
# define	PIDFILE	270
# define	PASSWORD	271
# define	PORT	272
# define	PROTOCOL	273
# define	PROTOCOLTYPE	274
# define	REALNAME	275
# define	SCANNER	276
# define	SERVER	277
# define	TARGET_IP	278
# define	TARGET_PORT	279
# define	TARGET_STRING	280
# define	USERNAME	281
# define	USER	282
# define	VHOST	283
# define	NUMBER	284
# define	STRING	285


extern YYSTYPE yylval;

#endif /* not BISON_CONFIG_TAB_H */
