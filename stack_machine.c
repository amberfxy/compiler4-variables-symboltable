#include "stack_machine_ir.h"
#include <stdio.h>

void stack_machine_emit(FILE* out, const char* fn_name, IRList* ir, int local_bytes_aligned) {
    // ---- Function prologue ----
    fprintf(out, "global %s\n%s:\n", fn_name, fn_name);
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    if (local_bytes_aligned > 0)
        fprintf(out, "    sub rsp, %d\n", local_bytes_aligned);

    // ---- Translate each IR instruction ----
    for (int i = 0; i < ir->count; i++) {
        IR instr = ir->code[i];
        switch (instr.op) {
            case IR_PUSH_INT:
                fprintf(out, "    mov rax, %d\n", instr.imm);
                fprintf(out, "    push rax\n");
                break;

            case IR_ADD:
                fprintf(out, "    pop rbx\n    pop rax\n");
                fprintf(out, "    add rax, rbx\n    push rax\n");
                break;

            case IR_SUB:
                fprintf(out, "    pop rbx\n    pop rax\n");
                fprintf(out, "    sub rax, rbx\n    push rax\n");
                break;

            case IR_MUL:
                fprintf(out, "    pop rbx\n    pop rax\n");
                fprintf(out, "    imul rax, rbx\n    push rax\n");
                break;

            case IR_DIV:
                fprintf(out, "    pop rbx\n    pop rax\n");
                fprintf(out, "    cqo\n    idiv rbx\n    push rax\n");
                break;

            case IR_MOD:
                fprintf(out, "    pop rbx\n    pop rax\n");
                fprintf(out, "    cqo\n    idiv rbx\n    push rdx\n");
                break;

            // ---- NEW: local variable support ----
            case IR_LOAD:
                // load value from [rbp - offset] and push
                fprintf(out, "    mov rax, [rbp-%d]\n", instr.imm);
                fprintf(out, "    push rax\n");
                break;

            case IR_STORE:
                // pop value and store into [rbp - offset]
                fprintf(out, "    pop rax\n");
                fprintf(out, "    mov [rbp-%d], rax\n", instr.imm);
                break;

            case IR_RET:
                fprintf(out, "    pop rax\n");
                break;
        }
    }

    // ---- Function epilogue ----
    fprintf(out, "    leave\n");
    fprintf(out, "    ret\n");
}