#include "objectCode.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "interCodes.h"
#include "help.h"

extern InterCodesList list;
static ObjectCodePtr ocHead;
static VarPtr varList;

static Register regs[REG_NUM];

int curReg = MIN_AVAIL_REG;
int spOffset = 0;
int curParam = 0;
int curArg = 0;
const int stackSize = 100;
const int NAME_LEN = 64;

char* getBuf(int size)
{
    char* buf = (char*)malloc(size * sizeof(char));
    return buf;
}

ObjectCodePtr newObjectCode(OC_Kind k, ...)
{
    ObjectCodePtr p = (ObjectCodePtr)malloc(sizeof(ObjectCode));
    p->kind = k;
    va_list ap;
    va_start(ap, k);
    switch (k)
    {

    case OC_MOVE:
    p->move.regx = va_arg(ap, int);
    p->move.regy = va_arg(ap, int);
    break;

    case OC_FUNC:
    p->func.funcname = va_arg(ap, char*);
    break;

    case OC_LABEL:
    p->label.x = va_arg(ap, int);
    break;

    case OC_LI:
    p->li.reg = va_arg(ap, int);
    p->li.imm = va_arg(ap, int);
    break;

    case OC_ADD:
    p->add.regx = va_arg(ap, int);
    p->add.regy = va_arg(ap, int);
    p->add.regz = va_arg(ap, int);
    break;

    case OC_SUB:
    p->sub.regx = va_arg(ap, int);
    p->sub.regy = va_arg(ap, int);
    p->sub.regz = va_arg(ap, int);
    break;

    case OC_MUL:
    p->mul.regx = va_arg(ap, int);
    p->mul.regy = va_arg(ap, int);
    p->mul.regz = va_arg(ap, int);
    break;

    case OC_DIV:
    p->div.regy = va_arg(ap, int);
    p->div.regz = va_arg(ap, int);
    break;

    case OC_SW:
    p->sw.regy = va_arg(ap, int);
    p->sw.offset = va_arg(ap, int);
    p->sw.regx = va_arg(ap, int);
    break;

    case OC_LW:
    p->lw.regx = va_arg(ap, int);
    p->lw.offset = va_arg(ap, int);
    p->lw.regy = va_arg(ap, int);
    break;

    case OC_J:
    p->j.label = va_arg(ap, int);
    break;

    case OC_MFLO:
    p->mflo.regx = va_arg(ap, int);
    break;

    case OC_JAL:
    p->jal.f = va_arg(ap, char*);
    break;

    case OC_BEQ:
    p->beq.regx = va_arg(ap, int);
    p->beq.regy = va_arg(ap, int);
    p->beq.z = va_arg(ap, int);
    break;

    case OC_BNE:
    p->bne.regx = va_arg(ap, int);
    p->bne.regy = va_arg(ap, int);
    p->bne.z = va_arg(ap, int);
    break;

    case OC_BGT:
    p->bgt.regx = va_arg(ap, int);
    p->bgt.regy = va_arg(ap, int);
    p->bgt.z = va_arg(ap, int);
    break;

    case OC_BLT:
    p->blt.regx = va_arg(ap, int);
    p->blt.regy = va_arg(ap, int);
    p->blt.z = va_arg(ap, int);
    break;

    case OC_BGE:
    p->bge.regx = va_arg(ap, int);
    p->bge.regy = va_arg(ap, int);
    p->bge.z = va_arg(ap, int);
    break;

    case OC_BLE:
    p->ble.regx = va_arg(ap, int);
    p->ble.regy = va_arg(ap, int);
    p->ble.z = va_arg(ap, int);
    break;

    case OC_LA:
    p->la.reg = va_arg(ap, int);
    p->la.name = va_arg(ap, char*);
    break;

    default:
    ;
    break;
    }
    va_end(ap);
    return p;
}

VarPtr newVar()
{
    VarPtr p = (VarPtr)malloc(sizeof(Var));
    memset(p, 0, sizeof(Var));
    return p;
}

