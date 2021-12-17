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
    u16 program_counter; // program counter
    u8 stack_pointer;

    u8 accumulator; // A
    u8 index_reg_X; // X
    u8 index_reg_Y; // Y

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
    INS_LDY_ABS_X = 0xBE,

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
u8 Fetch_Byte(s32 *cycles, Memory *mem)
{
    assert(cpu.program_counter < MAX_MEM);

    u8 data = mem->data[cpu.program_counter];
    cpu.program_counter++;
    (*cycles) -= 1;
    return data;
}

// 2 Cycles
u16 Fetch_Word(s32 *cycles, Memory *mem)
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

u8 Write_Byte(s32 *cycles, Memory *mem, u8 data, u16 address)
{
    assert(address < MAX_MEM);

    mem->data[address] = data;
    (*cycles) -= 1;
}

// 1 Cycle
u8 Read_Byte(s32 *cycles, u16 address, Memory *mem)
{
    assert(address < MAX_MEM);

    u8 data = mem->data[address];
    (*cycles) -= 1;
    return data;
}

// 2 Cycles
u16 Read_Word(s32 *cycles, u16 address, Memory *mem)
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

void LDA_set_status()
{
    cpu.Z = (cpu.accumulator == 0);
    cpu.N = (cpu.accumulator & 0b10000000) > 0;
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
        case INS_LDA_IM: // 2 Cycles
        {
            const u8 value = Fetch_Byte(&num_cycles, mem); // -1 cycle
            cpu.accumulator = value;
            LDA_set_status();

            break;
        }
        case INS_LDA_ZP: // 3 Cycles
        {
            const u8 zero_page_address = Fetch_Byte(&num_cycles, mem);
            cpu.accumulator = Read_Byte(&num_cycles, zero_page_address, mem);
            LDA_set_status();

            break;
        }
        case INS_LDA_ZP_X: // 4 Cycles
        {
            u8 zero_page_address = Fetch_Byte(&num_cycles, mem);
            zero_page_address += cpu.index_reg_X;
            num_cycles--;

            cpu.accumulator = Read_Byte(&num_cycles, zero_page_address, mem);
            LDA_set_status();

            break;
        }
        case INS_LDA_ABS: // 4 Cycles
        {
            //  1    PC     R  fetch opcode, increment PC
            //  2    PC     R  fetch low byte of address, increment PC
            //  3    PC     R  fetch high byte of address, increment PC
            //  4  address  R  read from effective address

            print_db("[INSTRUCTION] INS_LDA_ABS\n");

            const u16 absolute_address = Fetch_Word(&num_cycles, mem);
            cpu.accumulator = Read_Byte(&num_cycles, absolute_address, mem);
            break;
        }
        case INS_LDA_ABS_X: // 4 Cycles (+1 if page crossed)
        {
            print_db("[INSTRUCTION] INS_LDA_ABS_X\n");

            const u16 absolute_address = Fetch_Word(&num_cycles, mem); // 2 cycles

            const u16 absolute_address_x = absolute_address + cpu.index_reg_X;

            cpu.accumulator = Read_Byte(&num_cycles, absolute_address_x, mem); // 1 Cycle

            // print_hex(absolute_address);
            // print_hex(absolute_address_x);

            // crossing page boundary
            if ((absolute_address_x - absolute_address) > 0xFF)
            {
                num_cycles -= 1;
            }
            break;
        }
        case INS_LDA_ABS_Y: // 4 Cycles (+1 if page crossed)
        {
            print_db("[INSTRUCTION] INS_LDA_ABS_Y\n");

            const u16 absolute_address = Fetch_Word(&num_cycles, mem); // 2 cycles
            const u16 absolute_address_y = absolute_address + cpu.index_reg_Y;

            cpu.accumulator = Read_Byte(&num_cycles, absolute_address_y, mem); // 1 Cycle

            // crossing page boundary
            if ((absolute_address_y - absolute_address) > 0xFF)
            {
                num_cycles -= 1;
            }
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
            print_db("[INSTRUCTION] INS_LDA_IND_X\n");

            u8 zero_page_address = Fetch_Byte(&num_cycles, mem);
            zero_page_address += cpu.index_reg_X;
            num_cycles -= 1;

            const u16 effective_address = Read_Word(&num_cycles, zero_page_address, mem);
            cpu.accumulator = Read_Byte(&num_cycles, effective_address, mem);

            break;
        }
        case INS_LDA_IND_Y: // 5 Cycles (+1 if page crossed)
        {
            print_db("[INSTRUCTION] INS_LDA_IND_Y\n");

            const u8 zero_page_address = Fetch_Byte(&num_cycles, mem);
            const u16 effective_address = Read_Word(&num_cycles, zero_page_address, mem);

            const u16 effective_address_y = effective_address + cpu.index_reg_Y;

            cpu.accumulator = Read_Byte(&num_cycles, effective_address_y, mem);

            if ((effective_address_y - effective_address) >= 0xFF)
            {
                num_cycles -= 1;
            }
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