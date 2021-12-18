#ifndef __H6502_H__
#define __H6502_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include "macros.h"

// Ref
// > Opcodes : https://web.archive.org/web/20210426072206/http://www.obelisk.me.uk/6502/index.html
// > Instruction cycles : https://www.nesdev.com/6502_cpu.txt
// > Addressing Modes : https://www.c64-wiki.com/wiki/Addressing_mode
// > Hex Calculator : https://www.calculator.net/hex-calculator.html?number1=80&c2op=%2B&number2=ff&calctype=op&x=85&y=13
//              https://keisan.casio.com/exec/system/14495408631029
// The first 256 Bytes of the memory map (0-255 or $0000-$00FF) are called zeropage (Page 0)
//      look up [RAM Table]

//#define DEBUG_PRINT 1
#ifndef DEBUG_PRINT
#define DEBUG_PRINT 0
#endif

typedef uint8_t u8;   // byte [0, 255]
typedef uint16_t u16; // word [0, 65,535]
typedef uint32_t u32; // [0, 4,294,967,295]
typedef int32_t s32;

#define ASSERT(Condition, Text) \
    {                           \
        if (!(Condition))       \
        {                       \
            throw -1;           \
        }                       \
    }

#define MAX_MEM 65536 // 1024 * 64 = 65536

typedef struct Memory
{
    u8 data[MAX_MEM];
} Memory;

typedef struct CPU
{
    // Registers
    u16 program_counter; // Program Counter
    u8 stack_pointer;    // Stack Pointer

    u8 accumulator; // A
    u8 index_reg_X; // X
    u8 index_reg_Y; // Y

    // Processor Status
    u8 C : 1; // Carry flag
    u8 Z : 1; // Zero flag
    u8 I : 1; // Interrupt flag
    u8 D : 1; // Decimal flag
    u8 B : 1; // Break flag
    u8 V : 1; // Overflow flag
    u8 N : 1; // Negative flag
} CPU;

// opcodes
typedef enum
{
    // LDA - Load Accumulator
    INS_LDA_IM = 0xA9,
    INS_LDA_ZP = 0xA5,
    INS_LDA_ZP_X = 0xB5,
    INS_LDA_ABS = 0xAD,
    INS_LDA_ABS_X = 0xBD,
    INS_LDA_ABS_Y = 0xB9,
    INS_LDA_IND_X = 0xA1,
    INS_LDA_IND_Y = 0xB1,

    // LDX - Load X Register
    INS_LDX_IM = 0xA2,
    INS_LDX_ZP = 0xA6,
    INS_LDX_ZP_Y = 0xB6,
    INS_LDX_ABS = 0xAE,
    INS_LDX_ABS_Y = 0xBE,

    // LDY - Load Y Register
    INS_LDY_IM = 0xA0,
    INS_LDY_ZP = 0xA4,
    INS_LDY_ZP_X = 0xB4,
    INS_LDY_ABS = 0xAC,
    INS_LDY_ABS_X = 0xBC,

    // STA - Store Accumulator
    INS_STA_ZP = 0x85,
    INS_STA_ZP_X = 0x95,
    INS_STA_ABS = 0x8D,
    INS_STA_ABS_X = 0x9D,
    INS_STA_ABS_Y = 0x99,
    INS_STA_IND_X = 0x81,
    INS_STA_IND_Y = 0x91,

    // STX - Store X Register
    INS_STX_ZP = 0x86,
    INS_STX_ZP_Y = 0x96,
    INS_STX_ABS = 0x8E,

    // STY - Store Y Register
    INS_STY_ZP = 0x84,
    INS_STY_ZP_X = 0x94,
    INS_STY_ABS = 0x8C,

    // JSR - Jump to Subroutine
    INS_JSR = 0x20

} Opcode;

//#define INS_LDA_IM 0xA9
//#define INS_LDA_ZP 0xA5
//#define INS_LDA_ZPX 0xB5
//#define INS_LDA_ABS 0xAD
//#define INS_LDA_ABS_X 0xBD
//#define INS_LDA_ABS_Y 0xB9
//#define INS_LDA_IND_X 0xA1
//#define INS_LDA_IND_Y 0xB1

//#define INS_JSR 0x20

static Memory mem;
static CPU cpu;

