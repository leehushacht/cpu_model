#include "vm.h"
#include "opcodes.h"
#include "hal.h"


static void print_state(const VM* vm){
    printf("AX=%04X BX=%04X CX=%04X DX=%04X SI=%04X DI=%04X SP=%04X ES=%04X IP=%04X\n",
        read_r16(vm, AX), read_r16(vm, BX), read_r16(vm, CX), read_r16(vm, DX),
        read_r16(vm, SI), read_r16(vm, DI), read_r16(vm, SP),
        read_sreg16(vm, ES), read_ip16(vm));
    printf("CF=%d ZF=%d SF=%d OF=%d PF=%d AF=%d\n",
        get_flag(vm, FLAG_CF), get_flag(vm, FLAG_ZF), get_flag(vm, FLAG_SF),
        get_flag(vm, FLAG_OF), get_flag(vm, FLAG_PF), get_flag(vm, FLAG_AF));
    printf("mem[DS:0200]=%04X mem[DS:0202]=%02X mem[DS:0210]=%04X mem[DS:0220]=%04X\n",
        read_mem16(vm, get_address(read_sreg16(vm, DS), 0x0200)),
        read_mem8 (vm, get_address(read_sreg16(vm, DS), 0x0202)),
        read_mem16(vm, get_address(read_sreg16(vm, DS), 0x0210)),
        read_mem16(vm, get_address(read_sreg16(vm, DS), 0x0220)));
}

