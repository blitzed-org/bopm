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
# define	IRC	259
# define	KEY	260
# define	MASK	261
# define	MODE	262
# define	NAME	263
# define	NEGCACHE	264
# define	NICK	265
# define	OPER	266
# define	OPTIONS	267
# define	PIDFILE	268
# define	PASSWORD	269
# define	PORT	270
# define	REALNAME	271
# define	SCANNER	272
# define	SERVER	273
# define	USERNAME	274
# define	USER	275
# define	VHOST	276
# define	NUMBER	277
# define	STRING	278


extern YYSTYPE yylval;

#endif /* not BISON_CONFIG_TAB_H */
