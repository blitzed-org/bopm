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

#line 25 "config.y"

#include <stdio.h>
#include <string.h>
#include "malloc.h"
#include "config.h"

//int yydebug=1; 
void *tmp;        /* Variable to temporarily hold nodes before insertion to list */


#line 66 "config.y"
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
# define YYDEBUG 1
#endif



#define	YYFINAL		183
#define	YYFLAG		-32768
#define	YYNTBASE	39

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 287 ? yytranslate[x] : 85)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    38,    36,
       2,    37,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    34,     2,    35,     2,     2,     2,     2,
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
      26,    27,    28,    29,    30,    31,    32,    33
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
     214,   216,   218,   220,   225,   230,   235,   240,   245,   250,
     255,   260
};
static const short yyrhs[] =
{
      -1,    39,    40,     0,    46,     0,    41,     0,    66,     0,
      72,     0,    16,    34,    42,    35,    36,     0,    42,    43,
       0,    43,     0,    44,     0,    45,     0,     1,     0,    13,
      37,    32,    36,     0,    17,    37,    33,    36,     0,     7,
      34,    47,    35,    36,     0,    47,    48,     0,    48,     0,
      49,     0,    51,     0,    50,     0,    52,     0,    53,     0,
      54,     0,    55,     0,    56,     0,    57,     0,    58,     0,
      59,     0,    60,     0,     1,     0,     3,    37,    33,    36,
       0,    11,    37,    33,    36,     0,    14,    37,    33,    36,
       0,    15,    37,    33,    36,     0,    18,    37,    33,    36,
       0,    19,    37,    32,    36,     0,    22,    37,    33,    36,
       0,    24,    37,    33,    36,     0,    29,    37,    33,    36,
       0,    31,    37,    33,    36,     0,     5,    37,    33,    36,
       0,     0,    61,     4,    34,    62,    35,    36,     0,    62,
      63,     0,    63,     0,    64,     0,    65,     0,    12,    37,
      33,    36,     0,     8,    37,    33,    36,     0,     0,    67,
      30,    34,    68,    35,    36,     0,    68,    69,     0,    69,
       0,    70,     0,    71,     0,     1,     0,     9,    37,    33,
      36,     0,    23,    37,    33,    36,     0,     0,    73,    23,
      34,    74,    35,    36,     0,    74,    75,     0,    75,     0,
      76,     0,    77,     0,    80,     0,    78,     0,    81,     0,
      79,     0,    84,     0,    82,     0,    83,     0,     1,     0,
      12,    37,    33,    36,     0,    31,    37,    33,    36,     0,
      25,    37,    33,    36,     0,    27,    37,    33,    36,     0,
       6,    37,    32,    36,     0,    26,    37,    32,    36,     0,
      28,    37,    32,    36,     0,    10,    37,    32,    36,     0,
      20,    37,    21,    38,    32,    36,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,    78,    79,    82,    82,    83,    84,    90,    92,    92,
      95,    95,    96,    99,   104,   112,   114,   114,   117,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   131,   137,   143,   149,   155,   161,   166,   172,   178,
     184,   190,   199,   199,   216,   216,   219,   219,   222,   230,
     240,   240,   257,   257,   260,   260,   261,   264,   274,   286,
     286,   311,   311,   314,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   325,   332,   339,   346,   353,   359,   365,
     371,   377
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "AWAY", "CHANNEL", "CONNREGEX", "FD", "IRC", 
  "KEY", "MASK", "MAX_READ", "MODE", "NAME", "NEGCACHE", "NICK", "OPER", 
  "OPTIONS", "PIDFILE", "PASSWORD", "PORT", "PROTOCOL", "PROTOCOLTYPE", 
  "REALNAME", "SCANNER", "SERVER", "TARGET_IP", "TARGET_PORT", 
  "TARGET_STRING", "TIMEOUT", "USERNAME", "USER", "VHOST", "NUMBER", 
  "STRING", "'{'", "'}'", "';'", "'='", "':'", "config", "config_items", 
  "options_entry", "options_items", "options_item", "options_negcache", 
  "options_pidfile", "irc_entry", "irc_items", "irc_item", "irc_away", 
  "irc_mode", "irc_nick", "irc_oper", "irc_password", "irc_port", 
  "irc_realname", "irc_server", "irc_username", "irc_vhost", 
  "irc_connregex", "channel_entry", "@1", "channel_items", "channel_item", 
  "channel_name", "channel_key", "user_entry", "@2", "user_items", 
  "user_item", "user_mask", "user_scanner", "scanner_entry", "@3", 
  "scanner_items", "scanner_item", "scanner_name", "scanner_vhost", 
  "scanner_target_ip", "scanner_target_string", "scanner_fd", 
  "scanner_target_port", "scanner_timeout", "scanner_max_read", 
  "scanner_protocol", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    39,    39,    40,    40,    40,    40,    41,    42,    42,
      43,    43,    43,    44,    45,    46,    47,    47,    48,    48,
      48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    61,    60,    62,    62,    63,    63,    64,    65,
      67,    66,    68,    68,    69,    69,    69,    70,    71,    73,
      72,    74,    74,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84
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
       1,     1,     1,     4,     4,     4,     4,     4,     4,     4,
       4,     6
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
       0,     0,     0,    53,    54,    55,    72,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    62,    63,    64,
      66,    68,    65,    67,    70,    71,    69,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    15,     0,
       0,     0,     7,     0,     0,     0,    52,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    61,    31,    41,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
       0,     0,    45,    46,    47,    13,    14,     0,     0,    51,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    60,
       0,     0,     0,    44,    57,    58,    77,    80,    73,     0,
      75,    78,    76,    79,    74,     0,     0,    43,     0,    49,
      48,    81,     0,     0
};

