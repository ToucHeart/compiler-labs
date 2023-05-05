/*
 * helper function
*/

#include<stdlib.h>
#include<string.h>
#include"help.h"

bool strEqual(const char* str1, const char* str2)
{
    return strcmp(str1, str2) == 0;
}

char* mystrdup(const char* str)
{
    char* p = (char*)malloc(strlen(str) + 1);
    strcpy(p, str);
    return p;
}

int min(int a, int b)
{
    return a < b ? a : b;
}
