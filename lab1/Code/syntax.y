%{
#include "lex.yy.c"
#include <stdio.h>
%}

/* declared types */
%union {
    int type_int;
    float type_float;
    double type_double;
}

/* declared tokens */
%token <type_int> INT
%token <type_float> FLOAT
%token ADD SUB MUL DIV ID AND OR WS RELOP ASSIGNOP SEMI NEWLINE COMMA DOT LB LC LP RB RC RP

/* declared non-terminals */
%type <type_double> Exp Factor Term

%%
Calc : /* empty */
| Exp { printf("= %lf\n", $1); }
;
Exp : Factor
| Exp ADD Factor { $$ = $1 + $3; }
| Exp SUB Factor { $$ = $1 - $3; }
;
Factor : Term
| Factor MUL Term { $$ = $1 * $3; }
| Factor DIV Term { $$ = $1 / $3; }
;
Term : INT { $$ = $1; }
| FLOAT { $$ = $1; }
;

%%