#include "decode.h"
#include "ops.h"


// B0 -> B7
void H_MOV_r8_imm8(VM* vm, uint8_t cur_opcode){
    uint8_t reg_i = cur_opcode & 0x07;
    uint8_t imm8 = fetch_byte(vm);
    write_r8(vm, reg_i, imm8);
}
// B8 -> BF
void H_MOV_r16_imm16(VM* vm, uint8_t cur_opcode){ 
    uint8_t reg_i = cur_opcode & 0x07;
    uint16_t imm16 = fetch_word(vm);
    write_r16(vm, reg_i, imm16);
}

// 88
void H_MOV_rm8_r8(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decode_mod_rm(vm);
    uint32_t final_address = mod_rm_address(vm, m);
    write_rm8(vm, m, final_address, read_r8(vm, m.reg));
}
// 8A
void H_MOV_r8_rm8(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decode_mod_rm(vm);
    uint32_t final_address = mod_rm_address(vm, m);
    write_r8(vm, m.reg, read_rm8(vm, m, final_address));
}
// 89
void H_MOV_rm16_r16(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decode_mod_rm(vm);
    uint32_t final_address = mod_rm_address(vm, m);
    write_rm16(vm, m, final_address, read_r16(vm, m.reg));
}
// 8B
void H_MOV_r16_rm16(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decode_mod_rm(vm);
    uint32_t final_address = mod_rm_address(vm, m);
    write_r16(vm, m.reg, read_rm16(vm, m, final_address));
}
// C7
void H_MOV_rm16_imm16(VM* vm, uint8_t cur_opcode){
    ModRM m = decode_mod_rm(vm);
    if(!check_mod_rm_validation(vm, m, REG_ONLY(0), cur_opcode, false)) return;
    uint32_t final_address = mod_rm_address(vm, m);
    write_rm16(vm, m, final_address, fetch_word(vm));
}
// C6
void H_MOV_rm8_imm8(VM* vm, uint8_t cur_opcode){
    ModRM m = decode_mod_rm(vm);
    if(!check_mod_rm_validation(vm, m, REG_ONLY(0), cur_opcode, false)) return;
    uint32_t final_address = mod_rm_address(vm, m);
    write_rm8(vm, m, final_address, fetch_byte(vm));
}

// 8C
void H_MOV_rm16_sreg(VM* vm, uint8_t cur_opcode){
    ModRM m = decode_mod_rm(vm);
    if(!check_mod_rm_validation(vm, m, REG_RANGE(0, 3), cur_opcode, false)) return;
    uint32_t final_address = mod_rm_address(vm, m);
    write_rm16(vm, m, final_address, read_sreg16(vm, m.reg));
}
// 8E
void H_MOV_sreg_rm16(VM* vm, uint8_t cur_opcode){
    ModRM m = decode_mod_rm(vm);
    if(!check_mod_rm_validation(vm, m, REG_RANGE(0, 3), cur_opcode, false)) return;
    uint32_t final_address = mod_rm_address(vm, m);
    write_sreg16(vm, m.reg, read_rm16(vm, m, final_address));

    // ** when mov ss or mov cs -> there are some actions -> check later!
}

// a0 -> a3
void H_MOV_acc_moffs(VM* vm, uint8_t cur_opcode){
    uint16_t offset = fetch_word(vm);
    uint32_t address = get_address(read_sreg16(vm, DS), offset);
    switch (cur_opcode & 0x0003)
    {
    case 0:
        // from mem, 8
        write_r8(vm, AL, read_mem8(vm, address));
        break;
    case 1:
        // from mem, 16
        write_r16(vm, AX, read_mem16(vm, address));
        break;
    case 2:
        // to mem, 8
        write_mem8(vm, address, read_r8(vm, AL));
        break;
    default: //3
        // to mem, 16
        write_mem16(vm, address, read_r16(vm, AX));
        break;
    }
}