void Initialise_Memory(Memory *mem)
{
    for (size_t i = 0; i < MAX_MEM; i++)
    {
        mem->data[i] = 0;
    }
}

void Reset_CPU(CPU *cpu, Memory *mem)
{
    cpu->program_counter = 0xFFFC; // The low and high 8-bit halves of the register are called PCL and PCH
    cpu->stack_pointer = 0x00FF;

    cpu->accumulator = 0;
    cpu->index_reg_X = 0;
    cpu->index_reg_Y = 0;

    cpu->C = 0;
    cpu->Z = 0;
    cpu->I = 0;
    cpu->D = 0;
    cpu->B = 0;
    cpu->V = 0;
    cpu->N = 0;

    Initialise_Memory(mem);
}

// 1 Cycle
u8 Fetch_Byte(s32 *cycles, const Memory *mem)
{
    assert(cpu.program_counter < MAX_MEM);

    u8 data = mem->data[cpu.program_counter];
    cpu.program_counter++;
    (*cycles) -= 1;
    return data;
}

// 2 Cycles
u16 Fetch_Word(s32 *cycles, const Memory *mem)
{
    assert(cpu.program_counter < MAX_MEM);

    // 6502 is little endian
    u16 data = mem->data[cpu.program_counter];
    cpu.program_counter++;

    data |= (mem->data[cpu.program_counter] << 8);
    cpu.program_counter++;

    (*cycles) -= 2;
    return data;
}

// 1 cycle
u8 Write_Byte(s32 *cycles, Memory *mem, u8 data, u16 address)
{
    assert(address < MAX_MEM);

    mem->data[address] = data;
    (*cycles) -= 1;
}

// 1 Cycle
u8 Read_Byte(s32 *cycles, u16 address, const Memory *mem)
{
    assert(address < MAX_MEM);

    u8 data = mem->data[address];
    (*cycles) -= 1;
    return data;
}

// 2 Cycles
u16 Read_Word(s32 *cycles, u16 address, const Memory *mem)
{
    assert(address < MAX_MEM);

    const u8 low_byte = Read_Byte(cycles, address, mem);
    const u8 high_byte = Read_Byte(cycles, address + 1, mem);

    return low_byte | (high_byte << 8);
}

// 2 Cycles
void Write_Word(s32 *cycles, u16 data, u32 address)
{
    // move to the next address and set it equal to
    mem.data[address + 1] = (data >> 8); // 1 cycle
    (*cycles) -= 1;
    mem.data[address] = (data & 0xFF); // 1 cycle
    (*cycles) -= 1;
}

void Push_Word_To_Stack(s32 *cycles, Memory *mem, u16 value)
{
    // cycles, mem, data, address
    Write_Byte(cycles, mem, value >> 8, 0x100 | cpu.stack_pointer);
    cpu.stack_pointer--;
    Write_Byte(cycles, mem, value & 0xFF, 0x100 | cpu.stack_pointer);
    cpu.stack_pointer--;
}

/** Push the PC-1 onto the stack */
void Push_PC_Minus_One_To_Stack(s32 *cycles, Memory *mem)
{
    Push_Word_To_Stack(cycles, mem, cpu.program_counter - 1);
}

/** Push the PC+1 onto the stack */
void Push_PC_Plus_One_To_Stack(s32 *cycles, Memory *mem)
{
    Push_Word_To_Stack(cycles, mem, cpu.program_counter + 1);
}

/** Push the PC onto the stack */
void Push_PC_To_Stack(s32 *cycles, Memory *mem)
{
    Push_Word_To_Stack(cycles, mem, cpu.program_counter);
}

// A, X or Y Register
void Load_Register_Set_Status(u8 reg)
{
    cpu.Z = (reg == 0);
    cpu.N = (reg & 0b10000000) > 0;
}

// Addressing mode - Zero Page (1 cycle)
u16 Address_Zero_Page(s32 *cycles, const Memory *mem)
{
    u8 zero_page_address = Fetch_Byte(cycles, mem);
    return zero_page_address;
}

// Addressing mode - Zero Page (2 cycles)
u16 Address_Zero_Page_X(s32 *cycles, const Memory *mem)
{
    u8 zero_page_address = Fetch_Byte(cycles, mem);
    zero_page_address += cpu.index_reg_X;
    (*cycles) -= 1;

    return zero_page_address;
}

