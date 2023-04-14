#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

char* mystrdup(const char* str)
{
    char* p = (char*)malloc(strlen(str) + 1);
    strcpy(p, str);
    return p;
}

Type* newType()
{
    Type* t = (Type*)malloc(sizeof(Type));
    memset(t, 0x0, sizeof(Type));
    t->isRval = false;
    t->kind = NONE;
    return t;
}

Symbol* newSymbol(char* name)
{
    Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
    if (name != NULL)
        sym->name = mystrdup(name);
    sym->type = newType();
    sym->next = NULL;
    memset(sym->type, 0, sizeof(Type));
    return sym;
}

bool strEqual(const char* str1, const char* str2)
{
    return strcmp(str1, str2) == 0;
}

static void printSemanticError(int errType, int lineNum, char* message, int num, ...)
{
    printf("Error type %d at Line %d: %s", errType, lineNum, message);
    va_list valist;
    va_start(valist, num);
    for (int i = 0; i < num; i++)
    {
        char* str = va_arg(valist, char*);
        printf("%s", str);
    }
    printf("\n");
    va_end(valist);
}

static unsigned int hash(char* name)
{
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~HASH_TABLE_SIZE)
            val = (val ^ (i >> 12)) & HASH_TABLE_SIZE;
    }
    return val;
}

// global symbol table
Symbol* SymbolTable[HASH_TABLE_SIZE + 1];

bool searchTableItem(char* name, Kind k)
{
    unsigned key = hash(name);
    if (SymbolTable[key] == NULL)
    {
        return false;
    }
    else
    {
        Symbol* p = SymbolTable[key];
        while (p != NULL)
        {
            if (k == CANNOT_DUP && p->type->kind != FUNCTION || k == p->type->kind)
            {
                if (strEqual(p->name, name))
                {
                    return true;
                }
            }
            p = p->next;
        }
    }
    return false;
}

bool serarchStructItem(char* name, Symbol* structHead)
{
    Symbol* head = structHead;
    while (head != NULL)
    {
        if (strEqual(name, head->name))
            return true;
        head = head->next;
    }
    return false;
}

void insertStructItem(Symbol* structinfo, Symbol* s)
{
    Symbol* p = structinfo->type->t.structure;
    if (p == NULL)
    {
        structinfo->type->t.structure = s;
    }
    else
    {
        while (p->next != NULL)
        {
            p = p->next;
        }
        p->next = s;
        s->next = NULL;
    }
}

void insertTableItem(Symbol* s)
{
    unsigned key = hash(s->name);
    if (SymbolTable[key] == NULL)
    {
        SymbolTable[key] = s;
    }
    else
    {
        s->next = SymbolTable[key];
        SymbolTable[key] = s;
    }
}

Symbol* getTableSymbol(char* name, Kind k)
{
    unsigned key = hash(name);
    if (SymbolTable[key] == NULL)
    {
        return NULL;
    }
    else
    {
        Symbol* p = SymbolTable[key];
        while (p != NULL)
        {
            if (strEqual(p->name, name))
            {
                if (k == CANNOT_DUP && p->type->kind != FUNCTION || k == p->type->kind)
                    return p;
            }
            p = p->next;
        }
    }
    return NULL;
}

Symbol* getStructItem(char* name, Symbol* structHead)
{
    Symbol* head = structHead;
    while (head != NULL)
    {
        if (strEqual(name, head->name))
            return head;
        head = head->next;
    }
    return NULL;
}

/*
StructSpecifier → STRUCT OptTag LC DefList RC 定义
                | STRUCT Tag                  使用
OptTag → ID | 空
Tag → ID
*/
Symbol* StructSpecifier(Node* node)//只返回一个type的structure部分
{
    Node* first = node->child, * second = first->sibling;
    if (strEqual(second->unitName, "Tag"))//use struct defined
    {
        Symbol* structdef = getTableSymbol(second->child->val.str, STRUCTURE);
        if (structdef == NULL)
        {
            printSemanticError(17, second->lineNum, "Undefined structure \"", 2, second->child->val.str, "\".");
        }
        else
        {
            return structdef->type->t.structure;
        }
    }
    else if (strEqual(second->unitName, "OptTag"))//has opttag specified
    {
        Node* structid = second->child;
        bool has = false;
        if (has = searchTableItem(structid->val.str, CANNOT_DUP))
        {
            printSemanticError(16, structid->lineNum, "Duplicated name \"", 2, structid->val.str, "\".");
        }
        Symbol* sym = newSymbol(structid->val.str);
        if (!has)
        {
            insertTableItem(sym);
        }
        sym->type->kind = STRUCTURE;
        if (strEqual(second->sibling->sibling->unitName, "DefList"))
            DefList(second->sibling->sibling, sym);   //
        return sym->type->t.structure;
    }
    else if (strEqual(second->unitName, "LC"))//unnamed struct
    {
        Symbol* sym = newSymbol(NULL);
        sym->type->kind = STRUCTURE;
        DefList(second->sibling, sym);   //
        Symbol* f = sym->type->t.structure;
        free(sym);
        return f;
    }
    else
    {
        assert(false);
    }
    return NULL;
}

