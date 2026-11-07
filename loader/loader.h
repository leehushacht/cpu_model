#ifndef LOADER_H
#define LOADER_H
#include "vm.h"

bool load_setup_to_memory(VM* vm);
bool load_program_to_memory(VM* vm, const uint8_t* program, size_t length);

#endif