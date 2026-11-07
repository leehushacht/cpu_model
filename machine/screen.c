#include "memory_managment.h"
#include "hal.h"
#include "screen.h"

static void set_text_video(VM* vm, uint8_t a, uint8_t color)
{
    for (int i = TEXT_VIDEO_AREA; i < TEXT_VIDEO_AREA + TEXT_VIDEO_SIZE; i+=2)
    {
        write_mem8(vm, i, a);
        write_mem8(vm, i + 1, color);
    }
}

static void set_graphic(VM* vm, uint8_t val)
{
    for (int i = GRAPHIC_AREA; i < GRAPHIC_AREA + GRAPHIC_SIZE; i++)
    {
        write_mem8(vm, i, val);
    }
}

void print_char(VM* vm, uint8_t ch)
{
    uint8_t mode = get_mode(vm);
    uint8_t row;
    uint8_t col;
    get_cursor(vm, &col, &row);
    uint16_t screen_width = read_mem16(vm, BIOS_COLUMNS);
    uint8_t rows_minus = read_mem8(vm, BIOS_ROWS_MINUS_ONE);
    uint8_t new_row = row;
    uint8_t new_col = col;
    switch (ch)
    {
    case 0x0d:      // carriage return
        new_col = 0;
        break;
    case 0x0a:      // line feed
        new_row = row + 1;
        break;
    case 0x08:      // backspace
        if (col > 0) new_col = col - 1;
        break;
    default:
        if (mode == 0x03)
            write_mem8(vm, TEXT_VIDEO_AREA + (row * screen_width + col) * 2, ch);
        new_col = col + 1;
        if (new_col >= screen_width)
        {
            new_col = 0;
            new_row = row + 1;
        }
        break;
    }
    if (new_row > rows_minus)
    {
        new_row = rows_minus;
        int last_line = TEXT_VIDEO_AREA + rows_minus * screen_width * 2;
        for (int i = 0; i < rows_minus * screen_width * 2; i++)
            write_mem8(vm, TEXT_VIDEO_AREA + i, read_mem8(vm, TEXT_VIDEO_AREA + screen_width * 2 + i));
        for (int i = last_line; i < last_line + screen_width * 2; i += 2)
        {
            write_mem8(vm, i, 0x20);
            write_mem8(vm, i + 1, 0x07);
        }
    }
    set_cursor(vm, new_col, new_row);
}

void clear_graphic(VM* vm)
{
    set_graphic(vm, 0);
}

void clear_text_video(VM* vm)
{
    set_text_video(vm, 0x20, 0x07);
}

// fix - use internal funcs to improve
void set_mode(VM* vm, uint8_t mode)
{
    if (mode == 0x3)
    {
        // text
        write_mem8(vm, BIOS_ROWS_MINUS_ONE, 0x18);
        set_cursor(vm, 0, 0);
        write_mem8(vm, BIOS_VIDEO_MODE, 0x3);
        write_mem16(vm, BIOS_COLUMNS, 0x50);
        clear_text_video(vm);
    }
    else if (mode == 0x13)
    {
        // graphic
        write_mem8(vm, BIOS_ROWS_MINUS_ONE, 0x18);
        set_cursor(vm, 0, 0);
        write_mem8(vm, BIOS_VIDEO_MODE, 0x13);
        write_mem16(vm, BIOS_COLUMNS, 0x28);
        clear_graphic(vm);
    }
}

uint8_t get_mode(VM* vm)
{
    return read_mem8(vm, BIOS_VIDEO_MODE);
}

void set_cursor(VM* vm, uint8_t col, uint8_t row)
{
    write_mem8(vm, BIOS_CURSOR, col);  // column
    write_mem8(vm, BIOS_CURSOR + 1, row);  // row
}
void get_cursor(VM* vm, uint8_t *col, uint8_t *row)
{
    *col = read_mem8(vm, BIOS_CURSOR);  // column
    *row = read_mem8(vm, BIOS_CURSOR + 1);  // row
}

void set_pixel(VM* vm, uint16_t x, uint16_t y, uint8_t val)
{
    if (x < GRAPHIC_WIDTH && y < GRAPHIC_HEIGHT && get_mode(vm) == 0x13)
    {
        write_mem8(vm, GRAPHIC_AREA + y*GRAPHIC_WIDTH + x, val);
    }
}
int get_pixel(VM* vm, uint16_t x, uint16_t y)
{
    if (x < GRAPHIC_WIDTH && y < GRAPHIC_HEIGHT)
    {
        return read_mem8(vm, GRAPHIC_AREA + y*GRAPHIC_WIDTH + x);
    }
    return -1;
}

uint8_t get_columns(VM* vm)
{
    return read_mem8(vm, BIOS_COLUMNS);
}
