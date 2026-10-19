#include "decode.h"
#include "ops.h"
#include "ops_control.h"

// For absolute jumps within the same segment (if needed elsewhere)
void jmp_abs(VM* vm, uint16_t absolute_offset)
{
    write_ip16(vm, absolute_offset);
}

// For far jumps that change the segment
void jmp_abs_far(VM* vm, uint16_t absolute_offset, uint16_t new_seg)
{
    write_ip16(vm, absolute_offset);
    write_sreg16(vm, CS, new_seg);
}

void call_abs(VM* vm, uint16_t absolute_offset)
{
    push16(vm, read_ip16(vm));
    jmp_abs(vm, absolute_offset);
}

void call_abs_far(VM* vm, uint16_t absolute_offset, uint16_t new_seg)
{
    push16(vm, read_sreg16(vm, CS));
    push16(vm, read_ip16(vm));
    jmp_abs_far(vm, absolute_offset, new_seg);
}


// 70 -> 7f
void H_JCC_rel8(VM* vm, uint8_t cur_opcode)
{
    uint8_t invert = cur_opcode & 1;
    uint8_t condition = (cur_opcode >> 1) & 0x07;
    int16_t rel_offset = (int8_t)fetch_byte(vm);
    bool met;
    switch (condition)
    {
    case 0:
        met = get_flag(vm, FLAG_OF);
        break;
    case 1: met = get_flag(vm, FLAG_CF); break;
    case 2: met = get_flag(vm, FLAG_ZF); break;
    case 3: met = get_flag(vm, FLAG_CF) || get_flag(vm, FLAG_ZF); break;
    case 4: met = get_flag(vm, FLAG_SF); break;
    case 5: met = get_flag(vm, FLAG_PF); break;
    case 6: met = get_flag(vm, FLAG_SF) != get_flag(vm, FLAG_OF); break;
    case 7: met = get_flag(vm, FLAG_ZF) || (get_flag(vm, FLAG_SF) != get_flag(vm, FLAG_OF)); break;
    default: met = false; break;
    }
    if (invert ^ met)
    {
        jmp_abs(vm, read_ip16(vm) + rel_offset);
    }
}

// eb
void H_JMP_rel8(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    int16_t rel_offset = (int8_t)fetch_byte(vm);
    jmp_abs(vm, read_ip16(vm) + rel_offset);
}
// e9
void H_JMP_rel16(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    int16_t rel_offset = (int16_t)fetch_word(vm);
    jmp_abs(vm, read_ip16(vm) + rel_offset);
}
// EA off16 seg16
void H_JMP_far(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    uint16_t offset = fetch_word(vm);
    uint16_t seg = fetch_word(vm);
    jmp_abs_far(vm, offset, seg);
}
// e3
void H_JCXZ(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    int16_t rel_offset = (int8_t)fetch_byte(vm);
    if (read_r16(vm, CX) == 0)
    {
        jmp_abs(vm, read_ip16(vm) + rel_offset);
    }
}
// e2
void H_LOOP(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    int16_t rel_offset = (int8_t)fetch_byte(vm);
    uint16_t new_val = read_r16(vm, CX) - 1;
    write_r16(vm, CX, new_val);
    if (new_val != 0)
    {
        jmp_abs(vm, read_ip16(vm) + rel_offset);
    }
}
// e1
void H_LOOPZ(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    int16_t rel_offset = (int8_t)fetch_byte(vm);
    uint16_t new_val = read_r16(vm, CX) - 1;
    write_r16(vm, CX, new_val);
    if (new_val != 0 && get_flag(vm, FLAG_ZF))
    {
        jmp_abs(vm, read_ip16(vm) + rel_offset);
    }
}
// e0
void H_LOOPNZ(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    int16_t rel_offset = (int8_t)fetch_byte(vm);
    uint16_t new_val = read_r16(vm, CX) - 1;
    write_r16(vm, CX, new_val);
    if (new_val != 0 && !get_flag(vm, FLAG_ZF))
    {
        jmp_abs(vm, read_ip16(vm) + rel_offset);
    }
}


// CALL
// e8
void H_CALL_rel16(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    int16_t rel_offset = (int16_t)fetch_word(vm);
    call_abs(vm, read_ip16(vm) + rel_offset);
}
// 9a off16 seg16
void H_CALL_far(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    uint16_t offset = fetch_word(vm);
    uint16_t seg = fetch_word(vm);
    call_abs_far(vm, offset, seg);
}

// RET
// operations
static void ret_near(VM* vm){
    write_ip16(vm, pop16(vm));
}
static void ret_far(VM* vm){
    write_ip16(vm, pop16(vm));
    write_sreg16(vm, CS, pop16(vm));
}

// c3
void H_RET(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    ret_near(vm);
}
// c2
void H_RET_imm16(VM* vm, uint8_t cur_opcode){
    (void)cur_opcode;
    uint16_t imm = fetch_word(vm);
    ret_near(vm);
    write_r16(vm, SP, read_r16(vm, SP) + imm);
}
// cb
void H_RETF(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    ret_far(vm);
}
// ca
void H_RETF_imm16(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    uint16_t imm = fetch_word(vm);
    ret_far(vm);
    write_r16(vm, SP, read_r16(vm, SP) + imm);
}

// interrups

static void do_interrupt(VM* vm, uint8_t n)
{
    uint16_t entry = (uint16_t)n << 2;
    uint16_t flags = read_flags16(vm);
    push16(vm, flags);
    set_flag(vm, FLAG_IF, false);
    set_flag(vm, FLAG_TF, false);

    uint16_t new_ip = read_mem16(vm, (uint32_t)entry);
    uint16_t new_cs = read_mem16(vm, (uint32_t)entry + 2);
    call_abs_far(vm, new_ip, new_cs);
}
// cd
void H_INT_imm8(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    uint8_t n = fetch_byte(vm);
    do_interrupt(vm, n);
}
// cf
void H_IRET(VM* vm, uint8_t cur_opcode)
{
    (void)cur_opcode;
    ret_far(vm);
    write_flags16(vm, pop16(vm));
}
