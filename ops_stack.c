#include "decode.h"
#include "ops.h"


// 50 - 57
void PUSH_reg16(VM* vm, uint8_t cur_opcode){
    uint8_t reg_i = (cur_opcode & 0x07);
    PUSH_16(vm, read_reg16(vm, reg_i));
}

// 58 - 5F
void POP_reg16(VM* vm, uint8_t cur_opcode){
    uint8_t reg_i = (cur_opcode & 0x07);
    write_reg16(vm, reg_i, POP_16(vm));
}

// segreg stack op
void PUSH_segreg16(VM* vm, uint8_t cur_opcode){
    uint8_t reg_i = ((cur_opcode >> 3) & 0x07);
    PUSH_16(vm, read_segreg16(vm, reg_i));
}
void POP_segreg16(VM* vm, uint8_t cur_opcode){
    uint8_t reg_i = ((cur_opcode >> 3) & 0x07);
    write_segreg16(vm, reg_i, POP_16(vm));
}
void POP_rm16(VM* vm, uint8_t cur_opcode)
{
    ModRM m = decodeModRM(vm);
    if(!check_modRM_validation(vm, m, 0, cur_opcode)) return;
    uint32_t final_address = modRMaddress(vm, m);
    write_RM16(vm, m, final_address, POP_16(vm));
}

void PUSHF(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    PUSH_16(vm, read_flags16(vm));
}

void POPF(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    write_flags16(vm, POP_16(vm));
}