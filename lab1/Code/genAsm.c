#include <stdio.h>
#include "interCodes.h"
#include "help.h"
extern InterCodesList list;

void printStartCode(FILE *output)
{
    FILE *startcode = fopen("partcode.s", "r");
    int c;
    while ((c = fgetc(startcode)) != EOF)
    {
        fputc(c, output);
    }
    fputc('\n', output);
}

int getReg()
{
    static int regnum = 0;
    return regnum++;
}

void printRet(FILE *output)
{
    // move $v0, reg(x)
    // jr $ra
    fprintf(output, "move $v0, %d", getReg());
    fprintf(output, "jr $ra\n");
}

void printAssign(FILE *output, OperandPtr left, OperandPtr right)
{
    if (left == NULL)
        return;
    
}

void printOp(FILE *output, OperandPtr op)
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

void printasm(FILE *output, InterCodePtr p)
{
    switch (p->kind)
    {
    case IR_FUNCTION:
        printOp(output, p->u.oneop.op);
        fprintf(output, ":\n");
        break;

    case IR_PARAM:
        fprintf(output, "PARAM ");
        printOp(output, p->u.oneop.op);
        break;

    case IR_DEC:
        fprintf(output, "DEC %s %d", p->u.dec.varName->u.name, p->u.dec.size);
        break;

    case IR_ASSIGN:

        break;

    case IR_GET_ADDR: // 将地址赋给临时变量
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
        if (p->u.oneop.op->kind == OP_ADDRESS)
            fprintf(output, "*");
        printOp(output, p->u.oneop.op);
        break;

    case IR_GOTO:
        fprintf(output, "j ");
        printOp(output, p->u.oneop.op);
        break;

    case IR_LABEL:
        printOp(output, p->u.oneop.op);
        fprintf(output, ":");
        break;

    case IR_IF_GOTO:
        fprintf(output, "IF ");
        if (p->u.ifgoto.left->kind == OP_ADDRESS)
        {
            fprintf(output, "*");
        }
        printOp(output, p->u.ifgoto.left);
        fprintf(output, " ");
        printOp(output, p->u.ifgoto.op);
        fprintf(output, " ");
        if (p->u.ifgoto.right->kind == OP_ADDRESS)
        {
            fprintf(output, "*");
        }
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
        else if (p->u.oneop.op->kind == OP_ADDRESS && p->u.oneop.op->isBasicAddr)
            fprintf(output, "*");
        printOp(output, p->u.oneop.op);
        break;
    default:
        fprintf(stderr, RED "code type is %d\n" NORMAL, p->kind);
        assert(0);
        break;
    }
    fprintf(output, "\n");
}

void printAsm(FILE *output)
{
    printStartCode(output);
    InterCodesPtr p = list.dummyHead->next;
    while (p != list.dummyHead)
    {
        printasm(output, p->code);
        p = p->next;
    }
}