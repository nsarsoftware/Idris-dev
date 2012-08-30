#ifndef _CLOSURE_H
#define _CLOSURE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

// Closures

typedef enum {
    CON, INT, FLOAT, STRING, UNIT, PTR
} ClosureType;

typedef struct {
    int tag;
    int arity;
    void* args;
} con;

typedef struct {
    ClosureType ty;
    union {
        con c;
        int i;
        double f;
        char* str;
        void* ptr;
    } info;
} Closure;

typedef Closure* VAL;

typedef struct {
    VAL* valstack;
    int* intstack;
    double* floatstack;
    VAL* valstack_top;
    VAL* valstack_base;
    int* intstack_ptr;
    double* floatstack_ptr;
    void* heap;
    void* heap_next;
    int stack_max;
    VAL ret;
} VM;

VM* init_vm(int stack_size, size_t heap_size);

// Functions all take a pointer to their VM, and previous stack base, 
// and return nothing.
typedef void(*func)(VM*, VAL*);

// Register access 

#define RVAL (vm->ret)
#define LOC(x) (*(vm->valstack_base + (x)))
#define TOP(x) (*(vm->valstack_top + (x)))

// Retrieving values

#define GETSTR(x) (((VAL)(x))->info.str) 
#define GETPTR(x) (((void*)(x))->info.ptr) 
#define GETFLOAT(x) (((double)(x))->info.f)

#define TAG(x) ((x)->info.c.tag)

// Integers, floats and operators

typedef intptr_t i_int;

#define MKINT(x) ((void*)((x)<<1)+1)
#define GETINT(x) ((i_int)(x)>>1)
#define ISINT(x) ((((i_int)x)&1) == 1)

#define INTOP(op,x,y) MKINT((i_int)((((i_int)x)>>1) op (((i_int)y)>>1)))
#define FLOATOP(op,x,y) MKFLOAT(((GETFLOAT(x)) op (GETFLOAT(y))))
#define FLOATBOP(op,x,y) MKINT((i_int)(((GETFLOAT(x)) op (GETFLOAT(y)))))
#define ADD(x,y) (void*)(((i_int)x)+(((i_int)y)-1))
#define MULT(x,y) (MKINT((((i_int)x)>>1) * (((i_int)y)>>1)))

// Stack management

#define INITFRAME VAL* myoldbase
#define REBASE vm->valstack_base = oldbase
#define RESERVE(x)
#define ADDTOP(x) vm->valstack_top += (x)
#define TOPBASE(x) vm->valstack_top = vm->valstack_base + (x)
#define BASETOP(x) vm->valstack_base = vm->valstack_top + (x)
#define STOREOLD myoldbase = vm->valstack_base

#define CALL(f) f(vm, myoldbase);
#define TAILCALL(f) f(vm, oldbase);

// Creating new values (each value placed at the top of the stack)
VAL MKFLOAT(VM* vm, double val);
VAL MKSTR(VM* vm, char* str);

VAL MKCON(VM* vm, int tag, int arity, ...);

void PROJECT(VM* vm, VAL r, int loc, int arity); 

void dumpVal(VAL r);

// Handle stack overflow. 
// Just reports an error and exits.

void stackOverflow();

#endif 