// specifier
Type* Specifier(Node* n)
{
    Type* ret = newType();
    Node* node = n->child;
    if (strEqual(node->unitName, "TYPE"))
    {
        if (strEqual(node->val.str, "int"))
        {
            ret->kind = BASIC;
            ret->t.basicType = INT;
        }
        else if (strEqual(node->val.str, "float"))
        {
            ret->kind = BASIC;
            ret->t.basicType = FLOAT;
        }
    }
    else if (strEqual(node->unitName, "StructSpecifier"))
    {
        ret->kind = STRUCTURE;
        ret->t.structure = StructSpecifier(node);
    }
    else
    {
        assert(false);
    }
    return ret;
}
// VarDec → ID | VarDec LB INT RB
Symbol* Array(Node* node, Type* t, Symbol* structinfo)
{
    if (strEqual(node->unitName, "ID"))
    {
        char* name = node->val.str;
        if (structinfo == NULL)
        {
            if (searchTableItem(name, CANNOT_DUP))
            {
                printSemanticError(3, node->lineNum, "Redefined variable \"", 2, name, "\".");
            }
            else
            {
                Symbol* s = newSymbol(name);
                s->type = t;
                insertTableItem(s);
                return s;
            }
        }
        else
        {
            if (serarchStructItem(name, structinfo->type->t.structure))
            {
                printSemanticError(15, node->lineNum, "Redefined field \"", 2, node->val.str, "\".");
            }
            else
            {
                Symbol* s = newSymbol(node->val.str);
                s->type = t;
                insertStructItem(structinfo, s);
            }
        }
    }
    else if (strEqual(node->unitName, "VarDec"))
    {
        Type* temp = newType();
        temp->kind = ARRAY;
        temp->t.array.elem = t;
        temp->t.array.size = node->sibling->sibling->val.int_val;
        return Array(node->child, temp, structinfo);
    }
    else
    {
        assert(false);
    }
    return NULL;
}
// VarDec → ID | VarDec LB INT RB
Symbol* VarDec(Node* n, Type* t, Symbol* structinfo)
{
    Node* node = n->child;
    if (strEqual(node->unitName, "ID")) // simple variable
    {
        char* name = node->val.str;
        if (structinfo == NULL)
        {
            if (searchTableItem(name, CANNOT_DUP))
            {
                printSemanticError(3, node->lineNum, "Redefined variable \"", 2, name, "\".");
            }
            else
            {
                Symbol* s = newSymbol(node->val.str);
                s->type = t;
                insertTableItem(s);
                return s;
            }
        }
        else
        {
            if (serarchStructItem(name, structinfo->type->t.structure))
            {
                printSemanticError(15, node->lineNum, "Redefined field \"", 2, node->val.str, "\".");
            }
            else
            {
                Symbol* s = newSymbol(node->val.str);
                s->type = t;
                insertStructItem(structinfo, s);
            }
        }
    }
    else if (strEqual(node->unitName, "VarDec")) // arrary
    {
        return Array(node, t, structinfo);
    }
    else
    {
        assert(false);
    }
    return NULL;
}

