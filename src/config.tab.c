/* A Bison parser, made from config.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

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

#line 25 "config.y"

#include <stdio.h>
#include <string.h>
#include "malloc.h"
#include "config.h"

int yydebug=1; 
void *tmp;        /* Variable to temporarily hold nodes before insertion to list */


#line 64 "config.y"
#ifndef YYSTYPE
typedef union 
{
        int number;
        char *string;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		173
#define	YYFLAG		-32768
#define	YYNTBASE	37

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 285 ? yytranslate[x] : 81)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    36,    34,
       2,    35,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    32,     2,    33,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     1,     4,     6,     8,    10,    12,    18,    21,
      23,    25,    27,    29,    34,    39,    45,    48,    50,    52,
      54,    56,    58,    60,    62,    64,    66,    68,    70,    72,
      74,    76,    81,    86,    91,    96,   101,   106,   111,   116,
     121,   126,   131,   132,   139,   142,   144,   146,   148,   153,
     158,   159,   166,   169,   171,   173,   175,   177,   182,   187,
     188,   195,   198,   200,   202,   204,   206,   208,   210,   212,
     214,   216,   221,   226,   231,   236,   241,   246
};
static const short yyrhs[] =
{
      -1,    37,    38,     0,    44,     0,    39,     0,    64,     0,
      70,     0,    15,    32,    40,    33,    34,     0,    40,    41,
       0,    41,     0,    42,     0,    43,     0,     1,     0,    12,
      35,    30,    34,     0,    16,    35,    31,    34,     0,     7,
      32,    45,    33,    34,     0,    45,    46,     0,    46,     0,
      47,     0,    49,     0,    48,     0,    50,     0,    51,     0,
      52,     0,    53,     0,    54,     0,    55,     0,    56,     0,
      57,     0,    58,     0,     1,     0,     3,    35,    31,    34,
       0,    10,    35,    31,    34,     0,    13,    35,    31,    34,
       0,    14,    35,    31,    34,     0,    17,    35,    31,    34,
       0,    18,    35,    30,    34,     0,    21,    35,    31,    34,
       0,    23,    35,    31,    34,     0,    27,    35,    31,    34,
       0,    29,    35,    31,    34,     0,     5,    35,    31,    34,
       0,     0,    59,     4,    32,    60,    33,    34,     0,    60,
      61,     0,    61,     0,    62,     0,    63,     0,    11,    35,
      31,    34,     0,     8,    35,    31,    34,     0,     0,    65,
      28,    32,    66,    33,    34,     0,    66,    67,     0,    67,
       0,    68,     0,    69,     0,     1,     0,     9,    35,    31,
      34,     0,    22,    35,    31,    34,     0,     0,    71,    22,
      32,    72,    33,    34,     0,    72,    73,     0,    73,     0,
      74,     0,    75,     0,    78,     0,    76,     0,    79,     0,
      77,     0,    80,     0,     1,     0,    11,    35,    31,    34,
       0,    29,    35,    31,    34,     0,    24,    35,    31,    34,
       0,    26,    35,    31,    34,     0,     6,    35,    30,    34,
       0,    25,    35,    30,    34,     0,    19,    35,    20,    36,
      30,    34,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,    76,    77,    80,    80,    81,    82,    88,    90,    90,
      93,    93,    94,    97,   102,   110,   112,   112,   115,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   129,   135,   141,   147,   153,   159,   164,   170,   176,
     182,   188,   197,   197,   214,   214,   217,   217,   220,   228,
     238,   238,   255,   255,   258,   258,   259,   262,   272,   284,
     284,   306,   306,   309,   309,   310,   311,   312,   313,   314,
     315,   318,   325,   332,   339,   346,   352,   358
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "AWAY", "CHANNEL", "CONNREGEX", "FD", "IRC", 
  "KEY", "MASK", "MODE", "NAME", "NEGCACHE", "NICK", "OPER", "OPTIONS", 
  "PIDFILE", "PASSWORD", "PORT", "PROTOCOL", "PROTOCOLTYPE", "REALNAME", 
  "SCANNER", "SERVER", "TARGET_IP", "TARGET_PORT", "TARGET_STRING", 
  "USERNAME", "USER", "VHOST", "NUMBER", "STRING", "'{'", "'}'", "';'", 
  "'='", "':'", "config", "config_items", "options_entry", 
  "options_items", "options_item", "options_negcache", "options_pidfile", 
  "irc_entry", "irc_items", "irc_item", "irc_away", "irc_mode", 
  "irc_nick", "irc_oper", "irc_password", "irc_port", "irc_realname", 
  "irc_server", "irc_username", "irc_vhost", "irc_connregex", 
  "channel_entry", "@1", "channel_items", "channel_item", "channel_name", 
  "channel_key", "user_entry", "@2", "user_items", "user_item", 
  "user_mask", "user_scanner", "scanner_entry", "@3", "scanner_items", 
  "scanner_item", "scanner_name", "scanner_vhost", "scanner_target_ip", 
  "scanner_target_string", "scanner_fd", "scanner_target_port", 
  "scanner_protocol", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    37,    37,    38,    38,    38,    38,    39,    40,    40,
      41,    41,    41,    42,    43,    44,    45,    45,    46,    46,
      46,    46,    46,    46,    46,    46,    46,    46,    46,    46,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    59,    58,    60,    60,    61,    61,    62,    63,
      65,    64,    66,    66,    67,    67,    67,    68,    69,    71,
      70,    72,    72,    73,    73,    73,    73,    73,    73,    73,
      73,    74,    75,    76,    77,    78,    79,    80
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     0,     2,     1,     1,     1,     1,     5,     2,     1,
       1,     1,     1,     4,     4,     5,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     0,     6,     2,     1,     1,     1,     4,     4,
       0,     6,     2,     1,     1,     1,     1,     4,     4,     0,
       6,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     4,     4,     4,     4,     4,     6
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       1,    50,     0,     0,     2,     4,     3,     5,     0,     6,
       0,     0,     0,     0,     0,    30,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,    18,
      20,    19,    21,    22,    23,    24,    25,    26,    27,    28,
      29,     0,    12,     0,     0,     0,     9,    10,    11,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    16,     0,     0,     0,     0,     8,    56,
       0,     0,     0,    53,    54,    55,    70,     0,     0,     0,
       0,     0,     0,     0,     0,    62,    63,    64,    66,    68,
      65,    67,    69,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    15,     0,     0,     0,     7,     0,
       0,     0,    52,     0,     0,     0,     0,     0,     0,     0,
       0,    61,    31,    41,    32,    33,    34,    35,    36,    37,
      38,    39,    40,     0,     0,     0,    45,    46,    47,    13,
      14,     0,     0,    51,     0,     0,     0,     0,     0,     0,
       0,    60,     0,     0,     0,    44,    57,    58,    75,    71,
       0,    73,    76,    74,    72,     0,     0,    43,     0,    49,
      48,    77,     0,     0
};

