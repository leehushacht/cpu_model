#ifndef SCREEN_H
#define SCREEN_H
#include "vm.h"

void set_mode(VM* vm, uint8_t mode);
void clear_text_video(VM* vm);
void clear_graphic(VM* vm);
void print_char(VM* vm, uint8_t ch);

uint8_t get_mode(VM* vm);
void set_cursor(VM* vm, uint8_t col, uint8_t row);
void get_cursor(VM* vm, uint8_t *col, uint8_t *row);
void set_pixel(VM* vm, uint16_t x, uint16_t y, uint8_t val);
int get_pixel(VM* vm, uint16_t x, uint16_t y);
uint8_t get_columns(VM* vm);

#endif