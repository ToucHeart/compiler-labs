#include <stdio.h>
#include "interCodes.h"
#include "help.h"
#include "objectCode.h"

extern InterCodesList list;
static ObjectCodePtr ocHead;

#define REG_NUM 32
static const char* regs[REG_NUM] = {
    "$zero",
    "$at",
    "$v0",
    "$v1",
    "$a0",
    "$a1",
    "$a2",
    "$a3",
    "$t0",
    "$t1",
    "$t2",
    "$t3",
    "$t4",
    "$t5",
    "$t6",
    "$t7",
    "$s0",
    "$s1",
    "$s2",
    "$s3",
    "$s4",
    "$s5",
    "$s6",
    "$s7",
    "$t8",
    "$t9",
    "$k0",
    "$k1",
    "$gp",
    "$sp",
    "$fp",
    "$ra",
};
#define REG(X) regs[X]

void printStartCode(FILE* output)
{
    fprintf(output, ".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\n");
    fprintf(output, "read:\n  li $v0, 4\n  la $a0, _prompt\n  syscall\n  li $v0, 5\n  syscall\n  jr $ra\n");
    fprintf(output, "write:\n  li $v0, 1\n  syscall\n  li $v0, 4\n  la $a0, _ret\n  syscall\n  move $v0, $0\n  jr $ra\n");
}

/*
x := #k  li reg(x), k
x := y   move reg(x), reg(y)
x := y + #k     addi reg(x), reg(y), k
x := y + z      add reg(x), reg(y), reg(z)
x := y - #k     addi reg(x), reg(y), -k
x := y – z      sub reg(x), reg(y), reg(z)
x := y * z2     mul reg(x), reg(y), reg(z)
x := y / z      div reg(y), reg(z)
                mflo reg(x)
IF x == y GOTO z    beq reg(x), reg(y), z
IF x != y GOTO z    bne reg(x), reg(y), z
IF x > y GOTO z     bgt reg(x), reg(y), z
IF x < y GOTO z     blt reg(x), reg(y), z
IF x >= y GOTO z    bge reg(x), reg(y), z
IF x <= y GOTO z    ble reg(x), reg(y), z
*/

