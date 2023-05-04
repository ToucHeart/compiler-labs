/*
 * this file generates intermediate Code
 */

#include"interCodes.h"
#include"help.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdarg.h>
#include"symbolTable.h"

extern Node* root;
static InterCodesList list;

void setOpKind(OperandPtr op, int kind)
{
    if (op == NULL)
        return;
    op->kind = kind;
}

void set2Arg(OperandPtr op, bool isarg)
{
    if (op == NULL)
        return;
    op->isArg = isarg;
}

void setOpVal(OperandPtr op, int value)
{
    if (op == NULL)
        return;
    op->u.value = value;
}

void setOpName(OperandPtr op, char* name)
{
    if (op == NULL)
        return;
    if (op->u.name != NULL)
        free(op->u.name);
    op->u.name = name;
}

OperandPtr newOperand(int kind, int val, char* name)
{
    OperandPtr op = (OperandPtr)malloc(sizeof(Operand));
    op->kind = kind;
    op->u.value = val;
    op->u.name = name;
    op->isArg = false;
    return op;
}

OperandPtr newLabel()
{
    char labelname[20];
    sprintf(labelname, "label%d", list.labelIndex++);
    OperandPtr label = newOperand(OP_LABEL, 0, mystrdup(labelname));
    return label;
}

OperandPtr newTemp()
{
    char tempname[10];
    sprintf(tempname, "t%d", list.tmpIndex++);
    OperandPtr temp = newOperand(OP_VARIABLE, 0, mystrdup(tempname));
    return temp;
}

ArgListPtr newArgList()
{
    ArgListPtr arglist = (ArgListPtr)malloc(sizeof(ArgList));
    arglist->head = NULL;
    return arglist;
}

ArgNodePtr newArgNode(OperandPtr op)
{
    ArgNodePtr argnode = (ArgNodePtr)malloc(sizeof(ArgNode));
    argnode->arg = op;
    argnode->next = NULL;
    return argnode;
}

