#ifndef __HELP_H__
#define __HELP_H__
#include <stdbool.h>

typedef unsigned long long ULL;
#define int2cptr(x) ((char*)(ULL)x)
#define RED "\033[1;31m"
#define NORMAL "\033[0m"
bool strEqual(const char* str1, const char* str2);
char* mystrdup(const char* str);
int min(int a, int b);
#endif