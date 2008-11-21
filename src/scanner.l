%{
#include <stdio.h>
#include "y.tab.h"
 
%}
              
DSTR          '"' [^"]+ '"'
SSTR          "'" [^']+ "'"
Digits        [0-9]+
SLASH         '/'
PIPE          '|'
PLUS          '+'
DASH          '-'
EQ            '='
NE            '!='
LT            '<'
GT            '>'
GTE           '>='
LTE           '<='
AND           'and'
OR            'or'
MOD           'mod'
DIV           'div'
SPLAT         '*'
LPAREN        '('
RPAREN        ')'
LBRA          '['
RBRA          ']'
DOT           '.'
AT            '@'
COLON         ':'
DOLLAR        '$'
COMMA         ','
COMMENT       'comment'
TEXT          'text' 
PI            'processing-instruction' 
NODE          'node'
ANCESTOR      'ancestor' 
ANCESTORSELF  'ancestor-or-self' 
ATTR	        'attribute' 
CHILD	        'child' 
DESC	        'descendant' 
DESCSELF	    'descendant-or-self' 
FOLLOW	      'following'
FOLLOWSIB	    'following-sibling' 
NS	          'namespace' 
PARENT	      'parent' 
PRE	          'preceding' 
PRESIB	      'preceding-sibling' 
SELF	        'self'
S             [\x20|\x9|\xD|\xA]+


NCNameStartChar	[A-za-z_]
NCNameChar	  [A-za-z_\-\.0-9]
NCName        {NCNameStartChar}{NCNameChar}*

%%