static const short yydefgoto[] =
{
       1,     4,     5,    45,    46,    47,    48,     6,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,   135,   136,   137,   138,     7,     8,    72,
      73,    74,    75,     9,    10,    84,    85,    86,    87,    88,
      89,    90,    91,    92
};

static const short yypact[] =
{
  -32768,    58,   -31,   -25,-32768,-32768,-32768,-32768,   -18,-32768,
     -11,    49,    44,     3,     4,-32768,   -14,    -4,     6,    13,
      14,    26,    33,    36,    40,    42,    46,    -1,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,    35,-32768,    48,    50,    22,-32768,-32768,-32768,     5,
      63,     9,    53,    55,    59,    60,    62,    64,    65,    66,
      67,    68,    45,-32768,    69,    70,    71,    61,-32768,-32768,
      72,    73,    24,-32768,-32768,-32768,-32768,    74,    75,    76,
      77,    78,    79,    80,    18,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,-32768,     7,    93,    94,-32768,    98,
      99,    97,-32768,   102,   103,   113,   104,   106,   107,   108,
     109,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,   105,   110,    -3,-32768,-32768,-32768,-32768,
  -32768,   112,   114,-32768,   115,   116,   101,   117,   118,   119,
     120,-32768,   111,   124,   122,-32768,-32768,-32768,-32768,-32768,
     127,-32768,-32768,-32768,-32768,   125,   126,-32768,   128,-32768,
  -32768,-32768,   141,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768,   121,-32768,-32768,-32768,-32768,   131,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,   -32,-32768,-32768,-32768,-32768,-32768,
      32,-32768,-32768,-32768,-32768,-32768,    21,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768
};


