#include "lexer.h"
#include <stdbool.h>

// Look at the current character without consuming it
static char peek(Lexer* L) {
    if (L->src[L->pos] == '\r' && L->src[L->pos + 1] == '\n')
        return '\n'; // normalize CRLF to LF
    return L->src[L->pos];
}

static char peek_next(Lexer* L) {
    if (L->src[L->pos + 1] == '\r' && L->src[L->pos + 2] == '\n')
        return '\n';
    return L->src[L->pos + 1];
}

static char advance(Lexer* L) {
    char c = L->src[L->pos];
    if (c == '\r' && L->src[L->pos + 1] == '\n') { // handle Windows CRLF
        L->pos += 2;
        L->line++;
        return '\n';
    }
    if (c != '\0') L->pos++;
    if (c == '\n') L->line++;
    return c;
}

static void skip_ws(Lexer* L) {
    while (1) {
        char c = peek(L);
        if (isspace(c)) {
            advance(L);
            continue;
        }
        if (c == '/' && peek_next(L) == '/') {
            while (peek(L) != '\n' && peek(L) != '\0') advance(L);
            continue;
        }
        break;
    }
}

// Create a token for keyword or identifier
static Token make_kw_or_ident(const char* text, int line) {
    if (strcmp(text, "fn") == 0) return (Token){T_FN, strdup(text), 0, line};
    if (strcmp(text, "return") == 0) return (Token){T_RETURN, strdup(text), 0, line};
    if (strcmp(text, "let") == 0) return (Token){T_LET, strdup(text), 0, line};
    if (strcmp(text, "set") == 0) return (Token){T_SET, strdup(text), 0, line};
    if (strcmp(text, "int") == 0) return (Token){T_INT_TYPE, strdup(text), 0, line};
    return (Token){T_IDENTIFIER, strdup(text), 0, line};
}

// Create a token for number literals
static Token make_number(Lexer* L) {
    int start = L->pos;
    while (isdigit(peek(L))) advance(L);
    int len = L->pos - start;
    char* text = strndup(L->src + start, len);
    return (Token){T_INT_LITERAL, text, atoi(text), L->line};
}

// Initialize lexer
void init_lexer(Lexer* L, const char* src) {
    L->src = src;
    L->pos = 0;
    L->line = 1;
    L->current = (Token){T_INVALID, NULL, 0, 1};
    L->last_identifier[0] = '\0';
}

// Produce the next token
Token next_token(Lexer* L) {
    skip_ws(L);
    int line = L->line;
    char c = advance(L);

    // Debug: show each raw character
    printf("[DEBUG] CHAR: '%c' (ASCII %d) at line %d\n", c, (int)c, line);

    // End of file
    if (c == '\0')
        return (Token){T_EOF, "EOF", 0, line};

    // Single-character tokens
    switch (c) {
        case '(':
            return (Token){T_LPAREN, "(", 0, line};
        case ')':
            return (Token){T_RPAREN, ")", 0, line};
        case '{':
            return (Token){T_LBRACE, "{", 0, line};
        case '}':
            return (Token){T_RBRACE, "}", 0, line};
        case '+':
            return (Token){T_PLUS, "+", 0, line};
        case '-':
            if (peek(L) == '>') {
                advance(L);
                return (Token){T_ARROW, "->", 0, line};
            }
            return (Token){T_MINUS, "-", 0, line};
        case '*':
            return (Token){T_STAR, "*", 0, line};
        case '/':
            return (Token){T_SLASH, "/", 0, line};
        case '%':
            return (Token){T_PERCENT, "%", 0, line};
        case '=':
            return (Token){T_EQUAL, "=", 0, line};
        case ';':
            return (Token){T_SEMICOLON, ";", 0, line};
        case ':':
            return (Token){T_COLON, ":", 0, line};
    }

    // Identifiers / keywords
    if (isalpha(c)) {
        int start = L->pos - 1;
        while (isalnum(peek(L))) advance(L);
        int len = L->pos - start;
        char* text = strndup(L->src + start, len);
        strcpy(L->last_identifier, text);
        return make_kw_or_ident(text, line);
    }

    // Numbers
    if (isdigit(c)) {
        L->pos--;
        return make_number(L);
    }

    // Unknown characters
    return (Token){T_INVALID, "?", 0, line};
}

const char* token_type_to_string(TokenType t) {
    switch (t) {
        case T_EOF: return "EOF";
        case T_INT_LITERAL: return "INT_LITERAL";
        case T_IDENTIFIER: return "IDENTIFIER";
        case T_INT_TYPE: return "INT_TYPE";
        case T_RETURN: return "RETURN";
        case T_LET: return "LET";
        case T_SET: return "SET";
        case T_FN: return "FN";
        case T_LPAREN: return "LPAREN";
        case T_RPAREN: return "RPAREN";
        case T_LBRACE: return "LBRACE";
        case T_RBRACE: return "RBRACE";
        case T_PLUS: return "PLUS";
        case T_MINUS: return "MINUS";
        case T_STAR: return "STAR";
        case T_SLASH: return "SLASH";
        case T_PERCENT: return "PERCENT";
        case T_EQUAL: return "EQUAL";
        case T_SEMICOLON: return "SEMICOLON";
        case T_COLON: return "COLON";
        case T_ARROW: return "ARROW";
        case T_INVALID: return "INVALID";
        default: return "UNKNOWN";
    }
}

// Debug function to print all tokens
void debug_print_tokens(const char* src) {
    Lexer L;
    init_lexer(&L, src);

    Token t;
    printf("[DEBUG] ---- TOKEN STREAM ----\n");
    do {
        t = next_token(&L);
        printf("[DEBUG] %-12s  (%s)\n", token_type_to_string(t.type), t.text ? t.text : "NULL");
    } while (t.type != T_EOF);
    printf("[DEBUG] -----------------------\n");
}