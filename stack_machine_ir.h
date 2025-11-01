#pragma once
#include <stdlib.h>

// ---------- IR operation kinds ----------
typedef enum {
    IR_PUSH_INT,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_MOD,
    IR_LOAD,   // new: load local variable from stack frame
    IR_STORE,  // new: store value into local variable
    IR_RET
} IROp;

// ---------- IR instruction ----------
typedef struct {
    IROp op;   // instruction type
    int  imm;  // immediate value or offset
} IR;

// ---------- IR list ----------
typedef struct {
    IR* code;
    int count;
    int cap;
} IRList;

// ---------- IR functions ----------
static inline void ir_init(IRList* L) {
    L->code = NULL;
    L->count = 0;
    L->cap = 0;
}

static inline void ir_emit(IRList* L, IROp op, int imm) {
    if (L->count == L->cap) {
        L->cap = (L->cap == 0) ? 64 : L->cap * 2;
        L->code = realloc(L->code, sizeof(IR) * L->cap);
    }
    L->code[L->count++] = (IR){ .op = op, .imm = imm };
}