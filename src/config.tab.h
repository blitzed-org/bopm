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
# define	IRC	260
# define	KEY	261
# define	MASK	262
# define	MODE	263
# define	NAME	264
# define	NEGCACHE	265
# define	NICK	266
# define	OPER	267
# define	OPTIONS	268
# define	PIDFILE	269
# define	PASSWORD	270
# define	PORT	271
# define	REALNAME	272
# define	SCANNER	273
# define	SERVER	274
# define	USERNAME	275
# define	USER	276
# define	VHOST	277
# define	NUMBER	278
# define	STRING	279


extern YYSTYPE yylval;

#endif /* not BISON_CONFIG_TAB_H */
