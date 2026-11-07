#include "do_int_n.h"
#include "hal.h"
#include "screen.h"

static void handle_unknown_int(VM* vm, uint8_t n, uint8_t func)
{
    printf("Error: unsupported INT %02Xh function %02Xh at IP: %04X\n", n, func, read_initial_ip_op16(vm));
    turn_off_vm(vm);
}

void do_int_10h(VM* vm)
{
    switch (read_r8(vm, AH))
    {
    case 0:
    {
        set_mode(vm, read_r8(vm, AL));
        break;
    }
    case 0x0e:
    {
        print_char(vm, read_r8(vm, AL));
        break;
    }
    case 0x02:
    {
        // set cursor
        set_cursor(vm, read_r8(vm, DL), read_r8(vm, DH));
        break;
    }
    case 0x03:
    {
        // get cursor
        uint8_t col;
        uint8_t row;
        get_cursor(vm, &col, &row);
        write_r8(vm, DL, col);
        write_r8(vm, DH, row);
        break;
    }
    case 0x0c:
    {
        // put pixel
        uint16_t y = read_r16(vm, DX);
        uint16_t x = read_r16(vm, CX);
        uint8_t val = read_r8(vm, AL);
        set_pixel(vm, x, y, val);
        break;
    }
    case 0x0f:
    {
        write_r8(vm, AL, get_mode(vm));
        write_r8(vm, AH, get_columns(vm));
        write_r8(vm, BH, 0);  // page num -> currently not supported, only for "do it right"
        break;
    }
    default:
        handle_unknown_int(vm, 0x10, read_r8(vm, AH));
        break;
    }
}
void do_int_21h(VM* vm)
{
    switch (read_r8(vm, AH))
    {
    case 0x2:
    {
        print_char(vm, read_r8(vm, DL));
        break;
    }
    case 0x9:
    {
        uint16_t i = 0;
        while (true)
        {
            uint16_t off = read_r16(vm, DX) + i;
            uint8_t ch = read_mem8(vm, get_address(read_sreg16(vm, DS), off));
            if (ch == '$') break;
            print_char(vm, ch);
            i++;
        }
        break;
    }
    case 0x4c:
    {
        turn_off_vm(vm);
        break;
    }
    default:
    {
        handle_unknown_int(vm, 0x21, read_r8(vm, AH));
        break;
    }
    }
}