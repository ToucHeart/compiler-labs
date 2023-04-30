#include "syntaxTree.h"

bool hasError = false;
extern int yyparse();
extern void yyrestart(FILE*);
extern int yydebug;
extern void semanticAnalyse();

int main(int argc, char* argv[])
{
    setbuf(stdout, NULL);
    if (argc != 3)
    {
        fprintf(stderr, "pass 3 arguments\n");
        return 1;
    }
    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w+");
    if (!input)
    {
        perror(argv[1]);
        return 1;
    }
    if (!output)
    {
        perror(argv[2]);
        return 1;
    }
    yyrestart(input);
    yyparse();
    if (!hasError)
    {
        // printTree();
        semanticAnalyse();
    }
    freeMemory();
    return 0;
}