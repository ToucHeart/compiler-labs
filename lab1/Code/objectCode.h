#ifndef __OBJECT_CODE__
#define __OBJECT_CODE__
typedef enum
{
    OC_NONE,
    OC_FUNC,
    OC_LABEL,
    OC_LI,
    OC_MOVE,
    OC_ADD,
    OC_ADDI,
    OC_SUB,
    OC_MUL,
    OC_DIV,
    OC_MFLO,
    OC_LW,
    OC_SW,
    OC_J,
    OC_JAL,
    OC_JR,
    OC_BEQ,
    OC_BNE,
    OC_BGT,
    OC_BLT,
    OC_BGE,
    OC_BLE,
} OC_Kind;

typedef struct ObjectCode
{
    OC_Kind kind;
    union
    {
        struct
        {
            char *x; // lebel index
        } label;
        struct
        {
            int reg, imm;
        } li;
        struct
        {
            int regx, regy;
        } move;
        struct
        {
            int regx, regy, imm;
        } addi;
        struct
        {
            int regx, regy, regz;
        } add;
        struct
        {
            int regx, regy, regz;
        } sub;
        struct
        {
            int regx, regy, regz;
        } mul;
        struct
        {
            int regy, regz;
        } div;
        struct
        {
            int regx;
        } mflo;
        struct
        {
            int regx, regy, offset;
        } lw;
        struct
        {
            int regy, regx, offset;
        } sw;
        struct
        {
            int x
        } j;
        struct
        {
            char *f;
        } jal;
        struct
        {
            int regx, regy, z;
        } beq;
        struct
        {
            int regx, regy, z;
        } bne;
        struct
        {
            int regx, regy, z;
        } bgt;
        struct
        {
            int regx, regy, z;
        } blt;
        struct
        {
            int regx, regy, z;
        } bge;
        struct
        {
            int regx, regy, z;
        } ble;
        struct
        {
            char *funcname;
        } func;
    };
    struct ObjectCode *prev, *next;
} ObjectCode;
typedef ObjectCode *ObjectCodePtr;
void addObjectCode(ObjectCodePtr dummyHead, ObjectCodePtr p);
ObjectCodePtr newObjectCode(OC_Kind k, ...);
void initOClist(ObjectCodePtr dummyHead);
#endif