void genOc(InterCodePtr p)
{
    ObjectCodePtr ptr;
    int regx = 0, regy = 0, regz = 0;
    int offset = 0;
    int left, right;
    switch (p->kind)
    {
    case IR_FUNCTION:
    ptr = newObjectCode(OC_FUNC, mystrdup(p->u.oneop.op->u.name));
    addObjectCode(ocHead, ptr);
    break;

    case IR_LABEL:
    ptr = newObjectCode(OC_LABEL, mystrdup(p->u.oneop.op->u.name));
    addObjectCode(ocHead, ptr);
    break;

    case IR_PARAM:
    // fprintf(output, "PARAM ");
    // printOp(output, getOneOpOp(p));
    break;

    case IR_DEC:
    // fprintf(output, "DEC %s %d", p->u.dec.varName->u.name, p->u.dec.size);
    break;

    case IR_ASSIGN:
    if (p->u.assign.right->kind == OP_CONSTANT)
    {
        ptr = newObjectCode(OC_LI, regx, p->u.assign.right->u.value);
        addObjectCode(ocHead, ptr);
    }
    else
    {
        ptr = newObjectCode(OC_MOVE, regx, regy);
        addObjectCode(ocHead, ptr);
    }
    break;

    case IR_GET_ADDR: // 将地址赋给临时变量

    break;

    case IR_READ_ADDR:
    ptr = newObjectCode(OC_LW, regx, offset, regy);
    addObjectCode(ocHead, ptr);
    break;

    case IR_WRITE_ADDR:
    ptr = newObjectCode(OC_SW, regy, offset, regx);
    addObjectCode(ocHead, ptr);
    break;

    case IR_READ_WRITE_ADDR:
    // fprintf(output, "*");
    // printOp(output, p->u.assign.left);
    // fprintf(output, " := *");
    // printOp(output, p->u.assign.right);
    break;

    case IR_ADD:
    left = p->u.binop.left->kind;
    right = p->u.binop.right->kind;
    if (left == OP_CONSTANT)
    {
        ptr = newObjectCode(OC_LI, regy, p->u.binop.left->u.value);
        addObjectCode(ocHead, ptr);
    }
    if (right == OP_CONSTANT)
    {
        ptr = newObjectCode(OC_ADDI, regx, regy, p->u.binop.right->u.value);
        addObjectCode(ocHead, ptr);
    }
    else
    {
        ptr = newObjectCode(OC_ADD, regx, regy, regz);
        addObjectCode(ocHead, ptr);
    }
    break;
    case IR_SUB:
    left = p->u.binop.left->kind;
    right = p->u.binop.right->kind;
    if (left == OP_CONSTANT)
    {
        ptr = newObjectCode(OC_LI, regy, p->u.binop.left->u.value);
        addObjectCode(ocHead, ptr);
    }
    if (right == OP_CONSTANT)
    {
        ptr = newObjectCode(OC_ADDI, regx, regy, -p->u.binop.right->u.value);
        addObjectCode(ocHead, ptr);
    }
    else
    {
        ptr = newObjectCode(OC_SUB, regx, regy, regz);
        addObjectCode(ocHead, ptr);
    }
    break;
    case IR_MUL:
    left = p->u.binop.left->kind;
    right = p->u.binop.right->kind;
    if (left == OP_CONSTANT)
    {
        ptr = newObjectCode(OC_LI, regy, p->u.binop.left->u.value);
        addObjectCode(ocHead, ptr);
    }
    if (right == OP_CONSTANT)
    {
        ptr = newObjectCode(OC_LI, regz, p->u.binop.right->u.value);
        addObjectCode(ocHead, ptr);
    }
    ptr = newObjectCode(OC_MUL, regx, regy, regz);
    addObjectCode(ocHead, ptr);
    break;
    case IR_DIV:
    left = p->u.binop.left->kind;
    right = p->u.binop.right->kind;
    if (left == OP_CONSTANT)
    {
        ptr = newObjectCode(OC_LI, regy, p->u.binop.left->u.value);
        addObjectCode(ocHead, ptr);
    }
    if (right == OP_CONSTANT)
    {
        ptr = newObjectCode(OC_LI, regz, p->u.binop.right->u.value);
        addObjectCode(ocHead, ptr);
    }
    ptr = newObjectCode(OC_DIV, regy, regz);
    addObjectCode(ocHead, ptr);
    ptr = newObjectCode(OC_MFLO, regx);
    addObjectCode(ocHead, ptr);
    break;

    case IR_RETURN:
    ptr = newObjectCode(OC_MOVE, 2, regx);
    addObjectCode(ocHead, ptr);
    ptr = newObjectCode(OC_JR);
    addObjectCode(ocHead, ptr);
    break;

    case IR_GOTO:
    ptr = newObjectCode(OC_J, mystrdup(p->u.oneop.op->u.name));
    addObjectCode(ocHead, ptr);
    break;

    case IR_IF_GOTO:
    {
        OC_Kind k;
        char* op = p->u.ifgoto.op->u.name;
        if (strEqual(op, "!="))
            k = OC_BNE;
        else if (strEqual(op, "=="))
            k = OC_BEQ;
        else if (strEqual(op, ">"))
            k = OC_BGT;
        else if (strEqual(op, "<"))
            k = OC_BLT;
        else if (strEqual(op, "<="))
            k = OC_BLE;
        else if (strEqual(op, ">="))
            k = OC_BGE;
        ptr = newObjectCode(k, regx, regy, mystrdup(p->u.ifgoto.result->u.name));
        addObjectCode(ocHead, ptr);
    }
    break;

    case IR_READ:
    // fprintf(output, "READ ");
    // printOp(output, p->u.oneop.op);
    break;

    case IR_WRITE:
    // fprintf(output, "WRITE ");
    // if (p->u.oneop.op->kind == OP_ADDRESS)
    //     fprintf(output, "*");
    // printOp(output, p->u.oneop.op);
    break;
    case IR_CALL:
    // if (p->u.assign.left->kind == OP_ADDRESS)
    //     fprintf(output, "*");
    // printOp(output, p->u.assign.left);
    // fprintf(output, " := CALL ");
    // printOp(output, p->u.assign.right);
    ptr = newObjectCode(OC_JAL, mystrdup(p->u.assign.right->u.name));
    addObjectCode(ocHead, ptr);
    ptr = newObjectCode(OC_MOVE, regx, 2);
    addObjectCode(ocHead, ptr);
    break;

    case IR_ARG:
    // fprintf(output, "ARG ");
    // if (p->u.oneop.op->kind == OP_STRUCT_ARR_ID)
    //     fprintf(output, "&");
    // else if (p->u.oneop.op->kind == OP_ADDRESS && p->u.oneop.op->isBasicAddr)
    //     fprintf(output, "*");
    // printOp(output, p->u.oneop.op);
    break;
    default:
    fprintf(stderr, RED "code type is %d\n" NORMAL, p->kind);
    assert(0);
    break;
    }
}