// Addressing mode - Zero Page (2 cycles)
u16 Address_Zero_Page_Y(s32 *cycles, const Memory *mem)
{
    u8 zero_page_address = Fetch_Byte(cycles, mem);
    zero_page_address += cpu.index_reg_Y;
    (*cycles) -= 1;

    return zero_page_address;
}

// Addressing mode - Absolute (2 cycles)
u16 Address_Absolute(s32 *cycles, const Memory *mem)
{
    u16 absolute_address = Fetch_Word(cycles, mem);
    return absolute_address;
}

// Addressing mode - Absolute X (2/3 cycles)
u16 Address_Absolute_X(s32 *cycles, const Memory *mem)
{
    const u16 absolute_address = Fetch_Word(cycles, mem);
    const u16 absolute_address_x = absolute_address + cpu.index_reg_X;
    const int crossed_page_boundary = (absolute_address ^ absolute_address_x) >> 8;
    if (crossed_page_boundary)
        (*cycles) -= 1;

    return absolute_address_x;
}

// Addressing mode - Absolute Y (2/3 cycles)
u16 Address_Absolute_Y(s32 *cycles, const Memory *mem)
{
    const u16 absolute_address = Fetch_Word(cycles, mem);
    const u16 absolute_address_y = absolute_address + cpu.index_reg_Y;
    const int crossed_page_boundary = (absolute_address ^ absolute_address_y) >> 8;
    if (crossed_page_boundary)
        (*cycles) -= 1;

    return absolute_address_y;
}

// Addressing mode - Indirect X (4 cycles)
u16 Address_Indirect_X(s32 *cycles, const Memory *mem)
{
    u8 zero_page_address = Fetch_Byte(cycles, mem);
    zero_page_address += cpu.index_reg_X;
    (*cycles) -= 1;
    const u16 effective_address = Read_Word(cycles, zero_page_address, mem);
    return effective_address;
}

// Addressing mode - Indirect Y (3/4 cycles)
u16 Address_Indirect_Y(s32 *cycles, const Memory *mem)
{
    const u8 zero_page_address = Fetch_Byte(cycles, mem);
    const u16 effective_address = Read_Word(cycles, zero_page_address, mem);
    const u16 effective_address_y = effective_address + cpu.index_reg_Y;

    const int crossed_page_boundary = (effective_address ^ effective_address_y) >> 8;
    if (crossed_page_boundary)
        (*cycles) -= 1;

    return effective_address_y;
}

// Load a value at an 'address' into a given 'register' (1 cycle)
void Load_Register(s32 *cycles, u8 *reg, const u16 address, const Memory *mem)
{
    (*reg) = Read_Byte(cycles, address, mem);
    Load_Register_Set_Status((*reg));
}

