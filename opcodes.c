#include "opcodes.h"
#include "decode.h"
#include "ops.h"
#include "alu.h"

InstructionHandler opcode_main_handle_list[256];

// fe
void GROUP_FE_rm8(VM* vm, uint8_t cur_opcode){
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    switch (m.reg)
    {
    case 0:
        write_RM8(vm, m, final_address, alu_inc8(vm, read_RM8(vm, m, final_address)));
        break;
    case 1:
        write_RM8(vm, m, final_address, alu_dec8(vm, read_RM8(vm, m, final_address)));
        break;
    default:
        handle_unknown_opcode(vm, cur_opcode);
        return;
    }
}
// ff
void GROUP_FF_rm16(VM* vm, uint8_t cur_opcode){
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    switch (m.reg)
    {
    case 0:
        write_RM16(vm, m, final_address, alu_inc16(vm, read_RM16(vm, m, final_address)));
        break;
    case 1:
        write_RM16(vm, m, final_address, alu_dec16(vm, read_RM16(vm, m, final_address)));
        break;
    case 6:
        PUSH_16(vm, read_RM16(vm, m, final_address));
        break;
    // add here all others ...
    default:

        handle_unknown_opcode(vm, cur_opcode);
        return;
    }
}

// f4
void HLT(VM* vm, uint8_t cur_opcode){
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
        opcode_main_handle_list[i] = MOV_imm8_to_REG; 
    }
    for (int i = 0xB8; i <= 0xBF; i++){
        // complete
        opcode_main_handle_list[i] = MOV_imm16_to_REG;
    }
    for (int i = 0x50; i <= 0x57; i++){
        // complete
        opcode_main_handle_list[i] = PUSH_reg16;
    }
    for (int i = 0x58; i <= 0x5F; i++){
        // complete
        opcode_main_handle_list[i] = POP_reg16;
    }
    // complete
    opcode_main_handle_list[0x89] = MOV_r16_to_rm16;
    opcode_main_handle_list[0x8B] = MOV_rm16_to_r16;
    opcode_main_handle_list[0xC7] = MOV_imm16_to_rm16;
    opcode_main_handle_list[0x88] = MOV_r8_to_rm8;
    opcode_main_handle_list[0x8A] = MOV_rm8_to_r8;
    opcode_main_handle_list[0xC6] = MOV_imm8_to_rm8;
    opcode_main_handle_list[0x8C] = MOV_segreg_to_rm16;
    opcode_main_handle_list[0x8E] = MOV_rm16_to_segreg;
    for (int row = 0; row < 8; row++){
        int base = row * 8;
        opcode_main_handle_list[base + 0] = alu_rm8_r8;
        opcode_main_handle_list[base + 1] = alu_rm16_r16;
        opcode_main_handle_list[base + 2] = alu_r8_rm8;
        opcode_main_handle_list[base + 3] = alu_r16_rm16;
        opcode_main_handle_list[base + 4] = alu_al_imm8;
        opcode_main_handle_list[base + 5] = alu_ax_imm16;
    }
    for(int i = 0x40; i <= 0x47; i++){
        opcode_main_handle_list[i] = INC_reg16;
    }
    for(int i = 0x48; i <= 0x4f; i++){
        opcode_main_handle_list[i] = DEC_reg16;
    }
   
    opcode_main_handle_list[0xfe] = GROUP_FE_rm8;
    opcode_main_handle_list[0xff] = GROUP_FF_rm16;
    opcode_main_handle_list[0xF4] = HLT;

    opcode_main_handle_list[0x80] = alu_rm8_imm8;
    opcode_main_handle_list[0x81] = alu_rm16_imm16;
    opcode_main_handle_list[0x83] = alu_rm16_imm8;

    opcode_main_handle_list[0x06] = PUSH_segreg16;
    opcode_main_handle_list[0x0e] = PUSH_segreg16;
    opcode_main_handle_list[0x16] = PUSH_segreg16;
    opcode_main_handle_list[0x1e] = PUSH_segreg16;

    opcode_main_handle_list[0x07] = POP_segreg16;
    opcode_main_handle_list[0x17] = POP_segreg16;
    opcode_main_handle_list[0x1f] = POP_segreg16;

    opcode_main_handle_list[0x8f] = POP_rm16;

    opcode_main_handle_list[0x9c] = PUSHF;
    opcode_main_handle_list[0x9d] = POPF;
    // in progress: 
    
    for(int i = 0xa0; i <= 0xa3; i++){
        opcode_main_handle_list[i] = MOV_direct;
    }
    // -------------------
    set_alu();
    // --------------------
}