#define	YYLAST		166


static const short yytable[] =
{
      15,    11,    16,   -42,    17,   133,    69,    12,   134,    18,
      13,    14,    19,    20,    70,   133,    21,    22,   134,    76,
      23,    51,    24,    42,    77,    69,    25,    71,    26,    78,
     154,    52,    62,    70,    43,    49,    50,    79,    44,    64,
      93,    53,    80,    81,    82,    42,    71,    83,    54,    55,
      15,   120,    16,   -42,    17,    67,    43,   111,   172,    18,
      44,    56,    19,    20,    76,     2,    21,    22,    57,    77,
      23,    58,    24,     3,    78,    59,    25,    60,    26,   104,
     -59,    61,    79,    65,    94,    66,    95,    80,    81,    82,
      96,    97,    83,    98,    99,   108,   100,   101,   102,   103,
     106,   105,   107,   155,   112,   121,     0,   109,   110,   113,
     114,   115,   116,   117,   118,   119,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   139,   140,   141,
     142,   143,   144,   146,   145,   147,   148,   160,   149,   150,
     152,   173,   165,   151,     0,   153,   156,     0,   157,   158,
     159,   161,   162,   163,   164,   166,   167,   168,    63,   169,
     170,     0,   171,     0,     0,     0,    68
};

