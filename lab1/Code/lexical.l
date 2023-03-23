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
PLUS  "+"
MINUS  "-"
STAR  "*"
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
{INT} { yylval.nodePtr =getLexicalUnitNode(yylineno,"INT",TOKEN_TYPE_INT,yytext); return INT;}
{FLOAT} {yylval.nodePtr=getLexicalUnitNode(yylineno,"FLOAT",TOKEN_TYPE_FLOAT,yytext); return FLOAT;}
{STRUCT} {yylval.nodePtr=getLexicalUnitNode(yylineno,"STRUCT",TOKEN_TYPE_OTHER,yytext);  return STRUCT;}
{RETURN} {yylval.nodePtr=getLexicalUnitNode(yylineno,"RETURN",TOKEN_TYPE_OTHER,yytext); return RETURN;}
{TYPE} {yylval.nodePtr=getLexicalUnitNode(yylineno,"TYPE",TOKEN_TYPE_TYPE,yytext);  return TYPE;}
{IF} {yylval.nodePtr=getLexicalUnitNode(yylineno,"IF",TOKEN_TYPE_OTHER,yytext);  return IF;}
{ELSE} {yylval.nodePtr=getLexicalUnitNode(yylineno,"ELSE",TOKEN_TYPE_OTHER,yytext);  return ELSE;}
{WHILE} {yylval.nodePtr=getLexicalUnitNode(yylineno,"WHILE",TOKEN_TYPE_OTHER,yytext); return WHILE;}
{ID} {yylval.nodePtr=getLexicalUnitNode(yylineno,"ID",TOKEN_TYPE_ID,yytext);  return ID;}
{PLUS} {yylval.nodePtr=getLexicalUnitNode(yylineno,"PLUS",TOKEN_TYPE_OTHER,yytext); return PLUS;}
{MINUS} {yylval.nodePtr=getLexicalUnitNode(yylineno,"MINUS",TOKEN_TYPE_OTHER,yytext);return MINUS;}
{STAR} {yylval.nodePtr=getLexicalUnitNode(yylineno,"STAR",TOKEN_TYPE_OTHER,yytext);  return STAR;}
{DIV} {yylval.nodePtr=getLexicalUnitNode(yylineno,"DIV",TOKEN_TYPE_OTHER,yytext);  return DIV;}
{AND} {yylval.nodePtr=getLexicalUnitNode(yylineno,"AND",TOKEN_TYPE_OTHER,yytext);  return AND;}
{OR} {yylval.nodePtr=getLexicalUnitNode(yylineno,"OR",TOKEN_TYPE_OTHER,yytext);  return OR; }
{NOT} {yylval.nodePtr=getLexicalUnitNode(yylineno,"NOT",TOKEN_TYPE_OTHER,yytext);  return NOT;}
{WS} {}
{RELOP} { yylval.nodePtr=getLexicalUnitNode(yylineno,"RELOP",TOKEN_TYPE_OTHER,yytext);  return RELOP;}
{ASSIGNOP} {yylval.nodePtr=getLexicalUnitNode(yylineno,"ASSIGNOP",TOKEN_TYPE_OTHER,yytext);  return ASSIGNOP;}
{SEMI} {yylval.nodePtr=getLexicalUnitNode(yylineno,"SEMI",TOKEN_TYPE_OTHER,yytext);  return SEMI;}
{NEWLINE} {yycolumn = 1;}
{COMMA} {yylval.nodePtr=getLexicalUnitNode(yylineno,"COMMA",TOKEN_TYPE_OTHER,yytext);  return COMMA;}
{DOT} {yylval.nodePtr=getLexicalUnitNode(yylineno,"DOT",TOKEN_TYPE_OTHER,yytext);  return DOT;}
{LB} {yylval.nodePtr=getLexicalUnitNode(yylineno,"LB",TOKEN_TYPE_OTHER,yytext);  return LB;}
{RB} {yylval.nodePtr=getLexicalUnitNode(yylineno,"RB",TOKEN_TYPE_OTHER,yytext);  return RB;}
{LP} {yylval.nodePtr=getLexicalUnitNode(yylineno,"LP",TOKEN_TYPE_OTHER,yytext);  return LP;}
{RP} {yylval.nodePtr=getLexicalUnitNode(yylineno,"RP",TOKEN_TYPE_OTHER,yytext);  return RP;}
{LC} {yylval.nodePtr=getLexicalUnitNode(yylineno,"LC",TOKEN_TYPE_OTHER,yytext);  return LC;}
{RC} {yylval.nodePtr=getLexicalUnitNode(yylineno,"RC",TOKEN_TYPE_OTHER,yytext);  return RC;}
. {printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno, yytext);}
%%