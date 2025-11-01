#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>

// ========== Generate IR for expressions ==========
void gen_expr(IRList* ir, Expr* e) {
    switch (e->kind) {
        case EXPR_INT:
            ir_emit(ir, IR_PUSH_INT, e->int_value);
            break;

        case EXPR_VAR:
            printf("[WARN] Variable access not implemented yet: %s\n", e->var_name);
            break;

        case EXPR_BINOP:
            gen_expr(ir, e->bin.lhs);
            gen_expr(ir, e->bin.rhs);

            switch (e->bin.op) {
                case T_PLUS:    ir_emit(ir, IR_ADD, 0); break;
                case T_MINUS:   ir_emit(ir, IR_SUB, 0); break;
                case T_STAR:    ir_emit(ir, IR_MUL, 0); break;
                case T_SLASH:   ir_emit(ir, IR_DIV, 0); break;
                case T_PERCENT: ir_emit(ir, IR_MOD, 0); break;
                default:
                    fprintf(stderr, "Unknown operator in binary expression.\n");
                    exit(1);
            }
            break;

        default:
            fprintf(stderr, "Unknown expression kind.\n");
            exit(1);
    }
}

// ========== Generate IR for statements ==========
void gen_stmt(IRList* ir, Stmt* s) {
    switch (s->kind) {
        case STMT_LET:
            if (s->let_.init)
                gen_expr(ir, s->let_.init);
            break;

        case STMT_SET:
            gen_expr(ir, s->set_.expr);
            break;

        case STMT_RETURN:
            gen_expr(ir, s->ret_.expr);
            ir_emit(ir, IR_RET, 0);
            break;

        default:
            fprintf(stderr, "Unknown statement kind.\n");
            exit(1);
    }
}

// ========== Generate IR for the whole function ==========
void gen_function(Function* fn, IRList* ir, int* out_locals_aligned) {
    (void)out_locals_aligned;  // not used yet, just silence warning

    for (int i = 0; i < fn->stmt_count; i++) {
        gen_stmt(ir, fn->stmts[i]);
    }
}