VarPtr findVar(const char* name)
{
    VarPtr p = varList->next;
    while (p != varList)
    {
        if (strEqual(name, p->name))
        {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

void addObjectCode(ObjectCodePtr dummyHead, ObjectCodePtr p)
{
    if (dummyHead->next == dummyHead)
    {
        dummyHead->next = p;
        p->prev = dummyHead;
        dummyHead->prev = p;
        p->next = dummyHead;
    }
    else
    {
        dummyHead->prev->next = p;
        p->prev = dummyHead->prev;
        dummyHead->prev = p;
        p->next = dummyHead;
    }
}

void freeVars()
{
    VarPtr p = varList->next;
    while (p != varList)
    {
        VarPtr del = p;
        p = p->next;
        free(del->name);
        free(del);
    }
    free(varList);
}

void addVar(VarPtr p)
{
    if (varList->next == varList)
    {
        varList->next = p;
        p->prev = varList;
        varList->prev = p;
        p->next = varList;
    }
    else
    {
        varList->prev->next = p;
        p->prev = varList->prev;
        varList->prev = p;
        p->next = varList;
    }
}

static const char* regNames[REG_NUM] = {
    "$zero",
    "$at",//can't use
    "$v0",//存放返回值
    "$v1",//存放返回值
    "$a0",//存放函数参数
    "$a1",//存放函数参数
    "$a2",//存放函数参数
    "$a3",//存放函数参数
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
    "$k0",//can't use
    "$k1",//can't use
    "$gp",
    "$sp",
    "$fp",
    "$ra",
};

#define REG(X) regNames[X]

void printStartCode(FILE* output)
{
    fprintf(output, ".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\n");
    fprintf(output, "read:\n  li $v0, 4\n  la $a0, _prompt\n  syscall\n  li $v0, 5\n  syscall\n  jr $ra\n");
    fprintf(output, "write:\n  li $v0, 1\n  syscall\n  li $v0, 4\n  la $a0, _ret\n  syscall\n  move $v0, $0\n  jr $ra\n");
}

void initRegs()
{
    for (int i = 0; i < REG_NUM;++i)
    {
        regs[i].isUsed = false;
        regs[i].type = OP_NONE;
        regs[i].var = NULL;
    }
}

void lwVar2Reg(int index, VarPtr var)
{
    ObjectCodePtr ptr = newObjectCode(OC_LW, index, var->offset, REG_FP);
    addObjectCode(ocHead, ptr);
}

VarPtr getVar(OperandPtr p)
{
    char* name = getBuf(NAME_LEN);
    if (p->kind == OP_TEMP || p->kind == OP_ADDRESS)
    {
        sprintf(name, "t%d", p->u.value);
    }
    else if (p->kind == OP_VARIABLE)
    {
        strcpy(name, p->u.name);
    }
    VarPtr var = findVar(name);
    if (NULL == var)//不在变量表中
    {
        var = newVar();
        var->name = mystrdup(name);
        var->reg = REG_ZERO;//不在寄存器
        if (p->kind == OP_VARIABLE)
        {
            spOffset -= 4;
            var->offset = spOffset;
        }
        addVar(var);//加入变量表中
    }
    free(name);
    return var;
}

void swReg2Mem(int regIdx)
{
    VarPtr var = regs[regIdx].var;
    var->reg = REG_ZERO;
    ObjectCodePtr ptr = newObjectCode(OC_SW, regIdx, var->offset, REG_FP);
    addObjectCode(ocHead, ptr);
}

/*
* 如果y已经在某个寄存器中，不需要进行处理，选择这个寄存器作为Ry
* 如果y不在寄存器中，且有空闲寄存器，选择一个空闲寄存器作为Ry
*/
int getReg(OperandPtr p)
{
    VarPtr var = NULL;
    if (p->kind != OP_IMM)
    {
        var = getVar(p);
        if (var->reg != REG_ZERO)
            return var->reg;
    }
    else if (p->u.value == 0) //p is a immediate number
    {
        return REG_ZERO;
    }
    //找一个寄存器
    int target = 0;
    for (int i = MIN_AVAIL_REG;i < MAX_AVAIL_REG;++i)
    {
        if (regs[i].isUsed == false)//空闲寄存器,或者寄存器中是一个imm
        {
            target = i;
            break;
        }
    }
    if (target == 0)
    {
        for (int i = MIN_AVAIL_REG;i < MAX_AVAIL_REG;i++)
        {
            if (regs[i].type == OP_VARIABLE)
            {
                target = i;
                break;
            }
        }
        swReg2Mem(target);
    }
    if (p->kind != OP_IMM)
    {
        var->reg = target;
        regs[target].var = var;
    }
    else
    {
        ObjectCodePtr ptr = newObjectCode(OC_LI, target, p->u.value);
        addObjectCode(ocHead, ptr);
    }
    regs[target].type = p->kind;
    regs[target].isUsed = true;
    return target;
}

void resetReg(int reg)
{
    if (regs[reg].type == OP_IMM)
    {
        regs[reg].isUsed = false;
        regs[reg].type = OP_NONE;
        regs[reg].var = NULL;
    }
}

void resetRegs(int regx, int regy, int regz)
{
    resetReg(regx);
    resetReg(regy);
    resetReg(regz);
}

void genObjectCode(InterCodePtr p)
{
    ObjectCodePtr ptr = NULL;
    int regx = 0, regy = 0, regz = 0;
    int offset = 0;
    int left = 0, right = 0;
    OperandPtr leftOp = NULL, rightOp = NULL, result = NULL;
    switch (p->kind)
    {
    case IR_FUNCTION:
    ptr = newObjectCode(OC_FUNC, mystrdup(p->u.oneop.op->u.name));
    addObjectCode(ocHead, ptr);
    break;

    case IR_LABEL:
    ptr = newObjectCode(OC_LABEL, p->u.oneop.op->u.value);
    addObjectCode(ocHead, ptr);
    break;

    case IR_PARAM://TODO:
    // fprintf(output, "PARAM ");
    // printOp(output, getOneOpOp(p));
    break;

    case IR_DEC://TODO:
    // fprintf(output, "DEC %s %d", p->u.dec.varName->u.name, p->u.dec.size);
    break;

    case IR_ASSIGN:
    leftOp = p->u.assign.left;
    rightOp = p->u.assign.right;
    if (rightOp->kind == OP_IMM)//x := k
    {
        regx = getReg(leftOp);
        ptr = newObjectCode(OC_LI, regx, rightOp->u.value);
        addObjectCode(ocHead, ptr);
    }
    else                                       //x := y
    {
        regx = getReg(leftOp);
        regy = getReg(rightOp);
        ptr = newObjectCode(OC_MOVE, regx, regy);
        addObjectCode(ocHead, ptr);
    }
    break;

    case IR_GET_ADDR: // 将地址赋给临时变量
    leftOp = p->u.assign.left;
    rightOp = p->u.assign.right;
    regx = getReg(leftOp);
    ptr = newObjectCode(OC_LA, regx, rightOp->u.name);
    addObjectCode(ocHead, ptr);
    break;

    case IR_READ_ADDR:
    leftOp = p->u.assign.left;
    rightOp = p->u.assign.right;
    regx = getReg(leftOp);
    regy = getReg(rightOp);
    ptr = newObjectCode(OC_LW, regx, offset, regy);
    addObjectCode(ocHead, ptr);
    break;

    case IR_WRITE_ADDR:
    leftOp = p->u.assign.left;
    rightOp = p->u.assign.right;
    regx = getReg(leftOp);
    regy = getReg(rightOp);
    if (rightOp->kind == OP_IMM)//*x := k
    {
        ptr = newObjectCode(OC_LI, regy, rightOp->u.value);
        addObjectCode(ocHead, ptr);
    }
    ptr = newObjectCode(OC_SW, regy, offset, regx);
    addObjectCode(ocHead, ptr);
    break;

    case IR_READ_WRITE_ADDR://*x := *y
    leftOp = p->u.assign.left;
    rightOp = p->u.assign.right;
    regx = getReg(rightOp);
    regy = getReg(NULL);//regy存放y的值
    ptr = newObjectCode(OC_LW, regy, offset, regx);
    addObjectCode(ocHead, ptr);

    regx = getReg(leftOp);
    ptr = newObjectCode(OC_SW, regy, offset, regx);
    addObjectCode(ocHead, ptr);
    break;

    case IR_ADD:
    result = p->u.binop.result;
    leftOp = p->u.binop.left;
    rightOp = p->u.binop.right;
    left = leftOp->kind;
    right = rightOp->kind;
    regx = getReg(result);
    regy = getReg(leftOp);
    if (right == OP_IMM)
    {
        ptr = newObjectCode(OC_ADDI, regx, regy, rightOp->u.value);
        addObjectCode(ocHead, ptr);
    }
    else
    {
        regz = getReg(rightOp);
        ptr = newObjectCode(OC_ADD, regx, regy, regz);
        addObjectCode(ocHead, ptr);
    }
    break;

    case IR_SUB:
    result = p->u.binop.result;
    leftOp = p->u.binop.left;
    rightOp = p->u.binop.right;
    left = leftOp->kind;
    right = rightOp->kind;
    regx = getReg(result);
    regy = getReg(leftOp);
    if (right == OP_IMM)
    {
        ptr = newObjectCode(OC_ADDI, regx, regy, -rightOp->u.value);
        addObjectCode(ocHead, ptr);
    }
    else
    {
        regz = getReg(rightOp);
        ptr = newObjectCode(OC_SUB, regx, regy, regz);
        addObjectCode(ocHead, ptr);
    }
    break;

    case IR_MUL:
    result = p->u.binop.result;
    leftOp = p->u.binop.left;
    rightOp = p->u.binop.right;
    left = leftOp->kind;
    right = rightOp->kind;
    regx = getReg(result);
    regy = getReg(leftOp);
    regz = getReg(rightOp);
    ptr = newObjectCode(OC_MUL, regx, regy, regz);
    addObjectCode(ocHead, ptr);
    break;

    case IR_DIV:
    result = p->u.binop.result;
    leftOp = p->u.binop.left;
    rightOp = p->u.binop.right;
    left = leftOp->kind;
    right = rightOp->kind;
    regx = getReg(result);
    regy = getReg(leftOp);
    regz = getReg(rightOp);
    ptr = newObjectCode(OC_DIV, regy, regz);
    addObjectCode(ocHead, ptr);
    ptr = newObjectCode(OC_MFLO, regx);
    addObjectCode(ocHead, ptr);
    break;

    case IR_RETURN:
    regx = getReg(p->u.oneop.op);
    ptr = newObjectCode(OC_MOVE, REG_V0, regx);
    addObjectCode(ocHead, ptr);
    ptr = newObjectCode(OC_JR);
    addObjectCode(ocHead, ptr);
    break;

    case IR_GOTO:
    ptr = newObjectCode(OC_J, p->u.oneop.op->u.value);
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
        leftOp = p->u.ifgoto.left;
        rightOp = p->u.ifgoto.right;
        result = p->u.ifgoto.result;

        regx = getReg(leftOp);
        regy = getReg(rightOp);
        ptr = newObjectCode(k, regx, regy, result->u.value);
        addObjectCode(ocHead, ptr);
    }
    break;

    case IR_READ://TODO:
    // fprintf(output, "READ ");
    // printOp(output, p->u.oneop.op);
    break;

    case IR_WRITE://TODO:
    // fprintf(output, "WRITE ");
    // if (p->u.oneop.op->kind == OP_ADDRESS)
    //     fprintf(output, "*");
    // printOp(output, p->u.oneop.op);
    break;

    case IR_CALL://TODO:
    // if (p->u.assign.left->kind == OP_ADDRESS)
    //     fprintf(output, "*");
    // printOp(output, p->u.assign.left);
    // fprintf(output, " := CALL ");
    // printOp(output, p->u.assign.right);
    ptr = newObjectCode(OC_JAL, mystrdup(p->u.assign.right->u.name));
    addObjectCode(ocHead, ptr);
    leftOp = p->u.assign.left;
    regx = getReg(leftOp);
    ptr = newObjectCode(OC_MOVE, regx, REG_V0);
    addObjectCode(ocHead, ptr);
    break;

    case IR_ARG://TODO:
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
    resetRegs(regx, regy, regz);
}

void initOclist()
{
    ocHead = newObjectCode(OC_NONE);
    ocHead->next = ocHead->prev = ocHead;
}

void initVarList()
{
    varList = newVar();
    varList->prev = varList->next = varList;
}

void genObjectCodes()
{
    initOclist();
    initVarList();
    InterCodesPtr p = list.dummyHead->next;
    while (p != list.dummyHead)
    {
        genObjectCode(p->code);
        p = p->next;
    }
}

#define TAB "  "
#define STR(x) TAB #x
#define OP_RRR(instr, r1, r2, r3) fprintf(output, STR(instr) " %s, %s, %s", REG(r1), REG(r2), REG(r3));
#define OP_RRI(instr, r1, r2, imm) fprintf(output, STR(instr) " %s, %s, %d", REG(r1), REG(r2), imm);
#define OP_RR(instr, r1, r2) fprintf(output, STR(instr) " %s, %s", REG(r1), REG(r2));
#define OP_R(instr, r1) fprintf(output, STR(instr) " %s", REG(r1));
#define OP_ROR(instr, r1, offset, r2) fprintf(output, STR(instr) " %s, %d(%s)", REG(r1), offset, REG(r2));
#define OP_RI(instr, r1, imm) fprintf(output, STR(instr) " %s, %d", REG(r1), imm);
#define OP_RS(instr, r1, str) fprintf(output, STR(instr) " %s, %s", REG(r1),str);

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
    fprintf(output, "labbel%d:", p->label.x);
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
    fprintf(output, TAB"j label%d", p->j.label);
    break;

    case OC_JAL:
    fprintf(output, TAB"jal %s", p->jal.f);
    break;

    case OC_JR:
    fprintf(output, TAB"jr $ra");
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

    case OC_LA:
    OP_RS(la, p->la.reg, p->la.name);
    break;

    default:
    assert(0);
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