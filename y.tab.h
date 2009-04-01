/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison GLR parsers in C

   Copyright (C) 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NUMBER = 258,
     S = 259,
     AT = 260,
     LPAREN = 261,
     RPAREN = 262,
     PIPE = 263,
     LT = 264,
     SLASH = 265,
     DBLSLASH = 266,
     BANG = 267,
     COLON = 268,
     DBLCOLON = 269,
     QUERY = 270,
     HASH = 271,
     COMMA = 272,
     DOT = 273,
     DBLDOT = 274,
     GT = 275,
     LBRA = 276,
     RBRA = 277,
     TILDE = 278,
     SPLAT = 279,
     PLUS = 280,
     DASH = 281,
     EQ = 282,
     LTE = 283,
     GTE = 284,
     DOLLAR = 285,
     BSLASHLIT = 286,
     OTHER = 287,
     XANCESTOR = 288,
     XANCESTORSELF = 289,
     XATTR = 290,
     XCHILD = 291,
     XDESC = 292,
     XDESCSELF = 293,
     XFOLLOW = 294,
     XFOLLOWSIB = 295,
     XNS = 296,
     XPARENT = 297,
     XPRE = 298,
     XPRESIB = 299,
     XSELF = 300,
     XOR = 301,
     XAND = 302,
     XDIV = 303,
     XMOD = 304,
     XCOMMENT = 305,
     XTEXT = 306,
     XPI = 307,
     XNODE = 308,
     CXEQUATION = 309,
     CXOPHE = 310,
     CXOPNE = 311,
     CXOPSTARTEQ = 312,
     CXOPENDEQ = 313,
     CXOPCONTAINS = 314,
     CXOPCONTAINS2 = 315,
     CXFIRST = 316,
     CXLAST = 317,
     CXNOT = 318,
     CXEVEN = 319,
     CXODD = 320,
     CXEQ = 321,
     CXGT = 322,
     CXLT = 323,
     CXHEADER = 324,
     CXCONTAINS = 325,
     CXEMPTY = 326,
     CXHAS = 327,
     CXPARENT = 328,
     CXNTHCH = 329,
     CXNTHLASTCH = 330,
     CXNTHTYPE = 331,
     CXNTHLASTTYPE = 332,
     CXFIRSTCH = 333,
     CXLASTCH = 334,
     CXFIRSTTYPE = 335,
     CXLASTTYPE = 336,
     CXONLYCH = 337,
     CXONLYTYPE = 338,
     CXINPUT = 339,
     CXTEXT = 340,
     CXPASSWORD = 341,
     CXRADIO = 342,
     CXCHECKBOX = 343,
     CXSUBMIT = 344,
     CXIMAGE = 345,
     CXRESET = 346,
     CXBUTTON = 347,
     CXFILE = 348,
     CXENABLED = 349,
     CXDISABLED = 350,
     CXCHECKED = 351,
     CXSELECTED = 352,
     NAME = 353,
     STRING = 354
   };
#endif


/* Copy the first part of user declarations.  */
#line 1 "parser.y"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsed_xpath.h"
#include <libxml/hash.h>

#ifndef PARSER_Y_H_INCLUDED
#define PARSER_Y_H_INCLUDED

static pxpathPtr parsed_answer;

int yylex (void);
void yyerror (char const *);

void prepare_parse(char*);
void cleanup_parse(void);
void start_debugging(void);

static xmlHashTablePtr alias_hash;

char* xpath_alias(char*);
void init_xpath_alias();

int yyparse(void);
pxpathPtr myparse(char*);
void answer(pxpathPtr);

#define LIT_BIN_OP(A, B, C)           pxpath_cat_literals(3, A, LIT(B), C)
#define BIN_OP(A, B, C)               pxpath_cat_paths(3, A, OP(B), C)
#define PREP_OP(A, B)                 pxpath_cat_paths(2, OP(A), B)
#define PXP(A)                        pxpath_new_path(1, A)
#define LIT(A)                        pxpath_new_literal(1, A)
#define OP(A)   	                    pxpath_new_operator(1, A)
#define APPEND(A, S)                  pxpath_cat_paths(2, A, PXP(S)); 
#define PREPEND(A, S)                 pxpath_cat_paths(2, PXP(S), A); 
#define PXPWRAP(A, B, C)              pxpath_cat_paths(3, PXP(A), B, PXP(C))
#define P4E(A, B, C, D)               pxpath_cat_paths(4, A, PXP(B), C, PXP(D))
#define P4O(A, B, C, D)               pxpath_cat_paths(4, PXP(A), B, PXP(C), D)
#define P6E(A, B, C, D, E, F)         pxpath_cat_paths(6, A, PXP(B), C, PXP(D), E, PXP(F));
#define INPUT_TYPE(A, S)              APPEND(A, "[lower-case(name())='input' and lower-case(@type)='" #S "']")
#define TRACE(A, B)                   fprintf(stderr, "trace(%s): ", A); fprintf(stderr, "%s\n", pxpath_to_string(B));

#endif
  


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE 
#line 53 "parser.y"
{
  int empty;
	char* string;
  pxpathPtr node;
}
/* Line 2616 of glr.c.  */
#line 202 "y.tab.h"
	YYSTYPE;
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{

  char yydummy;

} YYLTYPE;
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;



