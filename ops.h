#ifndef OPS_H
#define OPS_H
#include "vm.h"
// B0 -> B7
void MOV_imm8_to_REG(VM* vm, uint8_t cur_opcode);
// B8 -> BF
void MOV_imm16_to_REG(VM* vm, uint8_t cur_opcode);

// 88
void MOV_r8_to_rm8(VM* vm, uint8_t cur_opcode);
// 8A
void MOV_rm8_to_r8(VM* vm, uint8_t cur_opcode);
// 89
void MOV_r16_to_rm16(VM* vm, uint8_t cur_opcode);
// 8B
void MOV_rm16_to_r16(VM* vm, uint8_t cur_opcode);
// C7
void MOV_imm16_to_rm16(VM* vm, uint8_t cur_opcode);
// C6
void MOV_imm8_to_rm8(VM* vm, uint8_t cur_opcode);

// 8C
void MOV_segreg_to_rm16(VM* vm, uint8_t cur_opcode);
// 8E
void MOV_rm16_to_segreg(VM* vm, uint8_t cur_opcode);
// a0 -> a3
void MOV_direct(VM* vm, uint8_t cur_opcode);

// 50 - 57
void PUSH_reg16(VM* vm, uint8_t cur_opcode);
// 58 - 5F
void POP_reg16(VM* vm, uint8_t cur_opcode);

// segreg stack op
void PUSH_segreg16(VM* vm, uint8_t cur_opcode);
void POP_segreg16(VM* vm, uint8_t cur_opcode);
void POP_rm16(VM* vm, uint8_t cur_opcode);
void PUSHF(VM* vm, uint8_t cur_opcode);

void POPF(VM* vm, uint8_t cur_opcode);


void set_alu(void);
//00, 08, 10, 18, 20, 28, 30, 38
void alu_rm8_r8(VM* vm, uint8_t cur_opcode);
//01, 09, 11, 19, 21, 29, 31, 39
void alu_rm16_r16(VM* vm, uint8_t cur_opcode);
//02, 0A, 12, 1A, 22, 2A, 32, 3A
void alu_r8_rm8(VM* vm, uint8_t cur_opcode);
//03, 0B, 13, 1B, 23, 2B, 33, 3B
void alu_r16_rm16(VM* vm, uint8_t cur_opcode);
//04, 0C, 14, 1C, 24, 2C, 34, 3C
void alu_al_imm8(VM* vm, uint8_t cur_opcode);
//05, 0D, 15, 1D, 25, 2D, 35, 3D
void alu_ax_imm16(VM* vm, uint8_t cur_opcode);
//80
void alu_rm8_imm8(VM* vm, uint8_t cur_opcode);
//81
void alu_rm16_imm16(VM* vm, uint8_t cur_opcode);
//83
void alu_rm16_imm8(VM* vm, uint8_t cur_opcode);
//40 -> 47
void INC_reg16(VM* vm, uint8_t cur_opcode);
// 48 -> 4f
void DEC_reg16(VM* vm, uint8_t cur_opcode);

#endif