// int global1, global2;
void ExtDecList(Node* subtree, Type* t) // subtree is firstchild of ExtDecList,== vardec
{
    VarDec(subtree->child, t, NULL);
    if (subtree->child->sibling != NULL)
    {
        Node* nextid = subtree->child->sibling->sibling; // nextid== ExtDecList
        ExtDecList(nextid, t);
    }
}
bool checkArrayEqual(Type* lhs, Type* rhs)
{
    if (lhs->kind != rhs->kind)
        return false;
    else if (lhs->kind == BASIC && lhs->t.basicType != rhs->t.basicType)
        return false;
    else if (lhs->kind == ARRAY)
    {
        return checkArrayEqual(lhs->t.array.elem, rhs->t.array.elem);
    }
    else if (lhs->kind == STRUCTURE)
    {
        return checkStructEqual(lhs->t.structure, rhs->t.structure);
    }
    return true;
}
bool checkStructEqual(Symbol* lhs, Symbol* rhs)
{
    bool result = true;
    if (lhs->type->kind != rhs->type->kind)
        return false;
    else if (lhs->type->kind == BASIC && lhs->type->t.basicType != rhs->type->t.basicType)
        return false;
    else if (lhs->type->kind == ARRAY)
        result = result && checkArrayEqual(lhs->type, rhs->type);
    else if (lhs->type->kind == STRUCTURE)
        result = result && checkStructEqual(lhs->type->t.structure, rhs->type->t.structure);
    if (lhs->next && rhs->next)
        result = result && checkStructEqual(lhs->next, rhs->next);
    else if (lhs->next == NULL && rhs->next != NULL)
        result = false;
    else if (lhs->next != NULL && rhs->next == NULL)
        result = false;
    return result;
}

void Args(Node* node, Parameter* cur)// Args → Exp COMMA Args | Exp
{
    if (cur == NULL)//参数给多了
    {
        printSemanticError(9, node->lineNum, "Too much arguments.", 0);
        return;
    }
    Node* first = node->child, * second = first->sibling;
    Type* expType = Exp(first);
    if (expType->kind != cur->t->kind)//参数类型不匹配
    {
        // printf("%d\t%d\n", expType->kind, cur->t->kind);
        printSemanticError(9, first->lineNum, "Function arguments do not match.", 0);
    }
    else
    {
        if (expType->kind == BASIC && expType->t.basicType != cur->t->t.basicType)//参数类型不匹配
        {
            // printf("%d\t%d\n", expType->t.basicType, cur->t->t.basicType);
            printSemanticError(9, first->lineNum, "Function arguments do not match.", 0);
        }
        else if (expType->kind == ARRAY)
        {
            if (!checkArrayEqual(expType, cur->t))
            {
                printSemanticError(9, first->lineNum, "Function arguments do not match,array type mismatch.", 0);
            }
        }
        else if (expType->kind == STRUCTURE)
        {
            if (!checkStructEqual(expType->t.structure, cur->t->t.structure))
            {
                printSemanticError(9, first->lineNum, "Function arguments do not match,struct type mismatch.", 0);
            }
        }
    }
    if (second != NULL)
    {
        Node* third = second->sibling;
        Args(third, cur->next);
    }
    else if (cur->next != NULL)//参数给少了
    {
        printSemanticError(9, node->lineNum, "Too few arguments.", 0);
        return;
    }
}

