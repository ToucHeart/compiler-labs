/*
 * this file generates intermediate Code
 */

#include"interCodes.h"
#include"help.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"syntaxTree.h"
#include<stdarg.h>

#define DEBUG 1

extern Node* root;
static InterCodesList list;
extern char* mystrdup(const char* str);

OperandPtr newOperand(int kind, int val, char* name)
{
    OperandPtr op = (OperandPtr)malloc(sizeof(Operand));
    op->kind = kind;
    op->u.value = val;
    op->u.name = name;
    return op;
}

InterCodePtr newInterCode(int kind, ...)
{
    InterCodePtr p = (InterCodePtr)malloc(sizeof(InterCode));
    p->kind = kind;
    va_list ap;
    va_start(ap, kind);
    switch (kind)
    {
    case IR_FUNCTION:
    case IR_PARAM:
    p->u.oneop.op = va_arg(ap, OperandPtr);
    break;
    case IR_ASSIGN:
    p->u.assign.left = va_arg(ap, OperandPtr);
    p->u.assign.right = va_arg(ap, OperandPtr);
    break;
    default:
    assert(0);
    break;
    }
    return p;
}

InterCodesPtr newInterCodes(InterCodePtr code)
{
    InterCodesPtr p = (InterCodesPtr)malloc(sizeof(InterCodes));
    p->code = code;
    p->next = p->prev = NULL;
    return p;
}

void initList()
{
    list.dummyHead = newInterCodes(NULL);
    list.dummyHead->next = list.dummyHead;
    list.dummyHead->prev = list.dummyHead;
    list.labelIndex = 1;
    list.tmpIndex = 1;
    list.varIndex = 1;
}

void addInterCodes(InterCodesPtr p)
{
    InterCodesPtr head = list.dummyHead;
    if (head->next == head)
    {
        head->next = p;
        head->prev = p;
        p->next = head;
        p->prev = head;
    }
    else
    {
        head->prev->next = p;
        p->prev = head->prev;
        head->prev = p;
        p->next = head;
    }
}

//VarDec → ID | VarDec LB INT RB
void translateVarDec(Node* node, OperandPtr op)
{
    assert(strEqual(node->unitName, "VarDec"));
    Node* first = node->child;
    if (strEqual(first->unitName, "ID"))
    {
        if (op != NULL)
        {
            op->u.name = mystrdup(first->val.str);
        }
    }
    else
    {
        translateVarDec(first, op);
    }
}

//ParamDec → Specifier VarDec
void translateParamDec(Node* node)
{
    assert(strEqual(node->unitName, "ParamDec"));
    Node* second = node->child->sibling;
    OperandPtr op = newOperand(OP_VARIABLE, 0, NULL);
    translateVarDec(second, op);
    InterCodePtr code = newInterCode(IR_PARAM, op);
    InterCodesPtr codes = newInterCodes(code);
    addInterCodes(codes);
}

//VarList → ParamDec COMMA VarList
//        | ParamDec
void translateVarList(Node* node)
{
    assert(strEqual(node->unitName, "VarList"));
    translateParamDec(node->child);
    while (node->child->sibling != NULL)
    {
        node = node->child->sibling->sibling;
        translateParamDec(node->child);
    }
}

// FunDec -> ID LP VarList RP
//         | ID LP RP
void translateFunDec(Node* node)//函数名的标识符以及由一对圆括号括起来的一个形参列表
{
    assert(strEqual(node->unitName, "FunDec"));
    Node* first = node->child, * third = first->sibling->sibling;
    OperandPtr op = newOperand(OP_FUNCTION, 0, mystrdup(first->val.str));
    InterCodePtr code = newInterCode(IR_FUNCTION, op);
    InterCodesPtr codes = newInterCodes(code);
    addInterCodes(codes);
    if (strEqual(third->unitName, "VarList"))
    {
        translateVarList(third);
    }
}

void translateExp(Node* node, OperandPtr op)
{

}

