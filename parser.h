#pragma once
#include "lexer.h"

// ========== Expression ==========

typedef enum {
    EXPR_INT,
    EXPR_VAR,
    EXPR_BINOP
} ExprKind;

typedef struct Expr {
    ExprKind kind;
    union {
        int int_value;      // e.g., 3
        char* var_name;     // e.g., x
        struct {
            TokenType op;       // e.g., T_PLUS, T_MINUS, T_STAR, T_SLASH, T_PERCENT
            struct Expr* lhs;
            struct Expr* rhs;
        } bin;
    };
} Expr;

// ========== Statement ==========

typedef enum {
    STMT_LET,     // let x: int = expr;
    STMT_SET,     // set x = expr;
    STMT_RETURN   // return expr;
} StmtKind;

typedef struct Stmt {
    StmtKind kind;
    union {
        struct { char* name; Expr* init; } let_;
        struct { char* name; Expr* expr; } set_;
        struct { Expr* expr; } ret_;
    };
} Stmt;

// ========== Function ==========

typedef struct Function {
    char* name;
    Stmt** stmts;
    int stmt_count;
} Function;

// ========== Parser ==========

typedef struct {
    Lexer lexer;
    Token current;
} Parser;

// ========== API ==========

void init_parser(Parser* p, const char* src);
Function* parse_function(Parser* p);
Stmt** parse_statements(Parser* p, int* count);
Expr* parse_primary(Parser* p);
Expr* parse_binop(Parser* p);
Token expect(Parser* p, TokenType t, const char* what);