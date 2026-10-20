#ifndef OPS_H
#define OPS_H
#include "vm.h"
// B0 -> B7
void H_MOV_r8_imm8(VM* vm, uint8_t cur_opcode);
// B8 -> BF
void H_MOV_r16_imm16(VM* vm, uint8_t cur_opcode);

// 88
void H_MOV_rm8_r8(VM* vm, uint8_t cur_opcode);
// 8A
void H_MOV_r8_rm8(VM* vm, uint8_t cur_opcode);
// 89
void H_MOV_rm16_r16(VM* vm, uint8_t cur_opcode);
// 8B
void H_MOV_r16_rm16(VM* vm, uint8_t cur_opcode);
// C7
void H_MOV_rm16_imm16(VM* vm, uint8_t cur_opcode);
// C6
void H_MOV_rm8_imm8(VM* vm, uint8_t cur_opcode);

// 8C
void H_MOV_rm16_sreg(VM* vm, uint8_t cur_opcode);
// 8E
void H_MOV_sreg_rm16(VM* vm, uint8_t cur_opcode);
// a0 -> a3
void H_MOV_acc_moffs(VM* vm, uint8_t cur_opcode);

// 50 - 57
void H_PUSH_r16(VM* vm, uint8_t cur_opcode);
// 58 - 5F
void H_POP_r16(VM* vm, uint8_t cur_opcode);

// segreg stack op
void H_PUSH_sreg(VM* vm, uint8_t cur_opcode);
void H_POP_sreg(VM* vm, uint8_t cur_opcode);
void H_POP_rm16(VM* vm, uint8_t cur_opcode);
void H_PUSHF(VM* vm, uint8_t cur_opcode);

void H_POPF(VM* vm, uint8_t cur_opcode);


void init_alu_tables(void);
//00, 08, 10, 18, 20, 28, 30, 38
void H_ALU_rm8_r8(VM* vm, uint8_t cur_opcode);
//01, 09, 11, 19, 21, 29, 31, 39
void H_ALU_rm16_r16(VM* vm, uint8_t cur_opcode);
//02, 0A, 12, 1A, 22, 2A, 32, 3A
void H_ALU_r8_rm8(VM* vm, uint8_t cur_opcode);
//03, 0B, 13, 1B, 23, 2B, 33, 3B
void H_ALU_r16_rm16(VM* vm, uint8_t cur_opcode);
//04, 0C, 14, 1C, 24, 2C, 34, 3C
void H_ALU_acc_imm8(VM* vm, uint8_t cur_opcode);
//05, 0D, 15, 1D, 25, 2D, 35, 3D
void H_ALU_acc_imm16(VM* vm, uint8_t cur_opcode);
//80
void H_ALU_rm8_imm8(VM* vm, uint8_t cur_opcode);
//81
void H_ALU_rm16_imm16(VM* vm, uint8_t cur_opcode);
//83
void H_ALU_rm16_imm8(VM* vm, uint8_t cur_opcode);
//40 -> 47
void H_INC_r16(VM* vm, uint8_t cur_opcode);
// 48 -> 4f
void H_DEC_r16(VM* vm, uint8_t cur_opcode);

// 70 -> 7f
void H_JCC_rel8(VM* vm, uint8_t cur_opcode);
// eb
void H_JMP_rel8(VM* vm, uint8_t cur_opcode);
// e9
void H_JMP_rel16(VM* vm, uint8_t cur_opcode);
// EA off16 seg16
void H_JMP_far(VM* vm, uint8_t cur_opcode);
// e3
void H_JCXZ(VM* vm, uint8_t cur_opcode);
// e2
void H_LOOP(VM* vm, uint8_t cur_opcode);
// e1
void H_LOOPZ(VM* vm, uint8_t cur_opcode);
// e0
void H_LOOPNZ(VM* vm, uint8_t cur_opcode);
// e8
void H_CALL_rel16(VM* vm, uint8_t cur_opcode);
// 9a off16 seg16
void H_CALL_far(VM* vm, uint8_t cur_opcode);
// c3
void H_RET(VM* vm, uint8_t cur_opcode);
// cb
void H_RETF(VM* vm, uint8_t cur_opcode);
// c2
void H_RET_imm16(VM* vm, uint8_t cur_opcode);
// ca
void H_RETF_imm16(VM* vm, uint8_t cur_opcode);

// cd
void H_INT_imm8(VM* vm, uint8_t cur_opcode);
// cf
void H_IRET(VM* vm, uint8_t cur_opcode);

#endif