// Dec → VarDec | VarDec ASSIGNOP Exp
void translateDec(Node* node)
{
    Node* first = node->child, * second = first->sibling;
    if (second == NULL)
    {
        translateVarDec(first, NULL);
    }
    else
    {
        Node* third = second->sibling;
        OperandPtr left = newOperand(OP_VARIABLE, 0, NULL);
        translateVarDec(first, left);
        OperandPtr right = newOperand(OP_NONE, 0, NULL);
        translateExp(third, right);
        InterCodePtr code = newInterCode(IR_ASSIGN, left, right);
        InterCodesPtr codes = newInterCodes(code);
    }
}

// DecList → Dec | Dec COMMA DecList
void translateDecList(Node* node)
{
    assert(strEqual(node->unitName, "DecList"));
    Node* first = node->child;
    translateDec(first);
    while (first->sibling != NULL)
    {
        Node* third = first->sibling->sibling->child;
        translateDec(third);
    }
}

// 每个Def就是一条变量定义，它包括一个类型描述符Specifier以及一个DecList
// Def → Specifier DecList SEMI
void translateDef(Node* node)
{
    assert(strEqual(node->unitName, "Def"));
    Node* second = node->child->sibling;
    translateDecList(second);
}

// DefList → Def DefList | 空
void translateDefList(Node* node)
{
    assert(strEqual(node->unitName, "DefList"));
    Node* first = node->child;
    translateDef(first);
    while (first->sibling != NULL)
    {
        first = first->sibling->child;
        translateDef(first);
    }
}

void translateStmtList(Node* node)
{
    assert(strEqual(node->unitName, "StmtList"));

}

//CompSt → LC DefList StmtList RC
void translateCompSt(Node* node)
{
    assert(strEqual(node->unitName, "CompSt"));
    Node* second = node->child->sibling, * third = second->sibling;
    if (strEqual(second->unitName, "DefList"))
    {
        translateDefList(second);
        if (strEqual(third->unitName, "StmtList"))
            translateStmtList(third);
    }
    else if (strEqual(second->unitName, "StmtList"))
    {
        translateStmtList(second);
    }
}

void translateExtDef(Node* node)
{
    //不需要处理全局变量和结构体定义,没有全局变量,只需要翻译函数
    assert(strEqual(node->unitName, "ExtDef"));
    Node* secondchild = node->child->sibling;
    if (strEqual(secondchild->unitName, "FunDec")) // function definition
    {
        translateFunDec(secondchild);
        translateCompSt(secondchild->sibling);
    }
}

void translateExtDefList(Node* node)
{
    assert(strEqual(node->unitName, "ExtDefList"));
    for (Node* q = node->child; q != NULL; q = q->sibling)
    {
        if (strEqual(q->unitName, "ExtDef"))
        {
            translateExtDef(q);
        }
        else if (strEqual(q->unitName, "ExtDefList"))
        {
            translateExtDefList(q);
        }
    }
}

void genInterCodes()
{
    initList(&list);
    if (root != NULL)
    {
        assert(strEqual(root->unitName, "Program"));
        translateExtDefList(root->child);
    }
}

void printOp(FILE* output, OperandPtr op)
{
    switch (op->kind)
    {
    case OP_CONSTANT:
    fprintf(output, "#%d", op->u.value);
    break;
    case OP_VARIABLE:
    case OP_ADDRESS:
    case OP_LABEL:
    case OP_FUNCTION:
    case OP_RELOP:
    fprintf(output, "%s", op->u.name);
    break;
    }
}

void printInterCode(FILE* output, InterCodePtr p)
{
    switch (p->kind)
    {
    case IR_FUNCTION:
    fprintf(output, "FUNCTION ");
    printOp(output, p->u.oneop.op);
    fprintf(output, " :");
    break;

    case IR_PARAM:
    fprintf(output, "PARAM ");
    printOp(output, p->u.oneop.op);
    break;

    default:
    fprintf(stderr, RED"code type is %d\n"NORMAL, p->kind);
    assert(0);
    break;
    }
    fprintf(output, "\n");
}

void printInterCodes(FILE* output)
{
    InterCodesPtr p = list.dummyHead->next;
    while (p != list.dummyHead)
    {
        printInterCode(output, p->code);
        p = p->next;
    }
}