static const short yydefgoto[] =
{
       1,     4,     5,    45,    46,    47,    48,     6,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,   141,   142,   143,   144,     7,     8,    72,
      73,    74,    75,     9,    10,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96
};

static const short yypact[] =
{
  -32768,    26,   -33,   -28,-32768,-32768,-32768,-32768,   -23,-32768,
     -12,    56,    51,   -19,   -18,-32768,   -15,    -2,     1,    11,
      25,    29,    32,    36,    39,    45,    47,    -1,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,    33,-32768,    49,    52,    23,-32768,-32768,-32768,    42,
      71,    22,    30,    46,    55,    57,    59,    24,    60,    61,
      62,    67,    65,-32768,    69,    72,    73,    74,-32768,-32768,
      68,    70,    18,-32768,-32768,-32768,-32768,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    19,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,-32768,     0,
      96,    97,-32768,   101,   102,   100,-32768,   105,   106,   107,
     118,   108,   110,   111,   113,   114,   112,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   109,
     115,    -3,-32768,-32768,-32768,-32768,-32768,   117,   119,-32768,
     120,   121,   122,   116,   123,   124,   125,   126,   127,-32768,
     131,   132,   130,-32768,-32768,-32768,-32768,-32768,-32768,   135,
  -32768,-32768,-32768,-32768,-32768,   133,   134,-32768,   136,-32768,
  -32768,-32768,   143,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768,    63,-32768,-32768,-32768,-32768,    84,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,   -32,-32768,-32768,-32768,-32768,-32768,
      99,-32768,-32768,-32768,-32768,-32768,    64,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768
};


#define	YYLAST		172


