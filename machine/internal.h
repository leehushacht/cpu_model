#ifndef INTERNAL_H
#define INTERNAL_H

#include "vm.h" 

// mapping
#define MEMORY_SIZE (1024*1024) 
#define IVT_AREA 0x00000
#define BIOS_DATA_AREA 0x00400
#define PROGRAM_AREA 0x00500
#define GRAPHIC_AREA 0xa0000
#define TEXT_VIDEO_AREA 0xb8000 // till bffff
#define SYSTEM_AREA 0xf0000

// sizes
#define IVT_SIZE 0x400
#define BIOS_DATA_SIZE 0x100
#define PROGRAM_SIZE 0x9fb00
#define GRAPHIC_SIZE 0x10000
#define TEXT_VIDEO_SIZE 0x8000
#define SYSTEM_SIZE 0x10000



struct VM {
    uint8_t memory[MEMORY_SIZE]; 
    uint16_t reg[8]; 
    uint16_t seg_reg[4]; 
    uint16_t flags_reg; 
    uint16_t ip;
    uint16_t initial_ip_op;
    bool is_running;
    bool is_halted;
};

#endif