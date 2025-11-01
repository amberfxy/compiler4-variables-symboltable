#include "stack_machine_ir.h"
#include <stdio.h>
#include <stdlib.h>

// Print IR list (for debugging or testing)
void ir_print(IRList* ir) {
    for (int i = 0; i < ir->count; i++) {
        IR instr = ir->code[i];
        switch (instr.op) {
            case IR_PUSH_INT:
                printf("%03d: PUSH_INT %d\n", i, instr.imm);
                break;

            case IR_ADD:
                printf("%03d: ADD\n", i);
                break;

            case IR_SUB:
                printf("%03d: SUB\n", i);
                break;

            case IR_MUL:
                printf("%03d: MUL\n", i);
                break;

            case IR_DIV:
                printf("%03d: DIV\n", i);
                break;

            case IR_MOD:
                printf("%03d: MOD\n", i);
                break;

            // --- NEW for Compiler 4 ---
            case IR_LOAD:
                printf("%03d: LOAD [rbp-%d]\n", i, instr.imm);
                break;

            case IR_STORE:
                printf("%03d: STORE [rbp-%d]\n", i, instr.imm);
                break;

            case IR_RET:
                printf("%03d: RET\n", i);
                break;

            default:
                printf("%03d: (unknown IR %d)\n", i, instr.op);
                break;
        }
    }
}