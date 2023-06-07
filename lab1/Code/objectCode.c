#include "objectCode.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void initOClist(ObjectCodePtr dummyHead)
{
    dummyHead = (ObjectCodePtr)malloc(sizeof(ObjectCode));
    dummyHead->next = dummyHead;
    dummyHead->prev = dummyHead;
}

ObjectCodePtr newObjectCode(OC_Kind k, ...)
{
    ObjectCodePtr p = (ObjectCodePtr)malloc(sizeof(ObjectCode));
    p->kind = k;
    va_list ap;
    va_start(ap, k);
    switch (k)
    {
    case OC_FUNC:
        p->func.funcname = va_arg(ap, char *);
        break;
    case OC_LABEL:
        p->label.x = va_arg(ap, char *);
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
    }
    va_end(ap);
    return p;
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