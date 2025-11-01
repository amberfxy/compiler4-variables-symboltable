#pragma once
#include "parser.h"
#include "stack_machine_ir.h"

// Generate IR code for a full function.
// If out_locals_aligned is NULL, locals are ignored.
void gen_function(Function* f, IRList* out_ir, int* out_locals_aligned);