void addArgNode(ArgNodePtr arg, ArgListPtr arglist)
{
    if (arglist->head == NULL)
    {
        arglist->head = arg;
    }
    else
    {
        arg->next = arglist->head;
        arglist->head = arg;
    }
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
    case IR_RETURN:
    case IR_READ:
    case IR_WRITE:
    case IR_ARG:
    p->u.oneop.op = va_arg(ap, OperandPtr);
    break;

    case IR_ASSIGN:
    case IR_READ_ADDR:
    case IR_WRITE_ADDR:
    case IR_READ_WRITE_ADDR:
    case IR_GET_ADDR:
    p->u.assign.left = va_arg(ap, OperandPtr);
    p->u.assign.right = va_arg(ap, OperandPtr);
    break;

    case IR_DEC:
    p->u.dec.varName = va_arg(ap, OperandPtr);
    p->u.dec.size = va_arg(ap, int);
    break;

    case IR_ADD:
    case IR_SUB:
    case IR_MUL:
    case IR_DIV:
    p->u.binop.result = va_arg(ap, OperandPtr);
    p->u.binop.left = va_arg(ap, OperandPtr);
    p->u.binop.right = va_arg(ap, OperandPtr);
    break;

    case IR_IF_GOTO:
    p->u.ifgoto.left = va_arg(ap, OperandPtr);
    p->u.ifgoto.op = va_arg(ap, OperandPtr);
    p->u.ifgoto.right = va_arg(ap, OperandPtr);
    p->u.ifgoto.result = va_arg(ap, OperandPtr);
    break;

    case IR_GOTO:
    p->u.oneop.op = va_arg(ap, OperandPtr);
    break;
    case IR_LABEL:
    p->u.oneop.op = va_arg(ap, OperandPtr);
    break;

    case IR_CALL:
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
void translateVarDec(Node* node, OperandPtr op, bool isParam)
{
    assert(strEqual(node->unitName, "VarDec"));
    Node* first = node->child;
    if (strEqual(first->unitName, "ID"))
    {
        //根据类型进行判断
        Symbol* sym = getTableSymbol(first->val.str, TYPE_CANNOT_DUP);
        switch (sym->type->kind)
        {
        case TYPE_BASIC:
        if (op != NULL)//有初始值变量或者是形参
        {
            setOpName(op, mystrdup(first->val.str));
        }
        break;
        case TYPE_ARRAY://需要分配空间
        {
            if (isParam)//是形参,op!=NULL
            {
                setOpName(op, mystrdup(first->val.str));
            }
            else//是变量定义,op==NULL
            {
                OperandPtr arr = newOperand(OP_VARIABLE, 0, mystrdup(first->val.str));
                InterCodePtr code = newInterCode(IR_DEC, arr, getTypeSize(sym->type));
                InterCodesPtr codes = newInterCodes(code);
                addInterCodes(codes);
            }
        }
        break;
        case TYPE_STRUCTURE:
        {
            if (isParam)
            {
                setOpName(op, mystrdup(first->val.str));
            }
            else
            {
                OperandPtr structvar = newOperand(OP_VARIABLE, 0, mystrdup(first->val.str));
                InterCodePtr code = newInterCode(IR_DEC, structvar, getTypeSize(sym->type));
                InterCodesPtr codes = newInterCodes(code);
                addInterCodes(codes);
            }
        }
        break;
        }
    }
    else
    {
        translateVarDec(first, op, isParam);
    }
}

//ParamDec → Specifier VarDec
void translateParamDec(Node* node)
{
    assert(strEqual(node->unitName, "ParamDec"));
    Node* second = node->child->sibling;
    OperandPtr op = newOperand(OP_VARIABLE, 0, NULL);
    translateVarDec(second, op, true);
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

void translate_Cond(Node* node, OperandPtr  label_true, OperandPtr label_false)
{
    assert(strEqual(node->unitName, "Exp"));
    Node* first = node->child, * second = first->sibling;
    if (strEqual(first->unitName, "NOT"))
    {
        translate_Cond(second, label_false, label_true);
        return;
    }
    else if (second != NULL)
    {
        Node* third = second->sibling;
        if (strEqual(second->unitName, "RELOP"))
        {
            /*
            t1 = new_temp()
            t2 = new_temp()
            code1 = translate_Exp(Exp1, sym_table, t1)
            code2 = translate_Exp(Exp2, sym_table, t2)
            op = get_relop(RELOP);
            code3 = [IF t1 op t2 GOTO label_true]
            return code1 + code2 + code3 + [GOTO label_false]
            */
            OperandPtr t1 = newTemp(), t2 = newTemp();
            translateExp(first, t1);
            translateExp(third, t2);
            OperandPtr op = newOperand(OP_RELOP, 0, mystrdup(second->val.str));
            addInterCodes(newInterCodes(newInterCode(IR_IF_GOTO, t1, op, t2, label_true)));
            addInterCodes(newInterCodes(newInterCode(IR_GOTO, label_false)));
            return;
        }
        else if (strEqual(second->unitName, "AND"))
        {
            OperandPtr label1 = newLabel();
            translate_Cond(first, label1, label_false);
            addInterCodes(newInterCodes(newInterCode(IR_LABEL, label1)));
            translate_Cond(third, label_true, label_false);
            return;
        }
        else if (strEqual(second->unitName, "OR"))
        {
            OperandPtr label1 = newLabel();
            translate_Cond(first, label_true, label1);
            addInterCodes(newInterCodes(newInterCode(IR_LABEL, label1)));
            translate_Cond(third, label_true, label_false);
            return;
        }
    }
    /*
    t1 = new_temp()
    code1 = translate_Exp(Exp, sym_table, t1)
    code2 = [IF t1 != #0 GOTO label_true]
    return code1 + code2 + [GOTO label_false]
    */
    OperandPtr t1 = newTemp();
    OperandPtr op = newOperand(OP_RELOP, 0, mystrdup("!="));
    OperandPtr zero = newOperand(OP_CONSTANT, 0, NULL);
    translateExp(node, t1);
    addInterCodes(newInterCodes(newInterCode(IR_IF_GOTO, t1, op, zero, label_true)));
    addInterCodes(newInterCodes(newInterCode(IR_GOTO, label_false)));
}

// Args → Exp COMMA Args | Exp
void translateArgs(Node* node, ArgListPtr args)
{
    /*
    t1 = new_temp()
    code1 = translate_Exp(Exp, sym_table, t1)
    arg_list = t1 + arg_list
    return code1
    */
    Node* first = node->child;

    OperandPtr t1 = newTemp();
    translateExp(first, t1);
    addArgNode(newArgNode(t1), args);
    while (first->sibling != NULL)
    {
        first = first->sibling->sibling->child;
        OperandPtr temp = newTemp();
        translateExp(first, temp);
        addArgNode(newArgNode(temp), args);
    }
}

/*
Exp →
//
    Exp ASSIGNOP Exp  ok
//
    | NOT Exp
    | Exp AND Exp
    | Exp OR Exp
    | Exp RELOP Exp
//
    | Exp PLUS Exp
    | Exp MINUS Exp
    | Exp STAR Exp
    | Exp DIV Exp
//
    | LP Exp RP
    | MINUS Exp
//
    | ID LP Args RP
    | ID LP RP

    | Exp LB Exp RB
    | Exp DOT ID
//
    | ID
    | INT
    | FLOAT
*/
Symbol* translateExp(Node* node, OperandPtr place)
{
    assert(strEqual(node->unitName, "Exp"));
    Node* first = node->child, * second = first->sibling;
    char* firstname = first->unitName;
    if (second == NULL)
    {
        if (strEqual(firstname, "ID"))
        {
            Symbol* sym = getTableSymbol(first->val.str, TYPE_CANNOT_DUP);
            if (sym->type->kind == TYPE_STRUCTURE || sym->type->kind == TYPE_ARRAY)
            {
                setOpKind(place, OP_STRUCT_ARR_ID);
                setOpName(place, mystrdup(first->val.str));
                set2Arg(place, sym->isArg);
            }
            else
            {
                setOpKind(place, OP_VARIABLE);
                setOpName(place, mystrdup(first->val.str));
            }
            return sym;
        }
        else if (strEqual(firstname, "INT"))
        {
            setOpKind(place, OP_CONSTANT);
            setOpVal(place, first->val.int_val);
        }
        else
        {
            assert(0);
        }
        // else if (strEqual(first, "FLOAT"))   //无浮点数
    }
    else
    {
        char* secondname = second->unitName;
        Node* third = second->sibling;
        char* thirdname = NULL;
        if (third)
            thirdname = third->unitName;
        if (strEqual(first->unitName, "ID") && strEqual(second->unitName, "LP"))
        {
            Symbol* funcname = getTableSymbol(first->val.str, TYPE_FUNCTION);
            assert(funcname != NULL);

            if (strEqual(funcname->name, "read"))
            {
                addInterCodes(newInterCodes(newInterCode(IR_READ, place)));
            }
            else
            {
                OperandPtr func = newOperand(OP_FUNCTION, 0, mystrdup(funcname->name));
                if (strEqual(thirdname, "RP"))//no arguments
                {
                    addInterCodes(newInterCodes(newInterCode(IR_CALL, place, func)));
                }
                else if (strEqual(thirdname, "Args"))
                {
                    /*
                    code1 = translate_Args(Args, sym_table, arg_list)
                    if (function.name == “write”)
                    return code1 + [WRITE arg_list[1]] + [place := #0]
                    for i = 1 to length(arg_list)
                        code2 = code2 + [ARG arg_list[i]]
                    return code1 + code2 + [place := CALL function.name]
                    */
                    ArgListPtr arg_list = newArgList();
                    translateArgs(third, arg_list);
                    if (strEqual(funcname->name, "write"))
                    {
                        addInterCodes(newInterCodes(newInterCode(IR_WRITE, arg_list->head->arg)));
                        OperandPtr zero = newOperand(OP_CONSTANT, 0, NULL);
                        addInterCodes(newInterCodes(newInterCode(IR_ASSIGN, place, zero)));
                    }
                    else
                    {
                        ArgNodePtr h = arg_list->head;
                        while (h != NULL)
                        {
                            addInterCodes(newInterCodes(newInterCode(IR_ARG, h->arg)));
                            h = h->next;
                        }
                        addInterCodes(newInterCodes(newInterCode(IR_CALL, place, func)));
                    }
                }
                else
                {
                    assert(0);
                }
            }
        }
        else if (strEqual(secondname, "ASSIGNOP"))
        {
            OperandPtr left = newTemp();
            translateExp(first, left);
            OperandPtr right = newTemp();
            translateExp(second->sibling, right);
            int type = IR_ASSIGN;
            if (left->kind == OP_ADDRESS && right->kind != OP_ADDRESS)
            {
                type = IR_WRITE_ADDR;
            }
            else if (left->kind != OP_ADDRESS && right->kind == OP_ADDRESS)
            {
                type = IR_READ_ADDR;
            }
            else if (left->kind == OP_ADDRESS && right->kind == OP_ADDRESS)
            {
                type = IR_READ_WRITE_ADDR;
            }
            InterCodePtr code = newInterCode(type, left, right);
            InterCodesPtr codes = newInterCodes(code);
            addInterCodes(codes);
        }
        else if (strEqual(firstname, "MINUS"))
        {
            OperandPtr zero = newOperand(OP_CONSTANT, 0, NULL);
            OperandPtr right = newTemp();
            translateExp(second, right);
            InterCodePtr code = newInterCode(IR_SUB, place, zero, right);
            InterCodesPtr codes = newInterCodes(code);
            addInterCodes(codes);
        }
        else if (strEqual(firstname, "LP") && strEqual(secondname, "Exp"))
        {
            translateExp(second, place);
        }
        else if (strEqual(secondname, "PLUS") || strEqual(secondname, "MINUS") || strEqual(secondname, "STAR") || strEqual(secondname, "DIV"))
        {
            Node* third = second->sibling;
            OperandPtr lhs = newTemp();
            translateExp(first, lhs);

            OperandPtr rhs = newTemp();
            translateExp(third, rhs);

            InterCodePtr code;
            switch (secondname[0])
            {
            case 'P':
            code = newInterCode(IR_ADD, place, lhs, rhs);
            break;
            case 'M':
            code = newInterCode(IR_SUB, place, lhs, rhs);
            break;
            case 'S':
            code = newInterCode(IR_MUL, place, lhs, rhs);
            break;
            case 'D':
            code = newInterCode(IR_DIV, place, lhs, rhs);
            break;
            default:
            assert(0);
            break;
            }
            InterCodesPtr codes = newInterCodes(code);
            addInterCodes(codes);
        }
        else if (strEqual(secondname, "AND") || strEqual(secondname, "OR") || strEqual(secondname, "RELOP") || strEqual(firstname, "NOT"))
        {
            /*
            label1 = new_label()
            label2 = new_label()
            code0 = [place := #0]
            code1 = translate_Cond(Exp, label1, label2, sym_table)
            code2 = [LABEL label1] + [place := #1]
            return code0 + code1 + code2 + [LABEL label2]
            */
            OperandPtr label1 = newLabel(), label2 = newLabel();
            OperandPtr zero = newOperand(OP_CONSTANT, 0, NULL);
            OperandPtr one = newOperand(OP_CONSTANT, 1, (char*)1);
            addInterCodes(newInterCodes(newInterCode(IR_ASSIGN, place, zero)));
            translate_Cond(node, label1, label2);
            addInterCodes(newInterCodes(newInterCode(IR_LABEL, label1)));
            addInterCodes(newInterCodes(newInterCode(IR_ASSIGN, place, one)));
            addInterCodes(newInterCodes(newInterCode(IR_LABEL, label2)));
        }
        else if (strEqual(secondname, "DOT"))//Exp DOT ID
        {
            OperandPtr base = newTemp(), baseaddr;
            Symbol* location = translateExp(first, base);
            if (base->kind != OP_ADDRESS)
            {
                baseaddr = newTemp();
                addInterCodes(newInterCodes(newInterCode(IR_GET_ADDR, baseaddr, base)));//得到基地址,把它赋给baseaddr
            }
            else
            {
                baseaddr = base;
                setOpKind(baseaddr, OP_VARIABLE);
            }

            Symbol* target;
            int offset = getStructEleOffset(location, third->val.str, &target);//得到在结构体中的偏移量
            OperandPtr off = newOperand(OP_CONSTANT, offset, int2cptr(offset));
            setOpKind(place, OP_ADDRESS);
            addInterCodes(newInterCodes(newInterCode(IR_ADD, place, baseaddr, off)));
            return target;
        }
        else if (strEqual(secondname, "LB"))// Exp LB Exp RB,数组元素,仅支持一维数组
        {
            OperandPtr base = newTemp(), baseaddr;
            Symbol* location = translateExp(first, base);//得到基地址
            if (location == NULL)
            {
                printf(RED"Cannot translate : Code contains variables of multi - dimensional array type "
                    "or parameters of array type."NORMAL);
                exit(0);
            }
            if (base->kind != OP_ADDRESS)
            {
                baseaddr = newTemp();
                addInterCodes(newInterCodes(newInterCode(IR_GET_ADDR, baseaddr, base)));//得到基地址,把它赋给temp
            }
            else
            {
                baseaddr = base;
                setOpKind(baseaddr, OP_VARIABLE);//place是最终的地址,baseaddr当做普通变量
            }

            OperandPtr idx = newTemp();
            translateExp(third, idx);//得到下标

            OperandPtr width = newOperand(OP_CONSTANT, 0, 0);
            setOpVal(width, getArrEleWidth(location));//得到数组元素宽度

            OperandPtr offset = newTemp();
            addInterCodes(newInterCodes(newInterCode(IR_MUL, offset, idx, width)));

            setOpKind(place, OP_ADDRESS);
            addInterCodes(newInterCodes(newInterCode(IR_ADD, place, baseaddr, offset)));
            Type* t = location->type->t.array.elem;
            switch (t->kind)
            {
            case TYPE_STRUCTURE:
            return t->t.structure;
            break;
            default:;
                break;
            }
        }
        else
        {
            assert(0);
        }
    }
    return NULL;
}

// Dec → VarDec | VarDec ASSIGNOP Exp
void translateDec(Node* node)
{
    Node* first = node->child, * second = first->sibling;
    if (second == NULL)
    {
        translateVarDec(first, NULL, false);
    }
    else
    {
        Node* third = second->sibling;
        OperandPtr left = newOperand(OP_VARIABLE, 0, NULL);
        translateVarDec(first, left, false);
        OperandPtr right = newTemp();
        translateExp(third, right);
        InterCodePtr code = newInterCode(IR_ASSIGN, left, right);
        InterCodesPtr codes = newInterCodes(code);
        addInterCodes(codes);
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
        first = first->sibling->sibling->child;
        translateDec(first);
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

//Stmt → Exp SEMI
    // | CompSt
    // | RETURN Exp SEMI
    // | IF LP Exp RP Stmt
    // | IF LP Exp RP Stmt ELSE Stmt
    // | WHILE LP Exp RP Stmt
void translateStmt(Node* node)
{
    assert(strEqual(node->unitName, "Stmt"));
    Node* first = node->child;
    if (strEqual(first->unitName, "CompSt"))
    {
        translateCompSt(first);
    }
    else if (strEqual(first->unitName, "RETURN"))
    {
        OperandPtr op = newTemp();
        translateExp(first->sibling, op);
        InterCodePtr code = newInterCode(IR_RETURN, op);
        InterCodesPtr codes = newInterCodes(code);
        addInterCodes(codes);
    }
    else if (strEqual(first->unitName, "Exp"))
    {
        OperandPtr temp = newTemp();
        translateExp(first, temp);
    }
    else if (strEqual(first->unitName, "IF"))
    {
        Node* five = first->sibling->sibling->sibling->sibling;

        OperandPtr label1 = newLabel();
        OperandPtr label2 = newLabel();
        OperandPtr label3;
        translate_Cond(first->sibling->sibling, label1, label2);
        addInterCodes(newInterCodes(newInterCode(IR_LABEL, label1)));
        translateStmt(five);
        if (five->sibling != NULL)
        {
            label3 = newLabel();
            addInterCodes(newInterCodes(newInterCode(IR_GOTO, label3)));
        }
        addInterCodes(newInterCodes(newInterCode(IR_LABEL, label2)));
        if (five->sibling != NULL)
        {
            translateStmt(five->sibling->sibling);
            addInterCodes(newInterCodes(newInterCode(IR_LABEL, label3)));
        }
        /*
        无else
            code1 = translate_Cond(Exp, label1, label2, sym_table)
            code2 = translate_Stmt(Stmt1, sym_table)
            return code1 + [LABEL label1] + code2 + [LABEL label2]
        有else
            label1 = new_label()
            label2 = new_label()
            label3 = new_label()
            code1 = translate_Cond(Exp, label1, label2, sym_table)
            code2 = translate_Stmt(Stmt1, sym_table)
            code3 = translate_Stmt(Stmt2, sym_table)
            return code1 + [LABEL label1] + code2 + [GOTO label3]
            + [LABEL label2] + code3 + [LABEL label3]
        */
    }
    else if (strEqual(first->unitName, "WHILE"))
    {
        /*
        code1 = translate_Cond(Exp, label2, label3, sym_table)
        code2 = translate_Stmt(Stmt1, sym_table)
        return [LABEL label1] + code1 + [LABEL label2] + code2
        + [GOTO label1] + [LABEL label3]
        */
        OperandPtr label1 = newLabel(), label2 = newLabel(), label3 = newLabel();
        Node* Exp = first->sibling->sibling;
        addInterCodes(newInterCodes(newInterCode(IR_LABEL, label1)));
        translate_Cond(Exp, label2, label3);
        addInterCodes(newInterCodes(newInterCode(IR_LABEL, label2)));
        translateStmt(Exp->sibling->sibling);
        addInterCodes(newInterCodes(newInterCode(IR_GOTO, label1)));
        addInterCodes(newInterCodes(newInterCode(IR_LABEL, label3)));
    }
}

// StmtList → Stmt StmtList| null
void translateStmtList(Node* node)
{
    assert(strEqual(node->unitName, "StmtList"));
    Node* first = node->child;
    translateStmt(first);
    while (first->sibling != NULL)
    {
        first = first->sibling->child;
        translateStmt(first);
    }
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
    case OP_STRUCT_ARR_ID:
    case OP_ADDRESS:
    case OP_VARIABLE:
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

    case IR_DEC:
    fprintf(output, "DEC %s %d", p->u.dec.varName->u.name, p->u.dec.size);
    break;

    case IR_ASSIGN:
    if (p->u.assign.left == NULL)
        return;
    printOp(output, p->u.assign.left);
    fprintf(output, " := ");
    printOp(output, p->u.assign.right);
    break;

    case IR_GET_ADDR://将地址赋给临时变量
    printOp(output, p->u.assign.left);
    fprintf(output, " := ");
    if (p->u.assign.right->isArg == false)
        fprintf(output, "&");
    printOp(output, p->u.assign.right);
    break;

    case IR_READ_ADDR:
    printOp(output, p->u.assign.left);
    fprintf(output, " := *");
    printOp(output, p->u.assign.right);
    break;

    case IR_WRITE_ADDR:
    fprintf(output, "*");
    printOp(output, p->u.assign.left);
    fprintf(output, " := ");
    printOp(output, p->u.assign.right);
    break;

    case IR_READ_WRITE_ADDR:
    fprintf(output, "*");
    printOp(output, p->u.assign.left);
    fprintf(output, " := *");
    printOp(output, p->u.assign.right);
    break;

    case IR_ADD:
    case IR_SUB:
    case IR_MUL:
    case IR_DIV:
    if (p->u.binop.result == NULL)
        return;
    printOp(output, p->u.binop.result);
    fprintf(output, " := ");
    if (p->u.binop.left->kind == OP_ADDRESS)
    {
        fprintf(output, "*");
    }
    printOp(output, p->u.binop.left);
    {
        switch (p->kind)
        {
        case IR_ADD:
        fprintf(output, " + ");
        break;
        case IR_SUB:
        fprintf(output, " - ");
        break;
        case IR_MUL:
        fprintf(output, " * ");
        break;
        case IR_DIV:
        fprintf(output, " / ");
        break;
        default:
        break;
        }
    }
    if (p->u.binop.right->kind == OP_ADDRESS)
    {
        fprintf(output, "*");
    }
    printOp(output, p->u.binop.right);
    break;

    case IR_RETURN:
    fprintf(output, "RETURN ");
    if (p->u.oneop.op->kind == OP_ADDRESS)
        fprintf(output, "*");
    printOp(output, p->u.oneop.op);
    break;

    case IR_GOTO:
    fprintf(output, "GOTO ");
    printOp(output, p->u.oneop.op);
    break;

    case IR_LABEL:
    fprintf(output, "LABEL ");
    printOp(output, p->u.oneop.op);
    fprintf(output, " :");
    break;

    case IR_IF_GOTO:
    fprintf(output, "IF ");
    printOp(output, p->u.ifgoto.left);
    fprintf(output, " ");
    printOp(output, p->u.ifgoto.op);
    fprintf(output, " ");
    printOp(output, p->u.ifgoto.right);
    fprintf(output, " ");
    fprintf(output, "GOTO ");
    printOp(output, p->u.ifgoto.result);
    break;

    case IR_READ:
    fprintf(output, "READ ");
    printOp(output, p->u.oneop.op);
    break;

    case IR_WRITE:
    fprintf(output, "WRITE ");
    if (p->u.oneop.op->kind == OP_ADDRESS)
        fprintf(output, "*");
    printOp(output, p->u.oneop.op);
    break;
    case IR_CALL:
    if (p->u.assign.left->kind == OP_ADDRESS)
        fprintf(output, "*");
    printOp(output, p->u.assign.left);
    fprintf(output, " := CALL ");
    printOp(output, p->u.assign.right);
    break;

    case IR_ARG:
    fprintf(output, "ARG ");
    if (p->u.oneop.op->kind == OP_STRUCT_ARR_ID)
        fprintf(output, "&");
    else if (p->u.oneop.op->kind == OP_ADDRESS)
        fprintf(output, "*");
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