static void test(VM* vm){
    uint32_t a = get_address(read_sreg16(vm, CS), read_ip16(vm));
    const uint8_t prog[] = {
        // --- MOV immediate ---
        0xB8, 0x34, 0x12,       // MOV AX, 1234h
        0xB3, 0xAB,             // MOV BL, ABh
        0xB7, 0xCD,             // MOV BH, CDh          -> BX=CDAB
        // --- stack ---
        0x50,                   // PUSH AX
        0x53,                   // PUSH BX
        0x58,                   // POP AX               -> AX=CDAB
        0x5B,                   // POP BX               -> BX=1234   (swapped)
        // --- MOV reg<->reg via ModRM ---
        0x89, 0xC1,             // MOV CX, AX           -> CX=CDAB
        0x8A, 0xE1,             // MOV AH, CL           -> AX=ABAB
        // --- MOV to/from memory, several addressing modes ---
        0xBE, 0x00, 0x02,       // MOV SI, 0200h
        0x89, 0x0C,             // MOV [SI], CX         -> mem[0200]=CDAB
        0xC6, 0x44, 0x02, 0x5A, // MOV byte [SI+2], 5Ah -> mem[0202]=5A
        0x8B, 0x14,             // MOV DX, [SI]         -> DX=CDAB
        0xBF, 0x10, 0x02,       // MOV DI, 0210h
        0xC7, 0x05, 0x00, 0x10, // MOV word [DI], 1000h -> mem[0210]=1000
        // --- segment register moves ---
        0x8C, 0xD8,             // MOV AX, DS           -> AX=0700
        0x8E, 0xC0,             // MOV ES, AX
        0xB8, 0x00, 0x08,       // MOV AX, 0800h
        0x8E, 0xC0,             // MOV ES, AX           -> ES=0800
        // --- ALU, register forms, all 8 ops ---
        0xB8, 0xFF, 0x7F,       // MOV AX, 7FFFh
        0xBB, 0x01, 0x00,       // MOV BX, 0001h
        0x01, 0xD8,             // ADD AX, BX           -> AX=8000, OF=1
        0xB8, 0xFF, 0xFF,       // MOV AX, FFFFh
        0x01, 0xD8,             // ADD AX, BX           -> AX=0000, CF=1
        0xB9, 0x05, 0x00,       // MOV CX, 0005h
        0x11, 0xD9,             // ADC CX, BX           -> CX=5+1+CF = 0007
        0x29, 0xD9,             // SUB CX, BX           -> CX=0006
        0x39, 0xD9,             // CMP CX, BX           -> flags only
        0xB8, 0x00, 0x00,       // MOV AX, 0
        0x29, 0xD8,             // SUB AX, BX           -> AX=FFFF, CF=1
        0x19, 0xD9,             // SBB CX, BX           -> CX=6-1-CF = 0004
        0x21, 0xD9,             // AND CX, BX           -> CX=0000
        0x09, 0xD9,             // OR  CX, BX           -> CX=0001
        0x31, 0xC9,             // XOR CX, CX           -> CX=0000
        // --- ALU, reg <- memory and accumulator-immediate forms ---
        0x03, 0x0C,             // ADD CX, [SI]         -> CX=CDAB
        0x2B, 0x0D,             // SUB CX, [DI]         -> CX=BDAB
        0x04, 0x10,             // ADD AL, 10h          -> AL=0F
        0x2D, 0x0F, 0x00,       // SUB AX, 000Fh        -> AX=FF00
        // --- group 80/81/83 ---
        0x80, 0xC3, 0x10,       // ADD BL, 10h          -> BL=11
        0x81, 0xEB, 0x10, 0x00, // SUB BX, 0010h        -> BX=0001
        0x83, 0xC3, 0xFE,       // ADD BX, -2           -> BX=FFFF
        0x83, 0x3C, 0x00,       // CMP word [SI], 0
        0x80, 0x7C, 0x02, 0x5A, // CMP byte [SI+2], 5Ah -> ZF=1
        // --- INC/DEC, all three forms, CF preserved ---
        0x05, 0xFF, 0x00,       // ADD AX, 00FFh        -> AX=FFFF, CF=0
        0x40,                   // INC AX               -> AX=0000, CF still 0
        0x4B,                   // DEC BX               -> BX=FFFE
        0xFE, 0xC3,             // INC BL               -> BX=FFFF
        0xFF, 0x0D,             // DEC word [DI]        -> mem[0210]=0FFF
        0xFE, 0x4C, 0x02,       // DEC byte [SI+2]      -> mem[0202]=59
        // --- segment push/pop: swap DS<->ES, observe, swap back ---
        0x1E, 0x06, 0x1F, 0x07, // PUSH DS; PUSH ES; POP DS; POP ES -> DS=0800, ES=0700
        0x8C, 0xC0,             // MOV AX, ES           -> AX=0700
        0x8C, 0xDB,             // MOV BX, DS           -> BX=0800
        0x1E, 0x06, 0x1F, 0x07, // swap back            -> DS=0700, ES=0800
        // --- PUSH/POP r/m16 through memory ---
        0xFF, 0x34,             // PUSH word [SI]       (CDAB)
        0x8F, 0x05,             // POP  word [DI]       -> mem[0210]=CDAB
        // --- PUSHF/POPF round trip ---
        0x31, 0xC9,             // XOR CX, CX           -> ZF=1 PF=1
        0x9C,                   // PUSHF                (saves ZF=1 PF=1)
        0x40,                   // INC AX               -> AX=0701, ZF=0 PF=0
        0xA3, 0x20, 0x02,       // MOV [0220], AX       -> mem[0220]=0701
        0xA1, 0x00, 0x02,       // MOV AX, [0200]       -> AX=CDAB
        0x9D,                   // POPF                 -> ZF=1 PF=1 restored
        // --- LOOP: sum 1..5 into DX ---
        0x31, 0xD2,             // XOR DX, DX
        0xB9, 0x05, 0x00,       // MOV CX, 5
        0x01, 0xCA,             // top:  ADD DX, CX
        0xE2, 0xFC,             // LOOP top              -> DX=000F, CX=0
        // --- Jcc taken / not taken ---
        0x83, 0xFA, 0x0F,       // CMP DX, 0Fh
        0x75, 0x04,             // JNE fail              (not taken)
        0xB0, 0x01,             // MOV AL, 1
        0xEB, 0x02,             // JMP skip
        0xB0, 0x00,             // fail: MOV AL, 0       (skipped)
        // --- JCXZ ---
        0xE3, 0x02,             // skip: JCXZ cxz        (taken, CX=0)
        0xB4, 0xFF,             // MOV AH, FFh           (skipped)
        0xB4, 0x02,             // cxz: MOV AH, 2        -> AX=0201
        // --- LOOPNZ: count BX up until BX==3 ---
        0xB9, 0x0A, 0x00,       // MOV CX, 10
        0xBB, 0x00, 0x00,       // MOV BX, 0
        0x43,                   // lz:  INC BX
        0x83, 0xFB, 0x03,       // CMP BX, 3
        0xE0, 0xFA,             // LOOPNZ lz             -> BX=0003, CX=0007
        // --- CALL near + RET imm16 ---
        0xBE, 0x11, 0x11,       // MOV SI, 1111h
        0x56,                   // PUSH SI
        0xE8, 0x02, 0x00,       // CALL sub1
        0xEB, 0x06,             // JMP after1
        0xBE, 0x22, 0x22,       // sub1: MOV SI, 2222h
        0xC2, 0x02, 0x00,       // RET 2                 -> SP back to FF00
        // --- indirect JMP via register, indirect CALL via memory ---
        0xBF, 0xCA, 0x01,       // after1: MOV DI, 01CAh (tgt)
        0xFF, 0xE7,             // JMP DI
        0xB6, 0xEE,             // MOV DH, EEh           (skipped)
        0xC7, 0x06, 0x00, 0x03, 0xD7, 0x01, // tgt: MOV word [0300], 01D7h (sub2)
        0xBF, 0x00, 0x03,       // MOV DI, 0300h
        0xFF, 0x15,             // CALL word [DI]
        0xEB, 0x03,             // JMP after2
        0xB6, 0x55,             // sub2: MOV DH, 55h     -> DX=550F
        0xC3,                   // RET
        // --- far CALL / RETF ---
        0x9A, 0xE1, 0x01, 0x00, 0x07, // after2: CALL 0700:01E1 (farsub)
        0xEB, 0x03,             // JMP after3
        0xB1, 0x77,             // farsub: MOV CL, 77h   -> CX=0077
        0xCB,                   // RETF
        // --- far JMP via memory pointer ---
        0xC7, 0x06, 0x10, 0x03, 0xF7, 0x01, // after3: MOV word [0310], 01F7h (farend)
        0xC7, 0x06, 0x12, 0x03, 0x00, 0x07, // MOV word [0312], 0700h
        0xBF, 0x10, 0x03,       // MOV DI, 0310h
        0xFF, 0x2D,             // JMP far [DI]
        0xB5, 0xEE,             // MOV CH, EEh           (skipped)
        0xE9, 0x06, 0x02        // farend: JMP 0400h     -> continue in int_test
    };
    for (size_t i = 0; i < sizeof prog; i++) write_mem8(vm, a + i, prog[i]);

    // ---------- interrupt test: program at CS:0400 ----------
    const uint8_t int_test[] = {
        0xB8, 0x01, 0x02,       // MOV AX, 0201h
        0x50,                   // PUSH AX
        0x9D,                   // POPF                  -> IF=1 CF=1
        0xBB, 0x05, 0x00,       // MOV BX, 0005h
        0xCD, 0x21,             // INT 21h               -> jumps to the ISR at 0700:0500
        0xB9, 0x34, 0x12,       // MOV CX, 1234h         (runs only if IRET came back here)
        0x9C,                   // PUSHF
        0x5A,                   // POP DX                -> DX = flags after IRET
        0xF4                    // HLT
    };
    // ---------- the interrupt handler (ISR) at CS:0500 ----------
    const uint8_t isr[] = {
        0x9C,                   // PUSHF
        0x5F,                   // POP DI                -> DI = flags inside the ISR
        0x43,                   // INC BX                -> BX = 0006
        0x31, 0xF6,             // XOR SI, SI            -> trashes the flags (CF=0 ZF=1)
        0xCF                    // IRET
    };
    uint32_t t = get_address(read_sreg16(vm, CS), 0x0400);
    for (size_t i = 0; i < sizeof int_test; i++) write_mem8(vm, t + i, int_test[i]);
    uint32_t h = get_address(read_sreg16(vm, CS), 0x0500);
    for (size_t i = 0; i < sizeof isr; i++) write_mem8(vm, h + i, isr[i]);

    // ---------- the loader fills IVT slot 21h: 0700:0500 ----------
    write_mem16(vm, 0x21 * 4,     0x0500);  // offset
    write_mem16(vm, 0x21 * 4 + 2, 0x0700);  // segment
}

static void prepare_segment_registers(VM* vm)
{
    // for now this value -> later give more thought on it
    write_sreg16(vm, CS, 0x0700);
    write_sreg16(vm, DS, 0x0700);
    write_sreg16(vm, ES, 0x0700);
    write_sreg16(vm, SS, 0x0700);
    write_r16(vm, SP, 0xFF00); // for test
    write_ip16(vm, 0x0100);

}
static void prepare_flags(VM* vm)
{
    set_flag(vm, FLAG_CF, false);
    set_flag(vm, FLAG_SF, false);
    set_flag(vm, FLAG_ZF, false);
    set_flag(vm, FLAG_AF, false);
    set_flag(vm, FLAG_DF, false);
    set_flag(vm, FLAG_IF, false);
    set_flag(vm, FLAG_OF, false);
    set_flag(vm, FLAG_PF, false);

}
int main(void){
    
    init_opcode_table();
    VM* vm = create_vm();
    if(vm == NULL) return -1;
    turn_on_vm(vm);

    
    // prapare all machine here!
    prepare_segment_registers(vm);
    prepare_flags(vm);
    print_state(vm);
    test(vm);

    run_vm(vm);
    print_state(vm);
    destroy_vm(vm);
    return 0;
}
