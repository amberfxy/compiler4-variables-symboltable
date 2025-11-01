#pragma once
#include <stdio.h>
#include "stack_machine_ir.h"

// Emits x86-64 assembly from the intermediate representation (IR).
void stack_machine_emit(FILE* out, const char* fn_name, IRList* ir, int locals);