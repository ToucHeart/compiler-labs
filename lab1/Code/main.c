#include "syntaxTree.h"

bool hasError = false;
extern int yyparse();
extern void yyrestart(FILE*);
extern int yydebug;
extern void semanticAnalyse();
extern void genInterCodes();
extern void printInterCodes(FILE* output);
extern void printObjectCodes(FILE* output);
extern void genObjectCode();

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "pass 3 arguments\n");
        return 1;
    }
    FILE* input = fopen(argv[1], "r");
    FILE* ocfile = fopen(argv[2], "w+");
    FILE* irfile = fopen("../Test/test.ir", "w+");
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
    if (!irfile)
    {
        perror("out.ir not exists\n");
        return 1;
    }
    setbuf(stdout, NULL);
    setbuf(irfile, NULL);
    setbuf(ocfile, NULL);
    yyrestart(input);
    yyparse();
    if (!hasError)
    {
        // printTree();
        semanticAnalyse();
        genInterCodes();
        printInterCodes(irfile);
        genObjectCode();
        printObjectCodes(ocfile);
    }
    freeMemory();
    return 0;
}