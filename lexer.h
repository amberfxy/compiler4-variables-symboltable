#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    T_EOF,
    T_INT_LITERAL,
    T_IDENTIFIER,
    T_INT_TYPE,
    T_RETURN,
    T_LET,
    T_SET,
    T_FN,
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    T_PERCENT,
    T_EQUAL,
    T_SEMICOLON,
    T_COLON,
    T_ARROW,
    T_INVALID
} TokenType;

typedef struct {
    TokenType type;
    char* text;
    int value;
    int line;
} Token;

typedef struct {
    const char* src;
    int pos;
    int line;
    Token current;
    char last_identifier[128];
} Lexer;

void init_lexer(Lexer* L, const char* src);
Token next_token(Lexer* L);
const char* token_type_to_string(TokenType t);

#endif