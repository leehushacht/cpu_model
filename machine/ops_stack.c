#include "decode.h"
#include "ops.h"


// 50 - 57
void H_PUSH_r16(VM* vm, uint8_t cur_opcode){
    uint8_t reg_i = (cur_opcode & 0x07);
    // by the 8086 rules: if SP pushed -> push the after decrement value *(sp - 2)
    if (reg_i == SP) {push16(vm, read_r16(vm, reg_i) - 2);}
    else {push16(vm, read_r16(vm, reg_i));}
}

// 58 - 5F
void H_POP_r16(VM* vm, uint8_t cur_opcode){
    uint8_t reg_i = (cur_opcode & 0x07);
    write_r16(vm, reg_i, pop16(vm));
}

// segreg stack op
void H_PUSH_sreg(VM* vm, uint8_t cur_opcode){
    uint8_t reg_i = ((cur_opcode >> 3) & 0x07);
    push16(vm, read_sreg16(vm, reg_i));
}
void H_POP_sreg(VM* vm, uint8_t cur_opcode){
    uint8_t reg_i = ((cur_opcode >> 3) & 0x07);
    write_sreg16(vm, reg_i, pop16(vm));
}
void H_POP_rm16(VM* vm, uint8_t cur_opcode)
{
    ModRM m = decode_mod_rm(vm);
    if(!check_mod_rm_validation(vm, m, REG_ONLY(0), cur_opcode, false)) return;
    uint32_t final_address = mod_rm_address(vm, m);
    write_rm16(vm, m, final_address, pop16(vm));
}

void H_PUSHF(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    push16(vm, read_flags16(vm));
}

void H_POPF(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    write_flags16(vm, pop16(vm));
}