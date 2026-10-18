#include "opcodes.h"
#include "decode.h"
#include "ops.h"
#include "alu.h"
#include "ops_control.h"

InstructionHandler opcode_main_handle_list[256];

// fe
static void H_GROUP_FE_rm8(VM* vm, uint8_t cur_opcode){
    ModRM m = decode_mod_rm(vm);
    uint32_t final_address = mod_rm_address(vm, m);
    switch (m.reg)
    {
    case 0:
        write_rm8(vm, m, final_address, alu_inc8(vm, read_rm8(vm, m, final_address)));
        break;
    case 1:
        write_rm8(vm, m, final_address, alu_dec8(vm, read_rm8(vm, m, final_address)));
        break;
    default:
        handle_unknown_opcode(vm, cur_opcode);
        return;
    }
}
// ff
static void H_GROUP_FF_rm16(VM* vm, uint8_t cur_opcode){
    ModRM m = decode_mod_rm(vm);
    uint32_t final_address = mod_rm_address(vm, m);
    switch (m.reg)
    {
    case 0:
        write_rm16(vm, m, final_address, alu_inc16(vm, read_rm16(vm, m, final_address)));
        break;
    case 1:
        write_rm16(vm, m, final_address, alu_dec16(vm, read_rm16(vm, m, final_address)));
        break;
    case 2:
        call_abs(vm, read_rm16(vm, m, final_address));
        break;
    case 3:
        if(check_mod_rm_validation(vm, m, REG_ALL, cur_opcode, true)){
            call_abs_far(vm, read_rm16(vm, m, final_address),  read_rm16(vm, m, final_address + 2));
        }
        break;
    case 4:
        jmp_abs(vm, read_rm16(vm, m, final_address));
        break;
    case 5:
        if(check_mod_rm_validation(vm, m, REG_ALL, cur_opcode, true)){
            jmp_abs_far(vm, read_rm16(vm, m, final_address),  read_rm16(vm, m, final_address + 2));
        }
        break;
    case 6:
        push16(vm, read_rm16(vm, m, final_address));
        break;
    // add here all others ...
    default:

        handle_unknown_opcode(vm, cur_opcode);
        return;
    }
}

// f4
static void H_HLT(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    halt_vm(vm);
}

// 3. פונקציית האתחול - נקראת פעם אחת בתחילת התוכנית!
void init_opcode_table(void) {



    // fill with error func
    for (int i = 0; i < 256; i++) {
        opcode_main_handle_list[i] = handle_unknown_opcode;
    }
    // fill with opcodes
    for (int i = 0xB0; i <= 0xB7; i++){
        // complete
        opcode_main_handle_list[i] = H_MOV_r8_imm8; 
    }
    for (int i = 0xB8; i <= 0xBF; i++){
        // complete
        opcode_main_handle_list[i] = H_MOV_r16_imm16;
    }
    for (int i = 0x50; i <= 0x57; i++){
        // complete
        opcode_main_handle_list[i] = H_PUSH_r16;
    }
    for (int i = 0x58; i <= 0x5F; i++){
        // complete
        opcode_main_handle_list[i] = H_POP_r16;
    }
    // complete
    opcode_main_handle_list[0x89] = H_MOV_rm16_r16;
    opcode_main_handle_list[0x8B] = H_MOV_r16_rm16;
    opcode_main_handle_list[0xC7] = H_MOV_rm16_imm16;
    opcode_main_handle_list[0x88] = H_MOV_rm8_r8;
    opcode_main_handle_list[0x8A] = H_MOV_r8_rm8;
    opcode_main_handle_list[0xC6] = H_MOV_rm8_imm8;
    opcode_main_handle_list[0x8C] = H_MOV_rm16_sreg;
    opcode_main_handle_list[0x8E] = H_MOV_sreg_rm16;
    for (int row = 0; row < 8; row++){
        int base = row * 8;
        opcode_main_handle_list[base + 0] = H_ALU_rm8_r8;
        opcode_main_handle_list[base + 1] = H_ALU_rm16_r16;
        opcode_main_handle_list[base + 2] = H_ALU_r8_rm8;
        opcode_main_handle_list[base + 3] = H_ALU_r16_rm16;
        opcode_main_handle_list[base + 4] = H_ALU_acc_imm8;
        opcode_main_handle_list[base + 5] = H_ALU_acc_imm16;
    }
    for(int i = 0x40; i <= 0x47; i++){
        opcode_main_handle_list[i] = H_INC_r16;
    }
    for(int i = 0x48; i <= 0x4f; i++){
        opcode_main_handle_list[i] = H_DEC_r16;
    }
   
    opcode_main_handle_list[0xfe] = H_GROUP_FE_rm8;
    opcode_main_handle_list[0xff] = H_GROUP_FF_rm16;
    opcode_main_handle_list[0xF4] = H_HLT;

    opcode_main_handle_list[0x80] = H_ALU_rm8_imm8;
    opcode_main_handle_list[0x81] = H_ALU_rm16_imm16;
    opcode_main_handle_list[0x83] = H_ALU_rm16_imm8;

    opcode_main_handle_list[0x06] = H_PUSH_sreg;
    opcode_main_handle_list[0x0e] = H_PUSH_sreg;
    opcode_main_handle_list[0x16] = H_PUSH_sreg;
    opcode_main_handle_list[0x1e] = H_PUSH_sreg;

    opcode_main_handle_list[0x07] = H_POP_sreg;
    opcode_main_handle_list[0x17] = H_POP_sreg;
    opcode_main_handle_list[0x1f] = H_POP_sreg;

    opcode_main_handle_list[0x8f] = H_POP_rm16;

    opcode_main_handle_list[0x9c] = H_PUSHF;
    opcode_main_handle_list[0x9d] = H_POPF;

    for (int i = 0x70; i <= 0x7f; i++)
    {
        opcode_main_handle_list[i] = H_JCC_rel8;
    }
    opcode_main_handle_list[0xeb] = H_JMP_rel8;
    opcode_main_handle_list[0xe9] = H_JMP_rel16;
    opcode_main_handle_list[0xea] = H_JMP_far;
    opcode_main_handle_list[0xe3] = H_JCXZ;
    opcode_main_handle_list[0xe2] = H_LOOP;
    opcode_main_handle_list[0xe1] = H_LOOPZ;
    opcode_main_handle_list[0xe0] = H_LOOPNZ;
    opcode_main_handle_list[0xe8] = H_CALL_rel16;
    opcode_main_handle_list[0x9a] = H_CALL_far;
    opcode_main_handle_list[0xc3] = H_RET;
    opcode_main_handle_list[0xcb] = H_RETF;
    opcode_main_handle_list[0xc2] = H_RET_imm16;
    opcode_main_handle_list[0xca] = H_RETF_imm16;

    for(int i = 0xa0; i <= 0xa3; i++){
        opcode_main_handle_list[i] = H_MOV_acc_moffs;
    }
    // -------------------
    init_alu_tables();
    // --------------------
}