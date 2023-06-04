#include "syntaxTree.h"

bool hasError = false;
extern int yyparse();
extern void yyrestart(FILE *);
extern int yydebug;
extern void semanticAnalyse();
extern void genInterCodes();
extern void printInterCodes(FILE *output);
extern void printAsm(FILE *output);

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);
    if (argc != 3)
    {
        fprintf(stderr, "pass 3 arguments\n");
        return 1;
    }
    FILE *input = fopen(argv[1], "r");
    FILE *asmfile = fopen(argv[2], "w+");
    FILE *irfile = fopen("./out.ir", "w+");
    if (!input)
    {
        perror(argv[1]);
        return 1;
    }
    if (!asmfile)
    {
        perror(argv[2]);
        return 1;
    }
    if (!irfile)
    {
        perror("out.ir not exists\n");
        return 1;
    }
    setbuf(asmfile, NULL);
    yyrestart(input);
    yyparse();
    if (!hasError)
    {
        // printTree();
        semanticAnalyse();
        genInterCodes();
        printInterCodes(irfile);
        printAsm(asmfile);
    }
    freeMemory();
    return 0;
}