#include "decode.h"
void handle_unknown_opcode(VM* vm, uint8_t cur_opcode) {
    (void)cur_opcode; // later add to the print
    printf("Error: Unimplemented Opcode at IP: %04X\n", read_ip16(vm) - 1);
    turn_off_vm(vm);
}

bool check_modRM_validation(VM* vm, ModRM m, uint8_t expected_reg, uint8_t cur_opcode)
{
    // in the future you will have to add newer things -> check and read later
    if(m.reg == expected_reg) return true;
    handle_unknown_opcode(vm, cur_opcode);
    return false;
}
static uint32_t get_final_address(VM* vm, uint8_t mod, uint8_t rm){
    uint16_t base_address = 0;
    uint32_t final_address = 0;
    uint8_t segment_reg_index = DS;
    switch (rm)
    {
        
        case 0: base_address = read_reg16(vm, BX) + read_reg16(vm, SI); break;
        case 1: base_address = read_reg16(vm, BX) + read_reg16(vm, DI); break;
        case 2: base_address = read_reg16(vm, BP) + read_reg16(vm, SI); segment_reg_index = SS; break;
        case 3: base_address = read_reg16(vm, BP) + read_reg16(vm, DI); segment_reg_index = SS; break;
        case 4: base_address = read_reg16(vm, SI); break;
        case 5: base_address = read_reg16(vm, DI); break;
        case 6:
        {
            if (mod == 0)
            {
                base_address = fetch_word(vm); break; // special *
            }
            else
            {
                 base_address = read_reg16(vm, BP); segment_reg_index = SS; break;
            }
        } 
        case 7: base_address = read_reg16(vm, BX); break;      
    }
    switch (mod)
    {
        case 1: {
            int8_t disp8 = (int8_t)fetch_byte(vm); // 0xFE הופך למינוס 2 אמיתי
            base_address += disp8;
            break;
        }
        case 2: {
            int16_t disp16 = (int16_t)fetch_word(vm); 
            base_address += disp16;
            break;
        }
    }
    
    final_address = get_address(read_segreg16(vm, segment_reg_index), base_address); // later do read_reg16(vm, BX) for segreg also

    return final_address;
}

// < ------------ Mod RM Area ------------ > //
ModRM decodeModRM(VM *vm){
    // 00 = mod | 000 = reg | 000 = RM
    uint8_t cur_byte = fetch_byte(vm);
    ModRM m;
    m.mod = (cur_byte & 0xC0) >> 6;
    m.reg = (cur_byte & 0x38) >> 3;
    m.rm = (cur_byte & 0x07);
    return m;
}

uint32_t modRMaddress(VM* vm, ModRM m){
    if (m.mod == 3){
        return 0;
    }
    return get_final_address(vm, m.mod, m.rm);
}

uint16_t read_RM16(VM *vm, ModRM m, uint32_t address){
    if (m.mod == 3){
        return read_reg16(vm, m.rm);
    }
    return read_mem16(vm, address);
}

void write_RM16(VM *vm, ModRM m, uint32_t address, uint16_t v){
    if (m.mod == 3){
        write_reg16(vm, m.rm, v);
    }
    else{
        write_mem16(vm, address, v);
    }
}
uint8_t read_RM8(VM *vm, ModRM m, uint32_t address){
    if (m.mod == 3){
        return read_reg8(vm, m.rm);
    }
    return read_mem8(vm, address);
}


void write_RM8(VM *vm, ModRM m, uint32_t address, uint8_t v){
    if (m.mod == 3){
        write_reg8(vm, m.rm, v);
    }
    else{
        write_mem8(vm, address, v);
    }
}