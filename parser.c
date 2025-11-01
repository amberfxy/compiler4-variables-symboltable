#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------- helpers ----------
static void advance(Parser* p) {
    p->current = next_token(&p->lexer);
}

Token expect(Parser* p, TokenType t, const char* what) {
    if (p->current.type != t) {
        fprintf(stderr,
                "Parse error at line %d: Expected %s, got %s\n",
                p->current.line, what, token_type_to_string(p->current.type));
        exit(1);
    }
    Token tok = p->current;
    advance(p);
    return tok;
}

// Forward declare
static Stmt* parse_stmt(Parser* p);

// ---------- expressions ----------

Expr* parse_primary(Parser* p) {
    Expr* e = calloc(1, sizeof(Expr));

    if (p->current.type == T_INT_LITERAL) {
        e->kind = EXPR_INT;

        // NOTE: adjust this if your lexer field name differs
        e->int_value = p->current.value;  // your lexer.h must have "int value;"
        advance(p);
        return e;
    }

    if (p->current.type == T_IDENTIFIER) {
        e->kind = EXPR_VAR;
        e->var_name = strdup(p->current.text);
        advance(p);
        return e;
    }

    fprintf(stderr, "Parse error at line %d: Expected expression, got %s\n",
            p->current.line, token_type_to_string(p->current.type));
    exit(1);
}

Expr* parse_binop(Parser* p) {
    Expr* left = parse_primary(p);

    while (p->current.type == T_PLUS ||
           p->current.type == T_MINUS ||
           p->current.type == T_STAR ||
           p->current.type == T_SLASH ||
           p->current.type == T_PERCENT) {
        TokenType op = p->current.type;
        advance(p);
        Expr* right = parse_primary(p);

        Expr* bin = calloc(1, sizeof(Expr));
        bin->kind = EXPR_BINOP;
        bin->bin.op = op;
        bin->bin.lhs = left;
        bin->bin.rhs = right;
        left = bin;
    }
    return left;
}

// ---------- statements ----------

static Stmt* parse_stmt(Parser* p) {
    Stmt* s = calloc(1, sizeof(Stmt));

    if (p->current.type == T_LET) {
        advance(p);
        Token name = expect(p, T_IDENTIFIER, "variable name");
        expect(p, T_COLON, ":");
        expect(p, T_INT_TYPE, "int");
        expect(p, T_EQUAL, "=");
        Expr* value = parse_binop(p);
        expect(p, T_SEMICOLON, ";");

        s->kind = STMT_LET;
        s->let_.name = strdup(name.text);
        s->let_.init = value;
        return s;
    }

    if (p->current.type == T_SET) {
        advance(p);
        Token name = expect(p, T_IDENTIFIER, "variable name");
        expect(p, T_EQUAL, "=");
        Expr* value = parse_binop(p);
        expect(p, T_SEMICOLON, ";");

        s->kind = STMT_SET;
        s->set_.name = strdup(name.text);
        s->set_.expr = value;
        return s;
    }

    if (p->current.type == T_RETURN) {
        advance(p);
        Expr* value = parse_binop(p);
        expect(p, T_SEMICOLON, ";");

        s->kind = STMT_RETURN;
        s->ret_.expr = value;
        return s;
    }

    fprintf(stderr, "Parse error at line %d: Unexpected token %s\n",
            p->current.line, token_type_to_string(p->current.type));
    exit(1);
}

// ---------- statement block ----------

Stmt** parse_statements(Parser* p, int* count) {
    const int CAP = 256;
    Stmt** stmts = malloc(sizeof(Stmt*) * CAP);
    int n = 0;

    while (p->current.type != T_RBRACE && p->current.type != T_EOF) {
        stmts[n++] = parse_stmt(p);
        if (n >= CAP) {
            fprintf(stderr, "Too many statements in block\n");
            exit(1);
        }
    }

    *count = n;
    return stmts;
}

// ---------- function ----------

Function* parse_function(Parser* p) {
    expect(p, T_FN, "fn");
    Token name = expect(p, T_IDENTIFIER, "function name");
    expect(p, T_LPAREN, "(");
    expect(p, T_RPAREN, ")");
    expect(p, T_ARROW, "->");
    expect(p, T_INT_TYPE, "return type");
    expect(p, T_LBRACE, "{");

    Function* fn = calloc(1, sizeof(Function));
    fn->name = strdup(name.text);

    fn->stmts = parse_statements(p, &fn->stmt_count);

    expect(p, T_RBRACE, "}");
    return fn;
}

// ---------- init ----------

void init_parser(Parser* p, const char* src) {
    init_lexer(&p->lexer, src);
    advance(p);
}