Type* Exp(Node* node)
{
    assert(strEqual(node->unitName, "Exp"));
    Type* t = newType();
    Node* first = node->child, * second = first->sibling;
    /*
        | ID                a
        | INT               1
        | FLOAT             2.1
    */
    if (strEqual(first->unitName, "FLOAT"))
    {
        t->kind = BASIC;
        t->t.basicType = FLOAT;
        t->isRval = true;
    }
    else if (strEqual(first->unitName, "INT"))
    {
        t->kind = BASIC;
        t->t.basicType = INT;
        t->isRval = true;
    }
    else if (second == NULL && strEqual(first->unitName, "ID"))
    {
        Symbol* s = getTableSymbol(first->val.str, CANNOT_DUP);
        if (s == NULL)
        {
            printSemanticError(1, first->lineNum, "Undefined variable \"", 2, first->val.str, "\".");
        }
        else
        {
            free(t);
            t = s->type;
        }
    }
    else if (strEqual(first->unitName, "LP"))// LP Exp RP   
    {
        return Exp(second);
    }
    else if (strEqual(first->unitName, "NOT"))// NOT Exp    
    {
        Type* temp = Exp(second);
        if (temp->kind != BASIC || temp->t.basicType != INT)
        {
            printSemanticError(7, second->lineNum, "Type mismatched for operands.", 0);
        }
        else
        {
            t->kind = BASIC;
            t->t.basicType = INT;
            t->isRval = true;
        }
    }
    else if (strEqual(first->unitName, "MINUS"))// MINUS Exp         -a
    {
        Type* temp = Exp(second);
        if (temp->kind != BASIC)
        {
            printSemanticError(7, second->lineNum, "Type mismatched for operands.", 0);
        }
        t->kind = temp->kind;
        t->isRval = true;
        t->t.basicType = temp->t.basicType;
    }
    else if (second != NULL)
    {
        char* op = second->unitName;
        if (strEqual(op, "ASSIGNOP"))//Exp ASSIGNOP Exp  e1 = e2 
        {
            Type* lhs = Exp(first);
            Node* third = second->sibling;
            Type* rhs = Exp(third);
            if (lhs->kind != rhs->kind || lhs->kind == BASIC && lhs->t.basicType != rhs->t.basicType)
            {
                printSemanticError(5, second->lineNum, "Type mismatched for assignment.", 0);
            }
            else if (lhs->isRval)
            {
                printSemanticError(6, first->lineNum, "Assign to rvalue.", 0);
            }
            else if (lhs->kind == STRUCTURE)
            {
                if (!checkStructEqual(lhs->t.structure, rhs->t.structure))
                {
                    printSemanticError(5, second->lineNum, "Type mismatched for assignment.", 0);
                }
            }
            t = rhs;
        }
        //左右类型必须相同且只能为int 或float
        /*
            | Exp PLUS Exp      a+b
            | Exp MINUS Exp     a-b
            | Exp STAR Exp      a*b
            | Exp DIV Exp       a/b
        */
        else if (strEqual(op, "PLUS") || strEqual(op, "MINUS") || strEqual(op, "STAR") || strEqual(op, "DIV"))
        {
            Type* lhs = Exp(first);
            Node* third = second->sibling;
            Type* rhs = Exp(third);
            if (lhs->kind != BASIC || rhs->kind != BASIC || lhs->t.basicType != rhs->t.basicType)
            {
                printSemanticError(7, second->lineNum, "Type mismatched for operands.", 0);
            }
            else
            {
                t->kind = BASIC;
                t->t.basicType = lhs->t.basicType;
                t->isRval = true;
            }
        }
        /*
            | Exp AND Exp       e1 && e2
            | Exp OR Exp        e || e2
        */
        else if (strEqual(op, "AND") || strEqual(op, "OR"))
        {
            Type* lhs = Exp(first);
            Node* third = second->sibling;
            Type* rhs = Exp(third);
            if (lhs->t.basicType != INT || rhs->t.basicType != INT)
            {
                printSemanticError(7, second->lineNum, "Type mismatched for operands.", 0);
            }
            else
            {
                t->kind = BASIC;
                t->t.basicType = INT;
                t->isRval = true;
            }
        }
        else if (strEqual(op, "DOT"))//| Exp DOT ID
        {
            Type* lhs = Exp(first);
            char* fieldname = second->sibling->val.str;
            if (lhs->kind != STRUCTURE)
            {
                printSemanticError(13, first->lineNum, "Illegal use of \".\"", 1, " Expecting a struct varible.");
            }
            else if (!serarchStructItem(fieldname, lhs->t.structure))
            {
                printSemanticError(14, second->sibling->lineNum, "Non-existent field \"", 2, fieldname, "\".");
            }
            else
            {
                Symbol* temp = getStructItem(fieldname, lhs->t.structure);
                free(t);
                t = temp->type;
            }
        }
        else if (strEqual(op, "RELOP"))// | Exp RELOP Exp     a>b
        {
            Type* lhs = Exp(first);
            Type* rhs = Exp(second->sibling);
            if (lhs->kind != BASIC || rhs->kind != BASIC || lhs->t.basicType != rhs->t.basicType)
            {
                printSemanticError(7, second->lineNum, "Type mismatched for operands.", 0);
            }
            else
            {
                t->kind = BASIC;
                t->t.basicType = INT;
                t->isRval = true;
            }
        }
        else if (strEqual(op, "LB"))// Exp LB Exp RB     a[1]
        {
            Node* third = second->sibling;
            Type* temp = Exp(first);
            if (temp->kind == ARRAY)
            {
                t->kind = temp->t.array.elem->kind;
                t->t = temp->t.array.elem->t;
            }
            else if (temp->kind != ARRAY)
            {
                printSemanticError(10, first->lineNum, "Expecting an array before \'[\'.", 0);
            }
            Type* a = Exp(third);
            if (a->t.basicType != INT)
            {
                printSemanticError(12, third->lineNum, "Expecting an integer in [].", 0);
            }
        }
        /*
            | ID LP Args RP     f(x,y)
            | ID LP RP          f()
        */
        else if (strEqual(op, "LP"))
        {
            Symbol* s = getTableSymbol(first->val.str, FUNCTION);
            if (s == NULL)
            {
                s = getTableSymbol(first->val.str, CANNOT_DUP);
                if (s != NULL)
                {
                    printSemanticError(11, first->lineNum, first->val.str, 1, " is not a function.");
                }
                else
                {
                    printSemanticError(2, first->lineNum, "Undefined function \"", 2, first->val.str, "\".");
                }
            }
            else
            {
                free(t);
                t = s->type->t.function.returnType;
                if (t->kind == BASIC)
                {
                    t->isRval = true;
                }
                Node* third = second->sibling;
                if (strEqual(third->unitName, "Args"))//
                {
                    Args(third, s->type->t.function.params);
                }
                else
                {
                    if (s->type->t.function.params != NULL)
                    {
                        printSemanticError(9, third->lineNum, "Too few arguments.", 0);
                    }
                }
            }
        }
    }
    return t;
}
/*
Stmt →  | Exp SEMI
        | CompSt
        | RETURN Exp SEMI
        | IF LP Exp RP Stmt
        | IF LP Exp RP Stmt ELSE Stmt
        | WHILE LP Exp RP Stmt
*/
void Stmt(Node* node, Symbol* funcSym)
{
    assert(strEqual(node->unitName, "Stmt"));
    Node* first = node->child, * second = first->sibling;
    if (strEqual(first->unitName, "Exp"))
    {
        Exp(first);
    }
    else if (strEqual(first->unitName, "CompSt"))
    {
        CompSt(first, funcSym);
    }
    else if (strEqual(first->unitName, "RETURN"))
    {
        Type* retType = Exp(second);
        Type* funcRetType = funcSym->type->t.function.returnType;
        if (retType->kind != funcRetType->kind)
        {
            printSemanticError(8, first->lineNum, "Type mismatched for return.", 0);
        }
        else if (retType->kind == BASIC && retType->t.basicType != funcRetType->t.basicType)
        {
            printSemanticError(8, first->lineNum, "Type mismatched for return.", 0);
        }
    }
    else if (strEqual(first->unitName, "WHILE") || strEqual(first->unitName, "IF"))
    {
        Node* third = second->sibling;
        Type* t = Exp(third);
        if (t->kind != BASIC || t->t.basicType != INT)
        {
            printSemanticError(7, third->lineNum, "Type mismatched for operands,expecting INT.", 0);
        }
        Node* five = third->sibling->sibling;
        Stmt(five, funcSym);
        if (five->sibling)
        {
            Stmt(five->sibling->sibling, funcSym);
        }
    }
}