static const short yycheck[] =
{
       1,    32,     3,     4,     5,     8,     1,    32,    11,    10,
      28,    22,    13,    14,     9,     8,    17,    18,    11,     1,
      21,    35,    23,     1,     6,     1,    27,    22,    29,    11,
      33,    35,    33,     9,    12,    32,    32,    19,    16,     4,
      31,    35,    24,    25,    26,     1,    22,    29,    35,    35,
       1,    33,     3,     4,     5,    33,    12,    33,     0,    10,
      16,    35,    13,    14,     1,     7,    17,    18,    35,     6,
      21,    35,    23,    15,    11,    35,    27,    35,    29,    34,
      22,    35,    19,    35,    31,    35,    31,    24,    25,    26,
      31,    31,    29,    31,    30,    34,    31,    31,    31,    31,
      30,    32,    31,   135,    72,    84,    -1,    35,    35,    35,
      35,    35,    35,    35,    35,    35,    34,    34,    34,    34,
      34,    34,    34,    34,    34,    34,    34,    34,    34,    31,
      31,    34,    30,    20,    31,    31,    30,    36,    31,    31,
      35,     0,    31,    34,    -1,    35,    34,    -1,    34,    34,
      34,    34,    34,    34,    34,    31,    34,    30,    27,    34,
      34,    -1,    34,    -1,    -1,    -1,    45
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 13:
#line 98 "config.y"
{
   OptionsItem->negcache = yyvsp[-1].number;
;
    break;}
case 14:
#line 103 "config.y"
{
   MyFree(OptionsItem->pidfile);
   OptionsItem->pidfile = DupString(yyvsp[-1].string);
;
    break;}
case 31:
#line 130 "config.y"
{
   MyFree(IRCItem->away);
   IRCItem->away = DupString(yyvsp[-1].string);
;
    break;}
case 32:
#line 136 "config.y"
{
   MyFree(IRCItem->mode);
   IRCItem->mode = DupString(yyvsp[-1].string);
;
    break;}
case 33:
#line 142 "config.y"
{
   MyFree(IRCItem->nick);
   IRCItem->nick = DupString(yyvsp[-1].string);
;
    break;}
case 34:
#line 148 "config.y"
{
   MyFree(IRCItem->oper);
   IRCItem->oper = DupString(yyvsp[-1].string);
;
    break;}
case 35:
#line 154 "config.y"
{
   MyFree(IRCItem->password);
   IRCItem->password = DupString(yyvsp[-1].string);
;
    break;}
case 36:
#line 160 "config.y"
{
   IRCItem->port = yyvsp[-1].number;
;
    break;}
case 37:
#line 165 "config.y"
{
   MyFree(IRCItem->realname);
   IRCItem->realname = DupString(yyvsp[-1].string);
;
    break;}
case 38:
#line 171 "config.y"
{
   MyFree(IRCItem->server);
   IRCItem->server = DupString(yyvsp[-1].string);
;
    break;}
case 39:
#line 177 "config.y"
{
   MyFree(IRCItem->username);
   IRCItem->username = DupString(yyvsp[-1].string);
;
    break;}
case 40:
#line 183 "config.y"
{
   MyFree(IRCItem->vhost);
   IRCItem->vhost = DupString(yyvsp[-1].string);
;
    break;}
case 41:
#line 189 "config.y"
{
   MyFree(IRCItem->connregex);
   IRCItem->connregex = DupString(yyvsp[-1].string);
;
    break;}
case 42:
#line 198 "config.y"
{
   node_t *node;
   struct ChannelConf *item;

   item = MyMalloc(sizeof(struct UserConf));

   item->name = DupString("");
   item->key = DupString("");

   node = node_create(item);
   list_add(IRCItem->channels, node);

   tmp = (void *) item;
;
    break;}
case 48:
#line 221 "config.y"
{
   struct ChannelConf *item = tmp;

   MyFree(item->name);
   item->name = DupString(yyvsp[-1].string);
;
    break;}
case 49:
#line 229 "config.y"
{
   struct ChannelConf *item = tmp;

   MyFree(item->name);
   item->name = DupString(yyvsp[-1].string);
;
    break;}
case 50:
#line 239 "config.y"
{
   node_t *node;
   struct UserConf *item;

   item = MyMalloc(sizeof(struct UserConf));

   item->masks = list_create();
   item->scanners = list_create();

   node = node_create(item);
   list_add(UserItemList, node);

   tmp = (void *) item; 
;
    break;}
case 57:
#line 263 "config.y"
{
   struct UserConf *item = (struct UserConf *) tmp;

   node_t *node;
   node = node_create((void *) DupString(yyvsp[-1].string));

   list_add(item->masks, node);
;
    break;}
case 58:
#line 273 "config.y"
{
   struct UserConf *item = (struct UserConf *) tmp;

   node_t *node;
   node = node_create((void *) DupString(yyvsp[-1].string));

   list_add(item->scanners, node);
;
    break;}
case 59:
#line 285 "config.y"
{
   node_t *node;
   struct ScannerConf *item;

   item = MyMalloc(sizeof(struct ScannerConf));

   /* Setup ScannerConf defaults */
   item->name = DupString("undefined");
   item->vhost = "0.0.0.0";
   item->fd = 512;
   item->target_ip = DupString("127.0.0.1");
   item->target_port = 6667;
   item->target_string = DupString("Looking up your hostname...");
   item->protocols = list_create();

   node = node_create(item);
   list_add(ScannerItemList, node);
   tmp = (void *) item;
;
    break;}
case 71:
#line 319 "config.y"
{
   struct ScannerConf *item = (struct ScannerConf *) tmp;
   MyFree(item->name);
   item->name = DupString(yyvsp[-1].string);
;
    break;}
case 72:
#line 326 "config.y"
{
   struct ScannerConf *item = (struct ScannerConf *) tmp;
   MyFree(item->vhost);
   item->vhost = DupString(yyvsp[-1].string);
;
    break;}
case 73:
#line 333 "config.y"
{
   struct ScannerConf *item = (struct ScannerConf *) tmp;
   MyFree(item->target_ip);
   item->vhost = DupString(yyvsp[-1].string);
;
    break;}
case 74:
#line 340 "config.y"
{
   struct ScannerConf *item = (struct ScannerConf *) tmp;
   MyFree(item->target_string);
   item->vhost = DupString(yyvsp[-1].string);
;
    break;}
case 75:
#line 347 "config.y"
{
   struct ScannerConf *item = (struct ScannerConf *) tmp;
   item->fd = yyvsp[-1].number;
;
    break;}
case 76:
#line 353 "config.y"
{
   struct ScannerConf *item = (struct ScannerConf *) tmp;
   item->target_port = yyvsp[-1].number;
;
    break;}
case 77:
#line 359 "config.y"
{
   struct ProtocolConf *item;
   struct ScannerConf  *item2;

   node_t *node;
 
   item = MyMalloc(sizeof(struct ProtocolConf));
   item->type = yyvsp[-3].number;
   item->port = yyvsp[-1].number;

   item2 = (struct ScannerConf *) tmp;

   node = node_create(item);
   list_add(item2->protocols, node);
;
    break;}
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 375 "config.y"

