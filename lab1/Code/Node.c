#include "Node.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
/*
    int lineNum;
    char *name;    // int i;name ==i ; ID:func
    NodeType type; // int,float,id
    union nodeVal
    {
        int int_val;
        float float_val;
        char *str;
    } val;
    struct Node *child; // 左子女
    struct Node *sibling; // 右兄弟

    typedef enum nodeType
    {
        TOKEN_TYPE_INT,
        TOKEN_TYPE_FLOAT,
        TOKEN_TYPE_ID,
        TOKEN_TYPE_OTHER,
        NOT_TOKEN,
    } NodeType;
*/
Node *root = NULL;

Node *getSyntaxUnitNode(int num, char *name, NodeType t, int argc, ...)
{
#if 0
    printf("syntax unit begin %s\n", name);
#endif
    Node *p = (Node *)malloc(sizeof(Node));
    p->lineNum = num;
    char *buf = (char *)malloc((strlen(name) + 1) * sizeof(char));
    memcpy(buf, name, strlen(name) + 1);
    p->unitName = buf;
    p->type = t;
    p->sibling = p->child = NULL;
    va_list arglist;
    va_start(arglist, argc);
    while (p->child == NULL)
    {
        p->child = va_arg(arglist, Node *);
    }
    Node *q = p->child;
    if (q != NULL)
    {
        for (int i = 1; i < argc;)
        {
            Node *temp = va_arg(arglist, Node *);
            ++i;
            while (temp == NULL && i < argc)
            {
                temp = va_arg(arglist, Node *);
                ++i;
            }
            q->sibling = temp;
            q = q->sibling;
        }
    }
    va_end(arglist);
#if 0
    printf("syntax unit end\n");
#endif
    return p;
}

Node *getLexicalUnitNode(int num, char *name, NodeType t, char *val)
{
#if 0
    printf("lexical unit begin\n");
#endif
    Node *p = (Node *)malloc(sizeof(Node));
    p->lineNum = num;
    char *buf = (char *)malloc((strlen(name) + 1) * sizeof(char));
    memcpy(buf, name, strlen(name) + 1);
    p->unitName = buf;
    p->type = t;
    p->sibling = p->child = NULL;
    switch (t)
    {
    case TOKEN_TYPE_INT:
        p->val.int_val = atoi(val);
        break;
    case TOKEN_TYPE_FLOAT:
        p->val.float_val = atof(val);
        break;
    case TOKEN_TYPE_ID:
    case TOKEN_TYPE_TYPE:
    {
        char *temp = (char *)malloc((strlen(val) + 1) * sizeof(char));
        memcpy(temp, val, strlen(val) + 1);
        p->val.str = temp;
    }
    break;
    case TOKEN_TYPE_OTHER:
        // do nothing
        break;
    default:
        assert(0);
        break;
    }
#if 0
    printf("lexical unit end\n");
#endif
    return p;
}
void printTree()
{
    printNode(root, 0);
}
void printNode(Node *subroot, int depth)
{
    if (subroot == NULL)
        return;
    for (int i = 0; i < depth; i++)
    {
        printf("  ");
    }
    switch (subroot->type)
    {
    case NOT_TOKEN:
        if (subroot->child != NULL)
            printf("%s (%d)\n", subroot->unitName, subroot->lineNum);
        break;
    case TOKEN_TYPE_INT:
        printf("%s: %d\n", subroot->unitName, subroot->val.int_val);
        break;
    case TOKEN_TYPE_FLOAT:
        printf("%s: %f\n", subroot->unitName, subroot->val.float_val);
        break;
    case TOKEN_TYPE_ID:
    case TOKEN_TYPE_TYPE:
        printf("%s: %s\n", subroot->unitName, subroot->val.str);
        break;
    case TOKEN_TYPE_OTHER:
        printf("%s\n", subroot->unitName);
        break;
    default:
        assert(0);
        break;
    }
    Node *p = subroot->child;
    while (p != NULL)
    {
        printNode(p, depth + 1);
        p = p->sibling;
    }
}
void freeNode(Node *subroot)
{
    Node *p = subroot->child;
    while (p != NULL)
    {
        Node *temp = p;
        p = p->sibling;
        freeNode(temp);
    }
    free(subroot->unitName);
    if (subroot->type == TOKEN_TYPE_ID || subroot->type == TOKEN_TYPE_TYPE)
        free(subroot->val.str);
    free(subroot);
}

void freeMemory()
{
    if (root != NULL)
        freeNode(root);
}