// StmtList → Stmt StmtList | 空
void StmtList(Node* node, Symbol* funcSym)
{
    assert(strEqual(node->unitName, "StmtList"));
    Node* first = node->child;
    Stmt(first, funcSym);
    if (first->sibling != NULL)
    {
        StmtList(first->sibling, funcSym);
    }
}

// Dec → VarDec | VarDec ASSIGNOP Exp
void Dec(Node* node, Type* type, Symbol* structinfo)
{
    Node* first = node->child;
    Symbol* s = VarDec(first, type, structinfo);//暂存符号用于后面的check assignment,structinfo!=NULL则不会用到
    if (first->sibling != NULL)                 //a=1这种
    {
        if (structinfo == NULL)          //in a function
        {
            Type* rhstype = Exp(first->sibling->sibling);//check Assignment legal
            if (rhstype != NULL && s != NULL)
            {
                if (s->type->kind != rhstype->kind || s->type->t.basicType != rhstype->t.basicType)
                {
                    printSemanticError(5, first->lineNum, "Type mismatched for assignment.", 0);
                    free(rhstype);
                }
            }
        }
        else                             //in a structure,assignments are not allowed
        {
            printSemanticError(15, first->lineNum, "Initialized field \"", 2, first->child->val.str, "\".");
        }
    }
}
// DecList → Dec | Dec COMMA DecList
void DecList(Node* node, Type* type, Symbol* structinfo)
{
    assert(strEqual(node->unitName, "DecList"));
    Node* first = node->child;
    Dec(first, type, structinfo);
    if (first->sibling != NULL)
    {
        DecList(first->sibling->sibling, type, structinfo);
    }
}
// 每个Def就是一条变量定义，它包括一个类型描述符Specifier以及一个DecList
// Def → Specifier DecList SEMI
void Def(Node* node, Symbol* structinfo)
{
    assert(strEqual(node->unitName, "Def"));
    Node* first = node->child, * second = first->sibling;
    Type* temp = Specifier(first);
    DecList(second, temp, structinfo);
}
// DefList → Def DefList | 空
void DefList(Node* node, Symbol* structinfo)
{
    assert(strEqual(node->unitName, "DefList"));
    Node* def = node->child;
    Def(def, structinfo);
    if (def->sibling != NULL)
    {
        DefList(def->sibling, structinfo);
    }
}

