#include "codegen.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global symbol stack (should be passed in, but for simplicity...)
extern SymStack* g_symstack;

// ========== Generate IR for expressions ==========
void gen_expr(IRList* ir, Expr* e) {
    switch (e->kind) {
        case EXPR_INT:
            ir_emit(ir, IR_PUSH_INT, e->int_value);
            break;

        case EXPR_VAR: {
            // Look up the variable in the symbol table
            Symbol* sym = symstack_lookup(g_symstack, e->var_name);
            if (!sym) {
                fprintf(stderr, "Error: undeclared variable '%s'\n", e->var_name);
                exit(1);
            }
            // Generate LOAD instruction with the variable's offset
            ir_emit(ir, IR_LOAD, sym->offset);
            free(sym);  // symstack_lookup allocates, so we free
            break;
        }

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
        case STMT_LET: {
            // Declare variable in symbol table
            int offset;
            if (!symstack_declare(g_symstack, s->let_.name, &offset)) {
                fprintf(stderr, "Error: variable '%s' already declared\n", s->let_.name);
                exit(1);
            }
            // If there's an initializer, generate code and store
            if (s->let_.init) {
                gen_expr(ir, s->let_.init);
                ir_emit(ir, IR_STORE, offset);
            }
            break;
        }

        case STMT_SET: {
            // Look up the variable
            Symbol* sym = symstack_lookup(g_symstack, s->set_.name);
            if (!sym) {
                fprintf(stderr, "Error: undeclared variable '%s'\n", s->set_.name);
                exit(1);
            }
            // Generate expression and store to variable
            gen_expr(ir, s->set_.expr);
            ir_emit(ir, IR_STORE, sym->offset);
            free(sym);
            break;
        }

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
    // Push a scope for this function
    symstack_push_scope(g_symstack);

    // Generate code for all statements
    for (int i = 0; i < fn->stmt_count; i++) {
        gen_stmt(ir, fn->stmts[i]);
    }

    // Calculate total local storage needed
    int total_bytes = symstack_total_locals(g_symstack);
    // Align to 16 bytes for x86-64 ABI
    if (out_locals_aligned) {
        *out_locals_aligned = (total_bytes + 15) & ~15;
    }

    // Pop the scope
    symstack_pop_scope(g_symstack);
}
