#pragma once
#include <stdbool.h>

// ---------- Basic symbol data ----------
typedef struct {
    int variable_slot; // stack offset (in bytes)
} Symbol_Data;

// ---------- Single symbol entry ----------
typedef struct Symbol {
    char* name;
    int offset;            // <--- NEW: offset for stack variable
    Symbol_Data data;
    struct Symbol* next;
} Symbol;

// ---------- Hash table ----------
typedef struct {
    Symbol** symbols;
    long number_of_slots;
    long entry_count;
} Symbol_Table;

// ---------- Symbol stack (scope manager) ----------
typedef struct SymStack {
    Symbol_Table* tables;
    int depth;
    int capacity;
    int next_offset;
} SymStack;

// ---------- Function declarations ----------
Symbol_Table make_symbol_table(long number_of_slots);
bool insert_symbol(Symbol_Table* table, const char* name, Symbol_Data data);
Symbol_Data* lookup_symbol(Symbol_Table* table, const char* name);
void grow_table(Symbol_Table* table, long new_number_of_slots);

// ---------- Scope management API ----------
SymStack* symstack_new();
void symstack_free(SymStack* s);
void symstack_push_scope(SymStack* s);
void symstack_pop_scope(SymStack* s);
int symstack_total_locals(SymStack* s);
bool symstack_declare(SymStack* s, const char* name, int* out_offset);
Symbol* symstack_lookup(SymStack* s, const char* name);