#include "hal.h"
#include "memory_managment.h"
#include "screen.h"
#include <unistd.h>
#include <stdio.h>


static void load_ivt(VM* vm)
{
    for (int n = 0; n < IVT_ENTRIES; n++)
    {
        uint32_t entry = IVT_AREA + n * 4;
        write_mem16(vm, entry, (uint16_t)n);          // offset  -> stub n
        write_mem16(vm, entry + 2, IRET_STUBS >> 4);     // segment -> F000
    }
}

static void load_system(VM* vm)
{
    for (int i = IRET_STUBS; i < IRET_STUBS + IVT_ENTRIES; i++)
    {
        write_mem8(vm, i, 0xcf);  // IRET
    }
}

static bool check_program_length(size_t length)
{
    return length > 0 && length <= MAX_PROGRAM_SIZE;  // more checks here later (?)
}

bool load_setup_to_memory(VM* vm)
{
    set_mode(vm, 0x3);
    clear_graphic(vm);
    load_system(vm);
    load_ivt(vm);
    
    return true;

}
static void prepare_segment_registers(VM* vm)
{
    write_sreg16(vm, CS, PROGRAM_SEGMENT);
    write_sreg16(vm, DS, PROGRAM_SEGMENT);
    write_sreg16(vm, ES, PROGRAM_SEGMENT);
    write_sreg16(vm, SS, PROGRAM_SEGMENT);
    write_r16(vm, SP, STACK_START);
    write_ip16(vm, PROGRAM_START);

}
static void prepare_flags(VM* vm)
{
    write_flags16(vm, 0);
}

bool load_program_to_memory(VM* vm, const uint8_t* program, size_t length)
{
    bool is_valid = check_program_length(length);
    if(!is_valid) return false;
    prepare_flags(vm);
    prepare_segment_registers(vm);
    uint32_t addres = get_address(PROGRAM_SEGMENT, PROGRAM_START);
    for (size_t i = 0; i < length; i++) {
        write_mem8(vm, addres + i, program[i]);
    }
    return true;

}

