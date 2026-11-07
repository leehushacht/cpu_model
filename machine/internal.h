#ifndef INTERNAL_H
#define INTERNAL_H

#include "vm.h" 

// mapping
#define MEMORY_SIZE (1024*1024) 


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