# CS5008 – Compiler 4: Variables and the Symbol Table  
**Author:** Xinyuan Fan (Amber)  
**NUID:** 002059850  

---

## 🧠 Overview

This project extends the Jive compiler from **Compiler 3** to support  
**variable declarations and assignments** using `let` and `set` statements.  

It implements a **symbol table** to manage variable names, offsets,  
and stack storage during code generation.  
The compiler now supports:
- `let identifier: type` and `let identifier: type = expression`
- `set identifier = expression`
- Local variable memory allocation within the current function scope
- New stack machine IR instructions `LOAD` and `STORE`
- Translation of variable access into x86-64 assembly

---

## 📁 Project Structure

| File | Description |
|------|--------------|
| `lexer.c / lexer.h` | Lexical analyzer (adds `let`, `set`, and `int` tokens) |
| `parser.c / parser.h` | Parser for new variable declaration and assignment syntax |
| `symbol_table.c / symbol_table.h` | Symbol table implementation (hash map for local variables) |
| `stack_machine_ir.c / stack_machine_ir.h` | IR layer defining new `LOAD` and `STORE` operations |
| `codegen.c` | AST → IR conversion; emits correct variable instructions |
| `stack_machine.c` | IR → Assembly translator (adds `mov [rbp-offset]`, `mov rax, [rbp-offset]`) |
| `main.c` | Compiler driver: ties all phases together and writes `.asm` output |
| `main.jive` | Sample input program for testing |

---

## ⚙️ Build & Run (via GitHub Codespaces or Linux)

```bash
# Compile the compiler
gcc -o compiler lexer.c parser.c symbol_table.c codegen.c \
stack_machine.c stack_machine_ir.c main.c

# Run the compiler on the sample program
./compiler main.jive out.asm

# Assemble and link the generated assembly
nasm -f elf64 out.asm -o out.o
gcc out.o -o a.out

# Run the program
./a.out
echo $?
