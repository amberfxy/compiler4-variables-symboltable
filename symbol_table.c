#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ----------------------------------------------------------
// Helper: hash function for strings (djb2 style)
// ----------------------------------------------------------
static unsigned long hash_string(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

// ----------------------------------------------------------
// Basic symbol table implementation
// ----------------------------------------------------------

Symbol_Table make_symbol_table(long number_of_slots) {
    Symbol_Table result = {
        .symbols = calloc(number_of_slots, sizeof(Symbol*)),
        .number_of_slots = number_of_slots,
        .entry_count = 0
    };
    return result;
}

bool insert_symbol(Symbol_Table* table, const char* name, Symbol_Data data) {
    unsigned long h = hash_string(name) % table->number_of_slots;
    Symbol* curr = table->symbols[h];

    // Check for duplicate declaration
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            fprintf(stderr, "Error: variable '%s' already declared.\n", name);
            return false;
        }
        curr = curr->next;
    }

    // Create new symbol
    Symbol* new_sym = malloc(sizeof(Symbol));
    new_sym->name = strdup(name);
    new_sym->offset = data.variable_slot;   // store offset
    new_sym->data = data;
    new_sym->next = table->symbols[h];
    table->symbols[h] = new_sym;

    table->entry_count++;
    if ((double)table->entry_count / table->number_of_slots > 0.7) {
        grow_table(table, table->number_of_slots * 2);
    }
    return true;
}

Symbol_Data* lookup_symbol(Symbol_Table* table, const char* name) {
    unsigned long h = hash_string(name) % table->number_of_slots;
    Symbol* curr = table->symbols[h];
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            return &curr->data;
        }
        curr = curr->next;
    }
    return NULL;
}

void grow_table(Symbol_Table* table, long new_number_of_slots) {
    Symbol** new_symbols = calloc(new_number_of_slots, sizeof(Symbol*));

    for (long i = 0; i < table->number_of_slots; i++) {
        Symbol* curr = table->symbols[i];
        while (curr) {
            Symbol* next = curr->next;
            unsigned long h = hash_string(curr->name) % new_number_of_slots;
            curr->next = new_symbols[h];
            new_symbols[h] = curr;
            curr = next;
        }
    }

    free(table->symbols);
    table->symbols = new_symbols;
    table->number_of_slots = new_number_of_slots;
}

// ----------------------------------------------------------
// Symbol stack (scope management)
// ----------------------------------------------------------

SymStack* symstack_new() {
    SymStack* s = calloc(1, sizeof(SymStack));
    s->capacity = 8;
    s->depth = 0;
    s->tables = calloc(s->capacity, sizeof(Symbol_Table));
    s->next_offset = 8;  // each var = 8 bytes
    return s;
}

void symstack_free(SymStack* s) {
    if (!s) return;
    free(s->tables);
    free(s);
}

void symstack_push_scope(SymStack* s) {
    if (s->depth >= s->capacity) {
        s->capacity *= 2;
        s->tables = realloc(s->tables, s->capacity * sizeof(Symbol_Table));
    }
    s->tables[s->depth++] = make_symbol_table(16);
}

void symstack_pop_scope(SymStack* s) {
    if (s->depth > 0) s->depth--;
}

int symstack_total_locals(SymStack* s) {
    return s->next_offset - 8;  // total bytes allocated
}

// Declare a new variable in the current scope
bool symstack_declare(SymStack* s, const char* name, int* out_offset) {
    if (s->depth == 0) symstack_push_scope(s);
    Symbol_Table* top = &s->tables[s->depth - 1];

    Symbol_Data data = {.variable_slot = s->next_offset};

    if (!insert_symbol(top, name, data)) {
        return false;
    }

    *out_offset = s->next_offset;
    s->next_offset += 8; // move stack by 8 bytes
    return true;
}

// Look up variable across all active scopes
Symbol* symstack_lookup(SymStack* s, const char* name) {
    for (int i = s->depth - 1; i >= 0; i--) {
        Symbol_Data* found = lookup_symbol(&s->tables[i], name);
        if (found) {
            Symbol* sym = malloc(sizeof(Symbol));
            sym->name = strdup(name);
            sym->offset = found->variable_slot;
            sym->data = *found;
            return sym;
        }
    }
    return NULL;
}