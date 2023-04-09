#include "syntaxTree.h"

bool hasError = false;
extern int yyparse();
extern void yyrestart(FILE*);
extern int yydebug;
extern void analyseTree();

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        yyparse();
        return 1;
    }
    FILE* f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    if (!hasError)
    {
        printTree();
        analyseTree();
    }
    freeMemory();
    return 0;
}