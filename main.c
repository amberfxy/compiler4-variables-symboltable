#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "symbol_table.h"
#include "codegen.h"
#include "stack_machine_ir.h"
#include "stack_machine.h"

// Global symbol stack
SymStack* g_symstack = NULL;

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.jive> <output.asm>\n", argv[0]);
        return 1;
    }

    const char* input_path  = argv[1];
    const char* output_path = argv[2];

    // Initialize global symbol stack
    g_symstack = symstack_new();

    // ---------- Step 1: Read source ----------
    FILE* f = fopen(input_path, "r");
    if (!f) {
        fprintf(stderr, "Error: cannot open input file %s\n", input_path);
        return 1;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* src = malloc(len + 1);
    fread(src, 1, len, f);
    src[len] = '\0';
    fclose(f);

    // ---------- Step 2: Initialize lexer & parser ----------
    Lexer lexer;
    init_lexer(&lexer, src);

    Parser parser;
    init_parser(&parser, src);

    Function* fn = parse_function(&parser);
    if (!fn) {
        fprintf(stderr, "Error: parsing failed.\n");
        free(src);
        symstack_free(g_symstack);
        return 1;
    }

    // ---------- Step 3: Generate IR ----------
    IRList ir;
    ir_init(&ir);
    int locals_aligned = 0;
    gen_function(fn, &ir, &locals_aligned);

    // ---------- Step 4: Write assembly ----------
    FILE* out = fopen(output_path, "w");
    if (!out) {
        fprintf(stderr, "Error: cannot open output file %s\n", output_path);
        free(src);
        symstack_free(g_symstack);
        return 1;
    }

    stack_machine_emit(out, fn->name, &ir, locals_aligned);
    fclose(out);

    printf("✅ Compilation successful!\n");
    printf("Generated assembly: %s\n", output_path);

    free(src);
    symstack_free(g_symstack);
    return 0;
}
