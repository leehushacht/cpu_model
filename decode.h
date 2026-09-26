#ifndef DECODE_H
#define DECODE_H
#include "hal.h"
#include "vm.h"

typedef struct { 
    uint8_t mod, 
    reg, 
    rm; 
} ModRM;

void handle_unknown_opcode(VM* vm, uint8_t cur_opcode);

bool check_modRM_validation(VM* vm, ModRM m, uint8_t expected_reg, uint8_t cur_opcode);
// < ------------ Mod RM Area ------------ > //
ModRM decodeModRM(VM *vm);
uint32_t modRMaddress(VM* vm, ModRM m);

uint16_t read_RM16(VM *vm, ModRM m, uint32_t address);
void write_RM16(VM *vm, ModRM m, uint32_t address, uint16_t v);
uint8_t read_RM8(VM *vm, ModRM m, uint32_t address);
void write_RM8(VM *vm, ModRM m, uint32_t address, uint8_t v);

#endif