%{
#include "Node.h"
#include "syntax.tab.h"
int yycolumn = 1;
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
%}

%option yylineno

NEWLINE [\n\r]+
WS [ \t]+
STRUCT  "struct"
RETURN  return
IF  if
ELSE  else
WHILE  while
DIGIT [0-9]
LETTER [_a-zA-Z]
LETTERSEC [0-9_a-zA-Z]
RELOP 	>|<|>=|<=|==|!=
TYPE int|float
SEMI  ";"
COMMA  ","
ASSIGNOP "="
ADD  "+"
SUB  "-"
MUL  "*"
DIV  "/"
AND "&&"
OR "||"
NOT "!"
DOT "."
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
ID {LETTER}{LETTERSEC}*
INT 0|[1-9]+[0-9]*
FLOAT {DIGIT}+"."{DIGIT}+|{DIGIT}*"."{DIGIT}+[eE][+-]?{DIGIT}+|{DIGIT}+"."{DIGIT}*[eE][+-]?{DIGIT}+
%%
{INT} { return INT;}
{FLOAT} {return FLOAT;}
{STRUCT} {return STRUCT;}
{RETURN} {return RETURN;}
{TYPE} {return TYPE;}
{IF} {return IF;}
{ELSE} {return ELSE;}
{WHILE} {return WHILE;}
{ID} {return ID;}
{ADD} {return ADD;}
{SUB} {return SUB;}
{MUL} {return MUL;}
{DIV} {return DIV;}
{AND} {return AND;}
{OR} {return OR; }
{NOT} {return NOT;}
{WS} {}
{RELOP} {return RELOP;}
{ASSIGNOP} {return ASSIGNOP;}
{SEMI} {return SEMI;}
{NEWLINE} {yycolumn = 1;}
{COMMA} {return COMMA;}
{DOT} {return DOT;}
{LB} {return LB;}
{RB} {return RB;}
{LP} {return LP;}
{RP} {return RP;}
{LC} {return LC;}
{RC} {return RC;}
. {printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno, yytext);}
%%