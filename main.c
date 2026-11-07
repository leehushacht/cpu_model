#include "vm.h"
#include "opcodes.h"
#include "hal.h"
#include "memory_managment.h"
#include "loader.h"

static void test_screen(VM* vm){
    static const uint8_t prog[] = {
        0xB8, 0x03, 0x00,       // 0100  MOV AX, 0003h   ; INT 10h AH=00: set mode 3
        0xCD, 0x10,             // 0103  INT 10h
        0xBA, 0x1E, 0x01,       // 0105  MOV DX, 011Eh   ; string
        0xB4, 0x09,             // 0108  MOV AH, 09h     ; print string
        0xCD, 0x21,             // 010A  INT 21h
        0xB4, 0x02,             // 010C  MOV AH, 02h     ; print char
        0xB2, 0x41,             // 010E  MOV DL, 'A'
        0xCD, 0x21,             // 0110  INT 21h
        0xB4, 0x03,             // 0112  MOV AH, 03h     ; get cursor
        0xB7, 0x00,             // 0114  MOV BH, 0
        0xCD, 0x10,             // 0116  INT 10h
        0xB4, 0x4C,             // 0118  MOV AH, 4Ch     ; exit
        0xB0, 0x07,             // 011A  MOV AL, 7
        0xCD, 0x21,             // 011C  INT 21h
        0x48, 0x49, 0x0D, 0x0A, 0x24   // 011E  "HI\r\n$"
    };
    bool res = load_program_to_memory(vm, prog, sizeof(prog));
    if (!res)
    {
        printf("not loaded\n");
    }
    else{
        printf("great! loaded!\n");
    }

}

static void print_screen_state(const VM* vm){
    printf("row0=%04X %04X  row1=%04X  cursor=%04X  mode=%02X  running=%d\n",
        read_mem16(vm, TEXT_VIDEO_AREA),
        read_mem16(vm, TEXT_VIDEO_AREA + 2),
        read_mem16(vm, TEXT_VIDEO_AREA + 0xA0),
        read_mem16(vm, BIOS_CURSOR),
        read_mem8 (vm, BIOS_VIDEO_MODE),
        is_vm_running(vm));
}

int main(void){
    
    init_opcode_table();
    VM* vm = create_vm();
    if(vm == NULL) return -1;
    turn_on_vm(vm);

    load_setup_to_memory(vm);
    print_screen_state(vm);
    test_screen(vm);

    run_vm(vm);
    print_screen_state(vm);

    //print_state(vm);
    destroy_vm(vm);
    return 0;
}
