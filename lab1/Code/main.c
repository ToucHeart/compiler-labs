#include <stdio.h>
extern int yylex(void);
extern void yyrestart(FILE *input_file);

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        yylex();
        return 0;
    }
    for (int i = 1; i < argc; i++)
    {
        FILE *f = fopen(argv[i], "r");
        if (!f)
        {
            perror(argv[i]);
            return 1;
        }
        yyrestart(f);
        yylex();
        fclose(f);
    }
    return 0;
}