void genObjectCode()
{
    ocHead = newObjectCode(OC_NONE);
    ocHead->next = ocHead->prev = ocHead;
    InterCodesPtr p = list.dummyHead->next;
    while (p != list.dummyHead)
    {
        genOc(p->code);
        p = p->next;
    }
}

#define STR(x) #x
#define OP_RRR(instr, r1, r2, r3) fprintf(output, STR(instr) " %s, %s, %s", REG(r1), REG(r2), REG(r3));
#define OP_RRI(instr, r1, r2, imm) fprintf(output, STR(instr) " %s, %s, %d", REG(r1), REG(r2), imm);
#define OP_RR(instr, r1, r2) fprintf(output, STR(instr) " %s, %s", REG(r1), REG(r2));
#define OP_R(instr, r1) fprintf(output, STR(instr) " %s", REG(r1));
#define OP_ROR(instr, r1, offset, r2) fprintf(output, STR(instr) " %s, %d(%s)", REG(r1), offset, REG(r2));
#define OP_RI(instr, r1, imm) fprintf(output, STR(instr) " %s, %d", REG(r1), imm);

void printObjectCode(FILE* output, ObjectCodePtr p)
{
    switch (p->kind)
    {
    case OC_LI:
    OP_RI(li, p->li.reg, p->li.imm)
        break;
    case OC_MOVE:
    OP_RR(move, p->move.regx, p->move.regy)
        break;
    case OC_FUNC:
    fprintf(output, "%s:", p->func.funcname);
    break;
    case OC_LABEL:
    fprintf(output, "%s", p->label.x);
    break;
    case OC_ADD: // add reg(x), reg(y), reg(z)
    OP_RRR(add, p->add.regx, p->add.regy, p->add.regz)
        break;
    case OC_ADDI: // addi reg(x), reg(y), k
    OP_RRI(addi, p->addi.regx, p->addi.regy, p->addi.imm)
        break;
    case OC_SUB: // sub reg(x), reg(y), reg(z)
    OP_RRR(sub, p->add.regx, p->add.regy, p->add.regz)
        break;
    case OC_MUL: // mul reg(x), reg(y), reg(z)
    OP_RRR(mul, p->mul.regx, p->mul.regy, p->mul.regz);
    break;
    case OC_DIV: // div reg(y), reg(z)
    OP_RR(div, p->div.regy, p->div.regz)
        break;
    case OC_MFLO: // mflo reg(x)
    OP_R(mflo, p->mflo.regx)
        break;
    case OC_LW: // lw reg(x), 0(reg(y))
    OP_ROR(lw, p->lw.regx, p->lw.offset, p->lw.regy)
        break;
    case OC_SW: // sw reg(y), 0(reg(x))
    OP_ROR(sw, p->sw.regy, p->sw.offset, p->sw.regx)
        break;
    case OC_J:
    fprintf(output, "j label%d", p->j.label);
    break;
    case OC_JAL:
    fprintf(output, "jal %s", p->jal.f);
    break;
    case OC_JR:
    fprintf(output, "jr $ra");
    break;
    case OC_BEQ: // beq reg(x), reg(y), z
    OP_RRI(beq, p->beq.regx, p->beq.regy, p->beq.z)
        break;
    case OC_BNE:
    OP_RRI(bne, p->bne.regx, p->bne.regy, p->bne.z)
        break;
    case OC_BGT:
    OP_RRI(bgt, p->bgt.regx, p->bgt.regy, p->bgt.z)
        break;
    case OC_BLT:
    OP_RRI(blt, p->blt.regx, p->blt.regy, p->blt.z)
        break;
    case OC_BGE:
    OP_RRI(bge, p->bge.regx, p->bge.regy, p->bge.z)
        break;
    case OC_BLE:
    OP_RRI(ble, p->ble.regx, p->ble.regy, p->ble.z)
        break;
    }
    fprintf(output, "\n");
}

void printObjectCodes(FILE* output)
{
    printStartCode(output);
    ObjectCodePtr p = ocHead->next;
    while (p != ocHead)
    {
        printObjectCode(output, p);
        p = p->next;
    }
}