#ifndef __H6502_H__
#define __H6502_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include "macros.h"

// Ref
// https://web.archive.org/web/20210426072206/http://www.obelisk.me.uk/6502/index.html
// https://www.nesdev.com/6502_cpu.txt

#define DEBUG_PRINT 1

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
// LDA - Load Accumulator
#define INS_LDA_IM 0xA9
#define INS_LDA_ZP 0xA5
#define INS_LDA_ZPX 0xB5
#define INS_LDA_ABS 0xAD
#define INS_LDA_ABS_X 0xBD
#define INS_LDA_ABS_Y 0xB9
#define INS_LDA_IND_X 0xA1
#define INS_LDA_IND_Y 0xB1

#define INS_JSR 0x20

static Memory mem;
static CPU cpu;

void initialise_memory(Memory *mem)
{
    for (size_t i = 0; i < MAX_MEM; i++)
    {
        mem->data[i] = 0;
    }
}

void reset_cpu(CPU *cpu, Memory *mem)
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

    initialise_memory(mem);
}

u8 fetch_byte(s32 *cycles, Memory *mem)
{
    assert(cpu.program_counter < MAX_MEM);

    u8 data = mem->data[cpu.program_counter];
    cpu.program_counter++;
    (*cycles) -= 1;
    return data;
}

u16 fetch_word(s32 *cycles, Memory *mem)
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

u8 write_byte(s32 *cycles, Memory *mem, u8 data, u16 address)
{
    assert(address < MAX_MEM);

    mem->data[address] = data;
    (*cycles) -= 1;
}

u8 Read_Byte(s32 *cycles, u16 address, Memory *mem)
{
    assert(address < MAX_MEM);

    u8 data = mem->data[address];
    (*cycles) -= 1;
    return data;
}

void write_word(s32 *cycles, u16 data, u32 address)
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
    write_byte(cycles, mem, value >> 8, 0x100 | cpu.stack_pointer);
    cpu.stack_pointer--;
    write_byte(cycles, mem, value & 0xFF, 0x100 | cpu.stack_pointer);
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
s32 execute(s32 num_cycles, Memory *mem)
{
    const s32 CyclesRequested = num_cycles;
    while (num_cycles > 0)
    {
        const u8 instruction = fetch_byte(&num_cycles, mem);
        print_int(instruction);
        switch (instruction)
        {
        case INS_LDA_IM: // 2 Cycles
        {
            const u8 value = fetch_byte(&num_cycles, mem); // (*cycles)--;
            cpu.accumulator = value;
            LDA_set_status();

            break;
        }
        case INS_LDA_ZP: // 3 Cycles
        {
            const u8 zero_page_address = fetch_byte(&num_cycles, mem);
            cpu.accumulator = Read_Byte(&num_cycles, zero_page_address, mem);
            LDA_set_status();

            break;
        }
        case INS_LDA_ZPX: // 4 Cycles
        {
            u8 zero_page_address = fetch_byte(&num_cycles, mem);
            zero_page_address += cpu.index_reg_X;
            num_cycles--;

            cpu.accumulator = Read_Byte(&num_cycles, zero_page_address, mem);
            LDA_set_status();

            break;
        }
        case INS_LDA_ABS: // 4 Cycles
        {
            print_db("[INSTRUCTION] INS_LDA_ABS\n");

            const u16 absolute_address = fetch_word(&num_cycles, mem);
            cpu.accumulator = Read_Byte(&num_cycles, absolute_address, mem);
            break;
        }
        case INS_LDA_ABS_X: // 4 Cycles (+1 if page crossed)
        {
            print_db("[INSTRUCTION] INS_LDA_ABS_X\n");

            u16 absolute_address = fetch_word(&num_cycles, mem);

            absolute_address += cpu.index_reg_X;
            // num_cycles--;

            cpu.accumulator = Read_Byte(&num_cycles, absolute_address, mem);
            break;
        }
        case INS_LDA_ABS_Y: // 4 Cycles (+1 if page crossed)
        {
            print_db("[INSTRUCTION] INS_LDA_ABS_Y\n");
            break;
        }
        case INS_LDA_IND_X: // 6 Cycles
        {
            print_db("[INSTRUCTION] INS_LDA_IND_X\n");
            break;
        }
        case INS_LDA_IND_Y: // 5 Cycles (+1 if page crossed)
        {
            print_db("[INSTRUCTION] INS_LDA_IND_Y\n");
            break;
        }
        case INS_JSR: // Jump to Subroutine : Absolute, 0x20, 3bytes, 6 cycles
        {
            // 1    PC     R  fetch opcode, increment PC

            // 2    PC     R  fetch low address byte, increment PC
            // 3  $0100,S  R  internal operation (predecrement S?)
            // 4  $0100,S  W  push PCH on stack, decrement S
            // 5  $0100,S  W  push PCL on stack, decrement S
            // 6    PC     R  copy low address byte to PCL, fetch high address byte to PCH

            printf("Cycles before = %d\n", num_cycles);
            u16 sub_address = fetch_word(&num_cycles, mem); // (*cycles) -= 2;
            Push_PC_Minus_One_To_Stack(&num_cycles, mem);   // (*cycles) -= 1; x2
            cpu.program_counter = sub_address;
            num_cycles -= 1;

            printf("Cycles after = %d\n", num_cycles);
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
    print_int(CyclesRequested);
    print_int(num_cycles);
    const s32 NumCyclesUsed = CyclesRequested - num_cycles;
    return NumCyclesUsed;
}
#endif // __H6502_H__