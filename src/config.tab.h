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
# define	BLACKLIST	258
# define	CHANNEL	259
# define	CONNREGEX	260
# define	DNSBL_FROM	261
# define	DNSBL_TO	262
# define	FD	263
# define	IRC	264
# define	KEY	265
# define	MASK	266
# define	MAX_READ	267
# define	MODE	268
# define	NAME	269
# define	NEGCACHE	270
# define	NICK	271
# define	OPER	272
# define	OPM	273
# define	OPTIONS	274
# define	PIDFILE	275
# define	PASSWORD	276
# define	PORT	277
# define	PROTOCOL	278
# define	PROTOCOLTYPE	279
# define	REALNAME	280
# define	SCANNER	281
# define	SENDMAIL	282
# define	SERVER	283
# define	TARGET_IP	284
# define	TARGET_PORT	285
# define	TARGET_STRING	286
# define	TIMEOUT	287
# define	USERNAME	288
# define	USER	289
# define	VHOST	290
# define	NUMBER	291
# define	STRING	292


extern YYSTYPE yylval;

#endif /* not BISON_CONFIG_TAB_H */
