#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "symbol_table.h"
#include "codegen.h"
#include "stack_machine_ir.h"
#include "stack_machine.h"



// Compiler pipeline:
// 1. Read source file
// 2. Lex and parse
// 3. Generate IR
// 4. Emit assembly

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.jive> <output.asm>\n", argv[0]);
        return 1;
    }

    const char* input_path  = argv[1];
    const char* output_path = argv[2];

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
        return 1;
    }

    // ---------- Step 3: Generate IR ----------
    IRList ir;
    ir_init(&ir);
    gen_function(fn, &ir, NULL);

    // ---------- Step 4: Write assembly ----------
    FILE* out = fopen(output_path, "w");
    if (!out) {
        fprintf(stderr, "Error: cannot open output file %s\n", output_path);
        free(src);
        return 1;
    }

    stack_machine_emit(out, fn->name, &ir, 0);
    fclose(out);

    printf("✅ Compilation successful!\n");
    printf("Generated assembly: %s\n", output_path);

    free(src);
}