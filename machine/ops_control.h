#ifndef OPS_CONTROL_H
#define OPS_CONTROL_H
#include "vm.h"

void jmp_abs(VM* vm, uint16_t absolute_offset);
void jmp_abs_far(VM* vm, uint16_t absolute_offset, uint16_t new_seg);
void call_abs(VM* vm, uint16_t absolute_offset);
void call_abs_far(VM* vm, uint16_t absolute_offset, uint16_t new_seg);

#endif