// execute "num_cycles" the instruction in memory
s32 Execute(s32 num_cycles, Memory *mem)
{
    const s32 CyclesRequested = num_cycles;
    while (num_cycles > 0)
    {
        const u8 instruction = Fetch_Byte(&num_cycles, mem); // -1 cycle
        print_int(instruction);
        switch (instruction)
        {
            // LDA - Load Accumulator
        case INS_LDA_IM: // 2 Cycles
        {
            cpu.accumulator = Fetch_Byte(&num_cycles, mem); // -1 cycle
            Load_Register_Set_Status(cpu.accumulator);

            break;
        }
        case INS_LDA_ZP: // 3 Cycles
        {
            const u8 zero_page_address = Address_Zero_Page(&num_cycles, mem);
            Load_Register(&num_cycles, &cpu.accumulator, zero_page_address, mem);
            break;
        }
        case INS_LDA_ZP_X: // 4 Cycles
        {
            const u16 zero_page_x_address = Address_Zero_Page_X(&num_cycles, mem);
            Load_Register(&num_cycles, &cpu.accumulator, zero_page_x_address, mem);

            break;
        }
        case INS_LDA_ABS: // 4 Cycles
        {
            //  1    PC     R  fetch opcode, increment PC
            //  2    PC     R  fetch low byte of address, increment PC
            //  3    PC     R  fetch high byte of address, increment PC
            //  4  address  R  read from effective address
            const u16 absolute_address = Address_Absolute(&num_cycles, mem);
            Load_Register(&num_cycles, &cpu.accumulator, absolute_address, mem);
            break;
        }
        case INS_LDA_ABS_X: // 4 Cycles (+1 if page crossed)
        {
            const u16 absolute_address = Address_Absolute_X(&num_cycles, mem); // 2 cycles
            Load_Register(&num_cycles, &cpu.accumulator, absolute_address, mem);
            break;
        }
        case INS_LDA_ABS_Y: // 4 Cycles (+1 if page crossed)
        {
            const u16 absolute_address = Address_Absolute_Y(&num_cycles, mem); // 2 cycles
            Load_Register(&num_cycles, &cpu.accumulator, absolute_address, mem);
            break;
        }
        case INS_LDA_IND_X: // 6 Cycles
        {
            //  1      PC       R  fetch opcode, increment PC
            //  2      PC       R  fetch pointer address, increment PC
            //  3    pointer    R  read from the address, add X to it
            //  4   pointer+X   R  fetch effective address low
            //  5  pointer+X+1  R  fetch effective address high
            //  6    address    R  read from effective address

            const u16 effective_address = Address_Indirect_X(&num_cycles, mem);
            Load_Register(&num_cycles, &cpu.accumulator, effective_address, mem);
            break;
        }
        case INS_LDA_IND_Y: // 5 Cycles (+1 if page crossed)
        {
            const u16 effective_address = Address_Indirect_Y(&num_cycles, mem);
            Load_Register(&num_cycles, &cpu.accumulator, effective_address, mem);
            break;
        }
            // LDX - Load X Register
        case INS_LDX_IM:
        {
            cpu.index_reg_X = Fetch_Byte(&num_cycles, mem); // -1 cycle
            Load_Register_Set_Status(cpu.index_reg_X);
            break;
        }
        case INS_LDX_ZP:
        {
            const u8 zero_page_address = Address_Zero_Page(&num_cycles, mem);
            Load_Register(&num_cycles, &cpu.index_reg_X, zero_page_address, mem);
            break;
        }
        case INS_LDX_ZP_Y:
        {
            const u16 zero_page_y_address = Address_Zero_Page_Y(&num_cycles, mem);
            Load_Register(&num_cycles, &cpu.index_reg_X, zero_page_y_address, mem);
            break;
        }
        case INS_LDX_ABS:
        {
            const u16 absolute_address = Address_Absolute(&num_cycles, mem); // 2 cycles
            Load_Register(&num_cycles, &cpu.index_reg_X, absolute_address, mem);
            break;
        }
        case INS_LDX_ABS_Y:
        {
            const u16 absolute_address = Address_Absolute_Y(&num_cycles, mem); // 2 cycles
            Load_Register(&num_cycles, &cpu.index_reg_X, absolute_address, mem);
            break;
        }
            // LDY - Load Y Register
        case INS_LDY_IM:
        {
            cpu.index_reg_Y = Fetch_Byte(&num_cycles, mem); // -1 cycle
            Load_Register_Set_Status(cpu.index_reg_Y);
            break;
        }
        case INS_LDY_ZP:
        {
            const u8 zero_page_address = Address_Zero_Page(&num_cycles, mem);
            Load_Register(&num_cycles, &cpu.index_reg_Y, zero_page_address, mem);
            break;
        }
        case INS_LDY_ZP_X:
        {
            const u16 zero_page_x_address = Address_Zero_Page_X(&num_cycles, mem);
            Load_Register(&num_cycles, &cpu.index_reg_Y, zero_page_x_address, mem);
            break;
        }
        case INS_LDY_ABS:
        {
            const u16 absolute_address = Address_Absolute(&num_cycles, mem); // 2 cycles
            Load_Register(&num_cycles, &cpu.index_reg_Y, absolute_address, mem);
            break;
        }
        case INS_LDY_ABS_X:
        {
            const u16 absolute_address = Address_Absolute_X(&num_cycles, mem); // 2 cycles
            Load_Register(&num_cycles, &cpu.index_reg_Y, absolute_address, mem);
            break;
        }
            // STA - Store Accumulator
        case INS_STA_ZP:
        {
            // STore Accumulator _ ZP value
            // 1    PC     R  fetch opcode, increment PC
            // 2    PC     R  fetch address, increment PC
            // 3  address  W  write register to effective address
            const u16 effective_address = Address_Zero_Page(&num_cycles, mem); // 1 cycle
            Write_Byte(&num_cycles, mem, cpu.accumulator, effective_address);
            break;
        }
        case INS_STA_ZP_X:
        {
            const u16 effective_address = Address_Zero_Page_X(&num_cycles, mem); // 2 cycles
            Write_Byte(&num_cycles, mem, cpu.accumulator, effective_address);
            break;
        }
        case INS_STA_ABS:
        {
            const u16 effective_address = Address_Absolute(&num_cycles, mem); // 2 cycles
            Write_Byte(&num_cycles, mem, cpu.accumulator, effective_address);
            break;
        }
        case INS_STA_ABS_X:
        {
            const u16 effective_address = Address_Absolute_X(&num_cycles, mem); // 2 cycles
            Write_Byte(&num_cycles, mem, cpu.accumulator, effective_address);
            break;
        }
        case INS_STA_ABS_Y:
        {
            const u16 effective_address = Address_Absolute_Y(&num_cycles, mem); // 2 cycles
            Write_Byte(&num_cycles, mem, cpu.accumulator, effective_address);
            break;
        }
        case INS_STA_IND_X:
        {
            const u16 effective_address = Address_Indirect_X(&num_cycles, mem); // 4 cycles
            Write_Byte(&num_cycles, mem, cpu.accumulator, effective_address);
            break;
        }
        case INS_STA_IND_Y:
        {
            const u16 effective_address = Address_Indirect_Y(&num_cycles, mem); // 2 cycles
            Write_Byte(&num_cycles, mem, cpu.accumulator, effective_address);
            break;
        }
            // STX - Store X Register
        case INS_STX_ZP:
        {
            const u16 effective_address = Address_Zero_Page(&num_cycles, mem); // 1 cycles
            Write_Byte(&num_cycles, mem, cpu.index_reg_X, effective_address);
            break;
        }
        case INS_STX_ZP_Y:
        {
            const u16 effective_address = Address_Zero_Page_Y(&num_cycles, mem); // 2 cycles
            Write_Byte(&num_cycles, mem, cpu.index_reg_X, effective_address);
            break;
        }
        case INS_STX_ABS:
        {
            const u16 effective_address = Address_Absolute(&num_cycles, mem); // 2 cycles
            Write_Byte(&num_cycles, mem, cpu.index_reg_X, effective_address);
            break;
        }
            // STY - Store Y Register
        case INS_STY_ZP:
        {
            const u16 effective_address = Address_Zero_Page(&num_cycles, mem); // 1 cycles
            Write_Byte(&num_cycles, mem, cpu.index_reg_Y, effective_address);
            break;
        }
        case INS_STY_ZP_X:
        {
            const u16 effective_address = Address_Zero_Page_X(&num_cycles, mem); // 2 cycles
            Write_Byte(&num_cycles, mem, cpu.index_reg_Y, effective_address);
            break;
        }
        case INS_STY_ABS:
        {
            const u16 effective_address = Address_Absolute(&num_cycles, mem); // 2 cycles
            Write_Byte(&num_cycles, mem, cpu.index_reg_Y, effective_address);
            break;
        }
        case INS_JSR: // Jump to Subroutine : Absolute, 0x20, 3bytes, 6 cycles
        {
            //  1    PC     R  fetch opcode, increment PC
            //  2    PC     R  fetch low address byte, increment PC
            //  3  $0100,S  R  internal operation (predecrement S?)
            //  4  $0100,S  W  push PCH on stack, decrement S
            //  5  $0100,S  W  push PCL on stack, decrement S
            //  6    PC     R  copy low address byte to PCL, fetch high address byte to PCH

            u16 sub_address = Fetch_Word(&num_cycles, mem); // (*cycles) -= 2;
            Push_PC_Minus_One_To_Stack(&num_cycles, mem);   // (*cycles) -= 1; x2
            cpu.program_counter = sub_address;
            num_cycles -= 1;

            break;
        }
        default:
        {
            print_db("Instruction not handled %x\n", instruction);
            // return -1;
            break;
        }
        }
    } // while (num_cycles > 0)

    const s32 NumCyclesUsed = CyclesRequested - num_cycles;

    print_int(CyclesRequested);
    print_int(num_cycles);
    print_int(NumCyclesUsed);

    return NumCyclesUsed;
}
#endif // __H6502_H__