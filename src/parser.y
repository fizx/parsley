%{
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kstring.h"

#define YYSTYPE char *

int yylex (void);
void yyerror (char const *);

void start_debugging(void);

int yyparse(void);
void myparse(char*);
  
%}

%glr-parser
%token_table
%debug

%token <string> AT
%token <string> BANG
%token <string> BSLASHLIT
%token <string> CLOSE_PAREN
%token <string> COMMA
%token <string> DOT
%token <string> LPAREN
%token <string> NAME
%token <string> NUMBER
%token <string> COLON
%token <string> OTHER
%token <string> PIPE
%token <string> QUERY
%token <string> RPAREN
%token <string> SLASH
%token <string> STRING
%token <string> STRING_SLASH
%token <string> WHITE
%token <string> HASH
%token <string> LT
%token <string> GT
%token <string> LBRA
%token <string> RBRA
%token <string> TILDE
%token <string> SPLAT
%token <string> PLUS
%token <string> EQ

%token <string> CXEQUATION
%token <string> CXOPHE
%token <string> CXOPNE
%token <string> CXOPSTARTEQ
%token <string> CXOPENDEQ
%token <string> CXOPCONTAINS
%token <string> CXOPCONTAINS2
%token <string> CXFIRST
%token <string> CXLAST
%token <string> CXNOT
%token <string> CXEVEN
%token <string> CXODD
%token <string> CXEQ
%token <string> CXGT
%token <string> CXLT
%token <string> CXHEADER
%token <string> CXCONTAINS
%token <string> CXEMPTY
%token <string> CXHAS
%token <string> CXPARENT
%token <string> CXHIDDEN
%token <string> CXVISIBLE
%token <string> CXNTHCH
%token <string> CXFIRSTCH
%token <string> CXLASTCH
%token <string> CXONLYCH
%token <string> CXINPUT
%token <string> CXTEXT
%token <string> CXPASSWORD
%token <string> CXRADIO
%token <string> CXCHECKBOX
%token <string> CXSUBMIT
%token <string> CXIMAGE
%token <string> CXRESET
%token <string> CXBUTTON
%token <string> CXFILE
%token <string> CXENABLED
%token <string> CXDISABLED
%token <string> CXCHECKED
%token <string> CXSELECTED


%%

optwhite:
    WHITE     %dprec 2  
  |           %dprec 1        {$$="";}
  ;
union: 
    optwhite selector optwhite COMMA union      { $$ = astrcat4("//", $2, " | ", $5); }  
  | optwhite selector optwhite  { $$ = astrcat("//", $2); }  
  ;  
selector:
    css_clause clause_operator selector   { $$ = astrcat3($1, $2, $3); }
  | css_clause
  ;
css_clause:
    name css_filters       { $$ = astrcat($1, $2); }
  | css_filters            { $$ = astrcat("*", $1); }
  | name
  ;
css_filters:
    css_filter css_filters    { $$ = astrcat($1, $2); }
  | css_filter
  ;
css_filter:
    id
  | class
  | attribute_filter
  | contains_filter
  | math_filter
  | selector_filter
  | nth_child_filter
  | simple_filter
  ;
selector_filter:
    CXHAS LPAREN selector RPAREN
  | CXNOT LPAREN selector RPAREN  { $$ = astrcat(" except //", $3); }
  ;
nth_child_filter:
    CXNTHCH LPAREN nth_argument RPAREN
  ;
contains_filter:
    CXCONTAINS LPAREN STRING RPAREN
  ;
math_filter:
    EQ LPAREN NUMBER RPAREN   { $$ = astrcat3("[position() = ", $3, "]"); }
  | GT LPAREN NUMBER RPAREN   { $$ = astrcat3("[position() > ", $3, "]"); }
  | LT LPAREN NUMBER RPAREN   { $$ = astrcat3("[position() < ", $3, "]"); }
  ;
attribute_filter:
    LBRA attribute_contents RBRA  { $$ = $2; }
  ;
attribute_contents:
    NAME                                           { $$ = astrcat3("[@", $1, "]"); }
  | NAME optwhite EQ optwhite STRING         { $$ = astrcat5("[@", $1, " = \"", esc($5), "\"]"); }
  | NAME optwhite CXOPNE optwhite STRING         { $$ = astrcat5("[@", $1, " != \"", esc($5), "\"]"); }
  | NAME optwhite CXOPHE optwhite STRING         { $$ = astrcat9("[@", $1, "=\"", $5, "\" or starts-with(@", $1, ", concat(\"", $5, "\", \"-\"))]"); }
  | NAME optwhite CXOPSTARTEQ optwhite STRING    { $$ = astrcat5("[starts-with(@", $1, ", \"", esc($5), "\")]"); }
  | NAME optwhite CXOPENDEQ optwhite STRING      { $$ = astrcat5("[ends-with(@", $1, ", \"", esc($5), "\")]"); }
  | NAME optwhite CXOPCONTAINS optwhite STRING   { $$ = attr_contains($1, $5); }
  | NAME optwhite CXOPCONTAINS2 optwhite STRING  { $$ = attr_contains($1, $5); }
  ;
clause_operator:
    WHITE                       { $$ = "//"; }
  | optwhite GT optwhite      { $$ = "/"; }
  | optwhite PLUS optwhite      { $$ = "/following-sibling::*[1]/self::"; }
  | optwhite TILDE optwhite     { $$ = "/following-sibling::*/self::"; }
  ;
nth_argument:
    NAME
  | NUMBER
  | CXEQUATION
  ;
simple_filter:
    CXFIRST           { $$ = "[1]"; }
  | CXLAST            { $$ = "[last()]"; }
  | CXEVEN            { $$ = "[position() % 2 = 0]"; }
  | CXODD             { $$ = "[position() % 2 = 1]"; }
  | CXHEADER          { $$ = "[contains(\"h1 h2 h3 h4 h5 h6\", lower-case(local-name()))]"; }
  | CXEMPTY
  | CXPARENT
  | CXHIDDEN
  | CXVISIBLE
  | CXFIRSTCH
  | CXLASTCH
  | CXONLYCH
  | CXINPUT
  | CXTEXT
  | CXPASSWORD
  | CXRADIO
  | CXCHECKBOX
  | CXSUBMIT
  | CXIMAGE
  | CXRESET
  | CXBUTTON
  | CXFILE
  | CXENABLED
  | CXDISABLED
  | CXCHECKED
  | CXSELECTED
  ;
id:
    HASH name         { $$ = astrcat3("[@id=\"", $2, "\"]"); }
  ;
class:
    DOT name          { $$ = attr_contains("class", $2); }
  ;
name:
    frag name { $$ = astrcat($1, $2); }
  | frag
  | SPLAT
  ;
frag:
    BSLASHLIT { $$ = strdup($1 + 1); }
  | NAME
  ;









%%

void yyerror (const char * s) {
  printf("%s\n", s);
  exit(1);
}

void start_debugging(){
  yydebug = 1;
  return;
}