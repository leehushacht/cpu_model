#include "decode.h"
#include "ops.h"


// B0 -> B7
void MOV_imm8_to_REG(VM* vm, uint8_t cur_opcode){
    uint8_t reg_i = cur_opcode & 0x07;
    uint8_t imm8 = fetch_byte(vm);
    write_reg8(vm, reg_i, imm8);
}
// B8 -> BF
void MOV_imm16_to_REG(VM* vm, uint8_t cur_opcode){ 
    uint8_t reg_i = cur_opcode & 0x07;
    uint16_t imm16 = fetch_word(vm);
    write_reg16(vm, reg_i, imm16);
}

// 88
void MOV_r8_to_rm8(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM8(vm, m, final_address, read_reg8(vm, m.reg));
}
// 8A
void MOV_rm8_to_r8(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_reg8(vm, m.reg, read_RM8(vm, m, final_address));
}
// 89
void MOV_r16_to_rm16(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM16(vm, m, final_address, read_reg16(vm, m.reg));
}
// 8B
void MOV_rm16_to_r16(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_reg16(vm, m.reg, read_RM16(vm, m, final_address));
}
// C7
void MOV_imm16_to_rm16(VM* vm, uint8_t cur_opcode){
    ModRM m = decodeModRM(vm);
    if(!check_modRM_validation(vm, m, 0, cur_opcode)) return;
    uint32_t final_address = modRMaddress(vm, m);
    write_RM16(vm, m, final_address, fetch_word(vm));
}
// C6
void MOV_imm8_to_rm8(VM* vm, uint8_t cur_opcode){
    ModRM m = decodeModRM(vm);
    if(!check_modRM_validation(vm, m, 0, cur_opcode)) return;
    uint32_t final_address = modRMaddress(vm, m);
    write_RM8(vm, m, final_address, fetch_byte(vm));
}

// 8C
void MOV_segreg_to_rm16(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_RM16(vm, m, final_address, read_segreg16(vm, m.reg));
}
// 8E
void MOV_rm16_to_segreg(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ModRM m = decodeModRM(vm);
    uint32_t final_address = modRMaddress(vm, m);
    write_segreg16(vm, m.reg, read_RM16(vm, m, final_address));

    // ** when mov ss or mov cs -> there are some actions -> check later!
}

// a0 -> a3
void MOV_direct(VM* vm, uint8_t cur_opcode){
    uint16_t offset = fetch_word(vm);
    uint32_t address = get_address(read_segreg16(vm, DS), offset);
    switch (cur_opcode & 0x0003)
    {
    case 0:
        // from mem, 8
        write_reg8(vm, AL, read_mem8(vm, address));
        break;
    case 1:
        // from mem, 16
        write_reg16(vm, AX, read_mem16(vm, address));
        break;
    case 2:
        // to mem, 8
        write_mem8(vm, address, read_reg8(vm, AL));
        break;
    default: //3
        // to mem, 16
        write_mem16(vm, address, read_reg16(vm, AX));
        break;
    }
}