static const short yytable[] =
{
      15,    11,    16,   -42,    17,   139,    12,    13,   139,   140,
      18,    14,   140,    19,    20,    49,    50,    21,    22,    69,
      76,    23,    51,    24,    42,    77,   182,    70,    25,    78,
      26,    79,   162,     2,    62,    52,    43,    64,    53,    80,
      44,    71,     3,    69,    81,    82,    83,    84,    54,   -59,
      85,    70,    42,   115,   126,    97,   103,    15,    67,    16,
     -42,    17,    55,    98,    43,    71,    56,    18,    44,    57,
      19,    20,    76,    58,    21,    22,    59,    77,    23,    99,
      24,    78,    60,    79,    61,    25,    65,    26,   100,    66,
     101,    80,   102,   104,   105,   106,    81,    82,    83,    84,
     107,   108,    85,   109,   110,   113,   111,   114,    68,   163,
     112,    63,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   145,   146,   147,   148,   149,   150,   151,   153,
     152,   154,   155,   183,   156,   157,   160,   158,   159,     0,
     127,     0,   161,   164,   169,   165,   166,   167,   168,   170,
     171,   172,   173,   174,   175,   176,   177,   178,     0,   179,
     180,   116,   181
};

static const short yycheck[] =
{
       1,    34,     3,     4,     5,     8,    34,    30,     8,    12,
      11,    23,    12,    14,    15,    34,    34,    18,    19,     1,
       1,    22,    37,    24,     1,     6,     0,     9,    29,    10,
      31,    12,    35,     7,    35,    37,    13,     4,    37,    20,
      17,    23,    16,     1,    25,    26,    27,    28,    37,    23,
      31,     9,     1,    35,    35,    33,    32,     1,    35,     3,
       4,     5,    37,    33,    13,    23,    37,    11,    17,    37,
      14,    15,     1,    37,    18,    19,    37,     6,    22,    33,
      24,    10,    37,    12,    37,    29,    37,    31,    33,    37,
      33,    20,    33,    33,    33,    33,    25,    26,    27,    28,
      33,    36,    31,    34,    32,    37,    33,    37,    45,   141,
      36,    27,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    36,    36,    36,    36,    36,    36,    36,    36,    36,
      36,    36,    36,    36,    33,    33,    36,    32,    32,    21,
      33,    33,    32,     0,    33,    32,    37,    33,    36,    -1,
      86,    -1,    37,    36,    38,    36,    36,    36,    36,    36,
      36,    36,    36,    36,    33,    33,    36,    32,    -1,    36,
      36,    72,    36
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
#line 100 "config.y"
{
   OptionsItem->negcache = yyvsp[-1].number;
;
    break;}
case 14:
#line 105 "config.y"
{
   MyFree(OptionsItem->pidfile);
   OptionsItem->pidfile = DupString(yyvsp[-1].string);
;
    break;}
case 31:
#line 132 "config.y"
{
   MyFree(IRCItem->away);
   IRCItem->away = DupString(yyvsp[-1].string);
;
    break;}
case 32:
#line 138 "config.y"
{
   MyFree(IRCItem->mode);
   IRCItem->mode = DupString(yyvsp[-1].string);
;
    break;}
case 33:
#line 144 "config.y"
{
   MyFree(IRCItem->nick);
   IRCItem->nick = DupString(yyvsp[-1].string);
;
    break;}
case 34:
#line 150 "config.y"
{
   MyFree(IRCItem->oper);
   IRCItem->oper = DupString(yyvsp[-1].string);
;
    break;}
case 35:
#line 156 "config.y"
{
   MyFree(IRCItem->password);
   IRCItem->password = DupString(yyvsp[-1].string);
;
    break;}
case 36:
#line 162 "config.y"
{
   IRCItem->port = yyvsp[-1].number;
;
    break;}
case 37:
#line 167 "config.y"
{
   MyFree(IRCItem->realname);
   IRCItem->realname = DupString(yyvsp[-1].string);
;
    break;}
case 38:
#line 173 "config.y"
{
   MyFree(IRCItem->server);
   IRCItem->server = DupString(yyvsp[-1].string);
;
    break;}
case 39:
#line 179 "config.y"
{
   MyFree(IRCItem->username);
   IRCItem->username = DupString(yyvsp[-1].string);
;
    break;}
case 40:
#line 185 "config.y"
{
   MyFree(IRCItem->vhost);
   IRCItem->vhost = DupString(yyvsp[-1].string);
;
    break;}
case 41:
#line 191 "config.y"
{
   MyFree(IRCItem->connregex);
   IRCItem->connregex = DupString(yyvsp[-1].string);
;
    break;}
case 42:
#line 200 "config.y"
{
   node_t *node;
   struct ChannelConf *item;

   item = MyMalloc(sizeof(struct ChannelConf));

   item->name = DupString("");
   item->key = DupString("");

   node = node_create(item);
   list_add(IRCItem->channels, node);

   tmp = (void *) item;
;
    break;}
case 48:
#line 223 "config.y"
{
   struct ChannelConf *item = tmp;

   MyFree(item->name);
   item->name = DupString(yyvsp[-1].string);
;
    break;}
case 49:
#line 231 "config.y"
{
   struct ChannelConf *item = tmp;

   MyFree(item->key);
   item->key = DupString(yyvsp[-1].string);
;
    break;}
case 50:
#line 241 "config.y"
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
#line 265 "config.y"
{
   struct UserConf *item = (struct UserConf *) tmp;

   node_t *node;
   node = node_create((void *) DupString(yyvsp[-1].string));

   list_add(item->masks, node);
;
    break;}
case 58:
#line 275 "config.y"
{
   struct UserConf *item = (struct UserConf *) tmp;

   node_t *node;
   node = node_create((void *) DupString(yyvsp[-1].string));

   list_add(item->scanners, node);
;
    break;}
case 59:
#line 287 "config.y"
{
   node_t *node;
   struct ScannerConf *item;

   item = MyMalloc(sizeof(struct ScannerConf));

   /* Setup ScannerConf defaults */
   item->name = DupString("undefined");
   item->vhost = DupString("0.0.0.0");
   item->fd = 512;
   item->target_ip = DupString("127.0.0.1");
   item->target_port = 6667;
   item->target_string = DupString("Looking up your hostname...");
   item->timeout = 30;
   item->max_read = 4096;
   item->protocols = list_create();

   node = node_create(item);

   list_add(ScannerItemList, node);
   tmp = (void *) item;
;
    break;}
case 73:
#line 326 "config.y"
{
   struct ScannerConf *item = (struct ScannerConf *) tmp;
   MyFree(item->name);
   item->name = DupString(yyvsp[-1].string);
;
    break;}
case 74:
#line 333 "config.y"
{
   struct ScannerConf *item = (struct ScannerConf *) tmp;
   MyFree(item->vhost);
   item->vhost = DupString(yyvsp[-1].string);
;
    break;}
case 75:
#line 340 "config.y"
{
   struct ScannerConf *item = (struct ScannerConf *) tmp;
   MyFree(item->target_ip);
   item->target_ip = DupString(yyvsp[-1].string);
;
    break;}
case 76:
#line 347 "config.y"
{
   struct ScannerConf *item = (struct ScannerConf *) tmp;
   MyFree(item->target_string);
   item->target_string = DupString(yyvsp[-1].string);
;
    break;}
case 77:
#line 354 "config.y"
{
   struct ScannerConf *item = (struct ScannerConf *) tmp;
   item->fd = yyvsp[-1].number;
;
    break;}
case 78:
#line 360 "config.y"
{
   struct ScannerConf *item = (struct ScannerConf *) tmp;
   item->target_port = yyvsp[-1].number;
;
    break;}
case 79:
#line 366 "config.y"
{
   struct ScannerConf *item = (struct ScannerConf *) tmp;
   item->timeout = yyvsp[-1].number;
;
    break;}
case 80:
#line 372 "config.y"
{
   struct ScannerConf *item = (struct ScannerConf *) tmp;
   item->max_read = yyvsp[-1].number;
;
    break;}
case 81:
#line 378 "config.y"
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
#line 394 "config.y"

