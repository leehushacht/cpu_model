#include "decode.h"
#include "ops.h"
#include "alu.h"

typedef uint8_t (*alu_operands_handle8)(VM* vm, uint8_t a, uint8_t b);
typedef uint16_t (*alu_operands_handle16)(VM* vm, uint16_t a, uint16_t b);

static alu_operands_handle8  alu_op8[8];    // add8, or8, adc8, sbb8, and8, sub8, xor8, cmp8
static alu_operands_handle16 alu_op16[8];   // same, 16-bit

void set_alu(void)
{
        // put in other proc maybe?
    alu_op8[0] = alu_add8;
    alu_op8[1] = alu_or8;
    alu_op8[2] = alu_adc8;
    alu_op8[3] = alu_sbb8;
    alu_op8[4] = alu_and8;
    alu_op8[5] = alu_sub8;
    alu_op8[6] = alu_xor8;
    alu_op8[7] = alu_cmp8;

    alu_op16[0] = alu_add16;
    alu_op16[1] = alu_or16;
    alu_op16[2] = alu_adc16;
    alu_op16[3] = alu_sbb16;
    alu_op16[4] = alu_and16;
    alu_op16[5] = alu_sub16;
    alu_op16[6] = alu_xor16;
    alu_op16[7] = alu_cmp16;
    // --------------------
}
// alu operands:
static uint8_t all_alu_operands_handle8(VM* vm, uint8_t op, uint8_t a, uint8_t b){
    return alu_op8[op & 0x07](vm, a, b);
}
static uint16_t all_alu_operands_handle16(VM* vm, uint8_t op, uint16_t a, uint16_t b){
    return alu_op16[op & 0x07](vm, a, b);
}
//00, 08, 10, 18, 20, 28, 30, 38
void alu_rm8_r8(VM* vm, uint8_t cur_opcode){
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM8(vm, m, final_address, all_alu_operands_handle8(vm, (cur_opcode >> 3), read_RM8(vm, m, final_address), read_reg8(vm, m.reg)));
}
//01, 09, 11, 19, 21, 29, 31, 39
void alu_rm16_r16(VM* vm, uint8_t cur_opcode){
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM16(vm, m, final_address, all_alu_operands_handle16(vm, (cur_opcode >> 3), read_RM16(vm, m, final_address), read_reg16(vm, m.reg)));
}
//02, 0A, 12, 1A, 22, 2A, 32, 3A
void alu_r8_rm8(VM* vm, uint8_t cur_opcode){
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_reg8(vm, m.reg, all_alu_operands_handle8(vm, (cur_opcode >> 3), read_reg8(vm, m.reg), read_RM8(vm, m, final_address)));
}
//03, 0B, 13, 1B, 23, 2B, 33, 3B
void alu_r16_rm16(VM* vm, uint8_t cur_opcode){
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_reg16(vm, m.reg, all_alu_operands_handle16(vm, (cur_opcode >> 3), read_reg16(vm, m.reg), read_RM16(vm, m, final_address)));
}
//04, 0C, 14, 1C, 24, 2C, 34, 3C
void alu_al_imm8(VM* vm, uint8_t cur_opcode){
    write_reg8(vm, AL, all_alu_operands_handle8(vm, (cur_opcode >> 3), read_reg8(vm, AL), fetch_byte(vm)));
}
//05, 0D, 15, 1D, 25, 2D, 35, 3D
void alu_ax_imm16(VM* vm, uint8_t cur_opcode){
    write_reg16(vm, AX, all_alu_operands_handle16(vm, (cur_opcode >> 3), read_reg16(vm, AX), fetch_word(vm)));

}
//80
void alu_rm8_imm8(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM8(vm, m, final_address, all_alu_operands_handle8(vm, m.reg, read_RM8(vm, m, final_address), fetch_byte(vm)));
}
//81
void alu_rm16_imm16(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM16(vm, m, final_address, all_alu_operands_handle16(vm, m.reg, read_RM16(vm, m, final_address), fetch_word(vm)));
}
//83
void alu_rm16_imm8(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM16(vm, m, final_address, all_alu_operands_handle16(vm, m.reg, read_RM16(vm, m, final_address),(int16_t)(int8_t)fetch_byte(vm)));
}
//40 -> 47
void INC_reg16(VM* vm, uint8_t cur_opcode){
    write_reg16(vm, cur_opcode & 0x07, alu_inc16(vm, read_reg16(vm, cur_opcode & 0x07)));
}
// 48 -> 4f
void DEC_reg16(VM* vm, uint8_t cur_opcode){
    write_reg16(vm, cur_opcode & 0x07, alu_dec16(vm, read_reg16(vm, cur_opcode & 0x07)));
}