//CompSt → LC DefList StmtList RC
void CompSt(Node* node, Symbol* funcSym)
{
    Node* secondone = node->child->sibling;
    Node* thirdone = NULL;
    if (strEqual(secondone->unitName, "DefList"))
    {
        DefList(secondone, NULL);
        thirdone = secondone->sibling;
    }
    else if (strEqual(secondone->unitName, "StmtList"))
    {
        thirdone = secondone;
    }
    if (thirdone != NULL && strEqual(thirdone->unitName, "StmtList"))
    {
        StmtList(thirdone, funcSym);
    }
}

//ParamDec → Specifier VarDec
void ParamDec(Node* node, Symbol* s)
{
    Type* temp = Specifier(node->child);
    Symbol* paraSymbol = VarDec(node->child->sibling, temp, NULL);
    if (paraSymbol == NULL)
        return;
    if (s->type->t.function.params == NULL)
    {
        s->type->t.function.params = newParameter();
    }
    Parameter* p = s->type->t.function.params;
    if (p->t == NULL)
    {
        p->t = paraSymbol->type;
    }
    else
    {
        while (p->next != NULL)
        {
            p = p->next;
        }
        Parameter* pp = newParameter();
        pp->t = paraSymbol->type;
        p->next = pp;
    }
}
//VarList → ParamDec COMMA VarList
//        | ParamDec
void VarList(Node* node, Symbol* s)
{
    ParamDec(node->child, s);
    if (node->child->sibling != NULL)
    {
        VarList(node->child->sibling->sibling, s);
    }
}
// Specifier FunDec CompSt
//FunDec → ID LP VarList RP 
//       | ID LP RP
void FunDec(Node* node, Type* retType)
{
    Node* idnode = node->child;
    char* name = idnode->val.str;
    bool has = false;
    if (has = searchTableItem(name, FUNCTION))
    {
        printSemanticError(4, node->lineNum, "Redefined function \"", 2, name, "\".");
    }
    Symbol* s = newSymbol(name);
    if (!has)
    {
        insertTableItem(s);
    }
    s->type->kind = FUNCTION;
    s->type->t.function.returnType = retType;
    Node* thirdone = idnode->sibling->sibling;
    if (strEqual(thirdone->unitName, "RP"))
    {
        s->type->t.function.argc = 0;
        s->type->t.function.params = NULL;
    }
    else if (strEqual(thirdone->unitName, "VarList"))
    {
        VarList(thirdone, s);
    }
    CompSt(node->sibling, s);
}
Parameter* newParameter()
{
    Parameter* p = (Parameter*)malloc(sizeof(Parameter));
    p->next = NULL;
    p->t = NULL;
    return p;
}

// 每个ExtDef表示一个全局变量、结构体或函数的定义
// ExtDef → Specifier ExtDecList SEMI
// ExtDef → Specifier SEMI
// ExtDef → Specifier FunDec CompSt
void ExtDef(Node* extdef)
{
    Node* firstchild = extdef->child, * secondchild = firstchild->sibling;
    Type* t = Specifier(firstchild);
    if (strEqual(secondchild->unitName, "FunDec")) // function definition
    {
        FunDec(secondchild, t);
    }
    else if (strEqual(secondchild->unitName, "ExtDecList")) // variable definition,including array
    {
        ExtDecList(secondchild, t);
    }
    else if (!strEqual(secondchild->unitName, "SEMI")) // not struct definition
    {
        assert(0);
    }
}

void ExtDefList(Node* subtree) // subtree unitName == "ExtDefList"
{
    for (Node* q = subtree->child; q != NULL; q = q->sibling)
    {
        if (strEqual(q->unitName, "ExtDef"))
        {
            ExtDef(q);
        }
        else if (strEqual(q->unitName, "ExtDefList"))
        {
            ExtDefList(q);
        }
    }
}
extern Node* root;
void semanticAnalyse()
{
    if (root != NULL)
        ExtDefList(root->child);
}