// memory placement
// ---------------

#define IVT_AREA 0x00000
#define BIOS_DATA_AREA 0x00400
#define FREE_AREA 0x00500
#define GRAPHIC_AREA 0xa0000
#define TEXT_VIDEO_AREA 0xb8000
#define SYSTEM_AREA 0xf0000

// area sizes
#define IVT_SIZE 0x400
#define BIOS_DATA_SIZE 0x100
#define FREE_SIZE 0x9fb00
#define GRAPHIC_SIZE 0x10000
#define TEXT_VIDEO_SIZE 0x8000
#define SYSTEM_SIZE 0x10000

// internal areas 
// --------------

// ivt
#define IVT_ENTRIES 0x100

// system
#define IRET_STUBS SYSTEM_AREA

// bios
#define BIOS_VIDEO_MODE (BIOS_DATA_AREA + 0x49)
#define BIOS_COLUMNS (BIOS_DATA_AREA + 0x4a)
#define BIOS_CURSOR (BIOS_DATA_AREA + 0x50)
#define BIOS_ROWS_MINUS_ONE (BIOS_DATA_AREA + 0x84)

// graphic
#define GRAPHIC_WIDTH 0x140
#define GRAPHIC_HEIGHT 0xc8

// program
#define STACK_START 0xff00
#define PROGRAM_START 0x0100
#define PROGRAM_SEGMENT (FREE_AREA >> 4)
#define MAX_PROGRAM_SIZE (STACK_START - PROGRAM_START)
