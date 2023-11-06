#include "syntaxTree.h"

bool hasError = false;
extern int yyparse();
extern void yyrestart(FILE*);
extern int yydebug;
extern void semanticAnalyse();
extern void genInterCodes();
extern void printInterCodes(FILE* output);
extern void printObjectCodes(FILE* output);
extern void genObjectCodes();
extern void freeVars();

void freeAll()
{
    freeMemory();
    freeVars();
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "pass 2 arguments\n");
        return 1;
    }
    FILE* input = fopen(argv[1], "r");
    FILE* ocfile = fopen(argv[2], "w+");
    if (!input)
    {
        perror(argv[1]);
        return 1;
    }
    if (!ocfile)
    {
        perror(argv[2]);
        return 1;
    }
#if SUBMIT
    // FILE* irfile = fopen("../Test/test.ir", "w+");
    if (!irfile)
    {
        perror("ir not exists\n");
        return 1;
    }
    setbuf(irfile, NULL);
#endif
    setbuf(stdout, NULL);
    setbuf(ocfile, NULL);
    yyrestart(input);
    yyparse();
    if (!hasError)
    {
        // printTree();
        semanticAnalyse();
        genInterCodes();
#if SUBMIT
        printInterCodes(irfile);
#endif
        genObjectCodes();
        printObjectCodes(ocfile);
    }
    freeAll();
    return 0;
}