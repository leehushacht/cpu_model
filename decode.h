#ifndef DECODE_H
#define DECODE_H
#include "hal.h"
#include "vm.h"

#define REG_ONLY(n) (1u << (n))
#define REG_ALL 0xffu
#define REG_RANGE(lo, hi) (((1u << ((hi) - (lo) + 1)) - 1u) << (lo))

typedef struct { 
    uint8_t mod, 
    reg, 
    rm; 
} ModRM;

void handle_unknown_opcode(VM* vm, uint8_t cur_opcode);

bool check_mod_rm_validation(VM* vm, ModRM m, uint8_t allowed_regs, uint8_t cur_opcode, bool must_m);
// < ------------ Mod RM Area ------------ > //
ModRM decode_mod_rm(VM *vm);
uint32_t mod_rm_address(VM* vm, ModRM m);

uint16_t read_rm16(VM *vm, ModRM m, uint32_t address);
void write_rm16(VM *vm, ModRM m, uint32_t address, uint16_t v);
uint8_t read_rm8(VM *vm, ModRM m, uint32_t address);
void write_rm8(VM *vm, ModRM m, uint32_t address, uint8_t v);

#endif