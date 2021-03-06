#ifndef __H6502_H__
#define __H6502_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

// https://stackoverflow.com/questions/7597025/difference-between-stdint-h-and-inttypes-h
#include <inttypes.h>

#include "macros.h"

// Ref
// > Opcodes : https://web.archive.org/web/20210426072206/http://www.obelisk.me.uk/6502/index.html
// >            http://www.6502.org/tutorials/6502opcodes.html
// > Instruction cycles : https://www.nesdev.com/6502_cpu.txt
// > Addressing Modes : https://www.c64-wiki.com/wiki/Addressing_mode
// > Hex Calculator : https://www.calculator.net/hex-calculator.html?number1=80&c2op=%2B&number2=ff&calctype=op&x=85&y=13
//              https://keisan.casio.com/exec/system/14495408631029
// The first 256 Bytes of the memory map (0-255 or $0000-$00FF) are called zeropage (Page 0)
//      look up [RAM Table]
// https://www.pagetable.com/c64ref/6502/?tab=2#INC

#define DEBUG_PRINT 0

// TODO: Macros to access registers in cpu

typedef uint8_t u8;   // byte [0, 255]
typedef uint16_t u16; // word [0, 65,535]
typedef uint32_t u32; // [0, 4,294,967,295]
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;

#define MAX_MEM 65536 // 1024 * 64 = 65536

typedef struct Memory
{
    u8 data[MAX_MEM];
} Memory;

// union Status_Flags
//{
//     u8 program_status;
//     Status_Flags flags;
// };

typedef struct CPU
{
    // Registers
    u16 program_counter; // Program Counter
    u8 stack_pointer;    // Stack Pointer

    u8 accumulator; // A
    u8 index_reg_X; // X
    u8 index_reg_Y; // Y

    union
    {
        uint8_t PS; // Processor Status
        struct
        {
            // Binary Representation
            // N V u B D I Z C
            // 8 7 6 5 4 3 2 1

            u8 C : 1;      // 0 - Carry flag
            u8 Z : 1;      // 1 - Zero flag
            u8 I : 1;      // 2 - Interrupt flag
            u8 D : 1;      // 3 - Decimal flag
            u8 B : 1;      // 4 - Break flag
            u8 unused : 1; // not used, should be 1 at all times
            u8 V : 1;      // 6 - Overflow flag
            u8 N : 1;      // 7 - Negative flag
        };
    };
} CPU;

enum FlagBits
{
    NEGATIVE_FLAG_BIT = 0x80,         // 0b''1000'0000
    OVERFLOW_FLAG_BIT = 0x40,         // 0b''0100'0000
    BREAK_FLAG_BIT = 0x10,            // 0b0'0001'0000
    unused_FLAG_BIT = 0x20,           // 0b0'0010'0000
    INTERUPT_DISABLE_FLAG_BIT = 0x04, // 0b0'0000'0100
    ZERO_BIT = 0x01,                  // 0b''0000'0001
};

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

    // JMP - JuMP
    INS_JMP_ABS = 0x4C,
    INS_JMP_IND = 0x6C,

    // JSR - Jump to SubRoutine
    INS_JSR = 0x20,

    // RTS - ReTurn from Subroutine
    INS_RTS = 0x60,

    // - Register Instructions -
    // These instructions are implied mode, have a length of one byte and require two machine cycles.
    INS_TAX = 0xAA, // Transfer Accumulator to Index X
    INS_TXA = 0x8A, // Transfer Index X to Accumulator
    INS_TAY = 0xA8, // Transfer Accumulator to Index Y
    INS_TYA = 0x98, // Transfer Index Y to Accumulator

    INS_DEX = 0xCA, // (DEcrement X)
    INS_INX = 0xE8, // (INcrement X)
    INS_DEY = 0x88, // (DEcrement Y)
    INS_INY = 0xC8, // (INcrement Y)

    // - Stack Instructions -
    // These instructions are implied mode, have a length of one byte and require machine cycles as
    // indicated. The stack is always on page one ($100-$1FF) and works top down.
    INS_TSX = 0xBA, // Transfer Stack Pointer to Index X
    INS_TXS = 0x9A, // Transfer Index X to Stack Register

    INS_PHA = 0x48, // (PusH Accumulator) Push Accumulator on Stack
    INS_PLA = 0x68, // (PuLl Accumulator) Pull Accumulator from Stack
    INS_PHP = 0x08, // (PusH Processor status) Push Processor Status on Stack
    INS_PLP = 0x28, // (PuLl Processor status) Pull Processor Status from Stack

    // ORA - OR Memory with Accumulator
    INS_ORA_IM = 0x09,
    INS_ORA_ZP = 0x05,
    INS_ORA_ZP_X = 0x15,
    INS_ORA_ABS = 0x0D,
    INS_ORA_ABS_X = 0x1D,
    INS_ORA_ABS_Y = 0x19,
    INS_ORA_IND_X = 0x01,
    INS_ORA_IND_Y = 0x11,

    // AND - bitwise AND with accumulator
    INS_AND_IM = 0x29,
    INS_AND_ZP = 0x25,
    INS_AND_ZP_X = 0x35,
    INS_AND_ABS = 0x2D,
    INS_AND_ABS_X = 0x3D,
    INS_AND_ABS_Y = 0x39,
    INS_AND_IND_X = 0x21,
    INS_AND_IND_Y = 0x31,

    // EOR - Exclusive OR
    INS_EOR_IM = 0x49,
    INS_EOR_ZP = 0x45,
    INS_EOR_ZP_X = 0x55,
    INS_EOR_ABS = 0x4D,
    INS_EOR_ABS_X = 0x5D,
    INS_EOR_ABS_Y = 0x59,
    INS_EOR_IND_X = 0x41,
    INS_EOR_IND_Y = 0x51,

    // BIT - test BITs
    INS_BIT_ZP = 0x24,
    INS_BIT_ABS = 0x2C,

    // DEC (DECrement memory)
    INS_DEC_ZP = 0xC6,
    INS_DEC_ZP_X = 0xD6,
    INS_DEC_ABS = 0xCE,
    INS_DEC_ABS_X = 0xDE,

    // INC (INCrement memory)
    INS_INC_ZP = 0xE6,
    INS_INC_ZP_X = 0xF6,
    INS_INC_ABS = 0xEE,
    INS_INC_ABS_X = 0xFE,

    // Branch Instructions
    INS_BPL = 0x10, // BPL (Branch on PLus)
    INS_BMI = 0x30, // BMI (Branch on MInus)
    INS_BVC = 0x50, // BVC (Branch on oVerflow Clear)
    INS_BVS = 0x70, // BVS (Branch on oVerflow Set)
    INS_BCC = 0x90, // BCC (Branch on Carry Clear)
    INS_BCS = 0xB0, // BCS (Branch on Carry Set)
    INS_BNE = 0xD0, // BNE (Branch on Not Equal)
    INS_BEQ = 0xF0, // BEQ (Branch on EQual)

    // Flag (Processor Status) Instructions
    INS_CLC = 0x18, // (CLear Carry)
    INS_SEC = 0x38, // (SEt Carry)
    INS_CLI = 0x58, // (CLear Interrupt)
    INS_SEI = 0x78, // (SEt Interrupt)
    INS_CLV = 0xB8, // (CLear oVerflow)
    INS_CLD = 0xD8, // (CLear Decimal)
    INS_SED = 0xF8, // (SEt Decimal)

    // NOP (No OPeration)
    INS_NOP = 0xEA,

    // ADC (ADd with Carry)
    INS_ADC_IM = 0x69,
    INS_ADC_ZP = 0x65,
    INS_ADC_ZP_X = 0x75,
    INS_ADC_ABS = 0x6D,
    INS_ADC_ABS_X = 0x7D,
    INS_ADC_ABS_Y = 0x79,
    INS_ADC_IND_X = 0x61,
    INS_ADC_IND_Y = 0x71

} Opcode;

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
    cpu->stack_pointer = 0xFF;

    cpu->accumulator = 0;
    cpu->index_reg_X = 0;
    cpu->index_reg_Y = 0;

    // cpu->PS = 0x00;

    cpu->C = 0;
    cpu->Z = 0;
    cpu->I = 0;
    cpu->D = 0;
    cpu->B = 0;
    cpu->unused = 1; // should be 1 at all times
    cpu->V = 0;
    cpu->N = 0;

    Initialise_Memory(mem);
}

#define GET_VAR_NAME(V) #V

void Display_CPU_State(const CPU *cpu)
{
    printf("A  : 0x%X \t(%d) \tSP: 0x%X \t(%d) \n", cpu->accumulator, cpu->accumulator, cpu->stack_pointer, cpu->stack_pointer);
    printf("X  : 0x%X \t(%d) \tPC: 0x%X \t(%d) \n", cpu->index_reg_X, cpu->index_reg_X, cpu->program_counter, cpu->program_counter);
    printf("Y  : 0x%X \t(%d) \n", cpu->index_reg_Y, cpu->index_reg_Y);

    char PS_str[] = "NV-BDIZC";
    // Binary Representation
    // N V u B D I Z C
    // 8 7 6 5 4 3 2 1
    printf("Current PS : %X\n", cpu->PS);

    for (int i = 0; i < 8; i++)
    {
        if (!((cpu->PS >> i) & 0x01))
        {
            PS_str[7 - i] = '-';
        }
    }

    printf("PS :  %s\t(0x%X)\n", PS_str, cpu->PS);
}

u16 Load_Program(const u8 *program, Memory *mem, int number_of_bytes)
{
    // if (!program)
    //{
    //     fprintf(stderr, "Error Loading Program Data\n");
    //     return
    // }
    u16 load_address = 0x00;
    if (program != NULL && number_of_bytes >= 2)
    {
        u32 current_position = 0;

        // LOW | (HIGH << 8) : 0xHHLL
        const u32 LL = program[current_position++];
        const u32 HH = program[current_position++] << 8;

        load_address = LL | HH;

        for (u16 i = load_address; i < load_address + number_of_bytes - 2; i++)
        {
            mem->data[i] = program[current_position++];
        }
    }
    return load_address;
}

u16 SP_To_Address()
{
    return 0x100 | cpu.stack_pointer;
}

// 1 Cycle
u8 Fetch_Byte(s32 *cycles, const Memory *mem)
{
    assert(cpu.program_counter < MAX_MEM);

    const u8 data = mem->data[cpu.program_counter];
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
void Write_Byte(s32 *cycles, Memory *mem, u8 data, u16 address)
{
    mem->data[address] = data;
    (*cycles) -= 1;
}

// 1 Cycle
u8 Read_Byte(s32 *cycles, u16 address, const Memory *mem)
{
    const u8 data = mem->data[address];
    (*cycles) -= 1;
    return data;
}

// 2 Cycles
u16 Read_Word(s32 *cycles, u16 address, const Memory *mem)
{
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

/** Pop a 16-bit value from the stack */
u16 Pop_Word_From_Stack(s32 *cycles, Memory *mem)
{
    const u16 value_from_stack = Read_Word(cycles, SP_To_Address() + 1, mem);
    cpu.stack_pointer += 2;
    (*cycles) -= 1;
    return value_from_stack;
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

void Push_Byte_Onto_Stack(s32 *cycles, u8 value, Memory *mem)
{
    mem->data[SP_To_Address()] = value;
    cpu.stack_pointer--;
    (*cycles) -= 1;
}

// 2 cycles
u8 Pop_Byte_From_Stack(s32 *cycles, Memory *mem)
{
    cpu.stack_pointer++;
    (*cycles) -= 2;
    return mem->data[SP_To_Address()];
}

// A, X or Y Register
void Load_Register_Set_Status(u8 reg)
{
    cpu.Z = (reg == 0);
    cpu.N = (reg & 0x80) > 0;
}

// Addressing mode - Zero Page (1 cycle)
u16 Address_Zero_Page(s32 *cycles, const Memory *mem)
{
    const u8 zero_page_address = Fetch_Byte(cycles, mem);
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
    const u16 absolute_address = Fetch_Word(cycles, mem);
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
u16 Address_Absolute_X_5_Cycle(s32 *cycles, const Memory *mem) // Special Case
{
    const u16 absolute_address = Fetch_Word(cycles, mem);
    const u16 absolute_address_x = absolute_address + cpu.index_reg_X;
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
u16 Address_Absolute_Y_5_Cycle(s32 *cycles, const Memory *mem) // Special case
{
    const u16 absolute_address = Fetch_Word(cycles, mem);
    const u16 absolute_address_y = absolute_address + cpu.index_reg_Y;
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
// 4 Cycles
u16 Address_Indirect_Y_6_Cycles(s32 *cycles, const Memory *mem) // Special Case
{
    const u8 zero_page_address = Fetch_Byte(cycles, mem);
    const u16 effective_address = Read_Word(cycles, zero_page_address, mem);
    const u16 effective_address_y = effective_address + cpu.index_reg_Y;
    (*cycles) -= 1;

    return effective_address_y;
}

// Load a value at an 'address' into a given 'register' (1 cycle)
void Load_Register(s32 *cycles, u8 *reg, const u16 address, const Memory *mem)
{
    (*reg) = Read_Byte(cycles, address, mem);
    Load_Register_Set_Status((*reg));
}

// AND the A register with the value from 'address'
void AND_Register(s32 *cycles, const u16 address, const Memory *mem)
{
    cpu.accumulator &= Read_Byte(cycles, address, mem);
    Load_Register_Set_Status(cpu.accumulator);
}
// OR the A register with the value from 'address'
void OR_Register(s32 *cycles, const u16 address, const Memory *mem)
{
    cpu.accumulator |= Read_Byte(cycles, address, mem);
    Load_Register_Set_Status(cpu.accumulator);
}
// EOR the A register with the value from 'address'
void EOR_Register(s32 *cycles, const u16 address, const Memory *mem)
{
    cpu.accumulator ^= Read_Byte(cycles, address, mem);
    Load_Register_Set_Status(cpu.accumulator);
}

void Branch_If(s32 *cycles, const Memory *mem, u8 flag, u8 expected)
{
    const s8 jump_offset = (s8)Fetch_Byte(cycles, mem);
    if (flag == expected)
    {
        const u16 old_program_counter = cpu.program_counter;
        cpu.program_counter += jump_offset;
        (*cycles) -= 1;

        if ((cpu.program_counter >> 8) != (old_program_counter >> 8))
        {
            (*cycles) -= 2;
        }
    }
}

/** Do add with carry given the the operand */
void ADC(u8 operand)
{
    assert(cpu.D == false && "haven't handled decimal mode!");

    const bool AreSignBitsTheSame = !((cpu.accumulator ^ operand) & NEGATIVE_FLAG_BIT);
    u16 sum = cpu.accumulator;
    sum += operand;
    sum += cpu.C;

    cpu.accumulator = (sum & 0xFF);

    SetZeroAndNegativeFlags(cpu.accumulator);

    cpu.C = sum > 0xFF;
    cpu.V = AreSignBitsTheSame && ((cpu.accumulator ^ operand) & NEGATIVE_FLAG_BIT);
};

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
        case INS_STA_ZP: // 3 cycles
        {
            // STore Accumulator _ ZP value
            // 1    PC     R  fetch opcode, increment PC
            // 2    PC     R  fetch address, increment PC
            // 3  address  W  write register to effective address
            const u16 effective_address = Address_Zero_Page(&num_cycles, mem); // 1 cycle
            Write_Byte(&num_cycles, mem, cpu.accumulator, effective_address);
            break;
        }
        case INS_STA_ZP_X: // 4 cycles
        {
            const u16 effective_address = Address_Zero_Page_X(&num_cycles, mem); // 2 cycles
            Write_Byte(&num_cycles, mem, cpu.accumulator, effective_address);
            break;
        }
        case INS_STA_ABS: // 4 cycles
        {
            const u16 effective_address = Address_Absolute(&num_cycles, mem); // 2 cycles
            Write_Byte(&num_cycles, mem, cpu.accumulator, effective_address);
            break;
        }
        case INS_STA_ABS_X: // 5 cycles
        {
            const u16 effective_address = Address_Absolute_X_5_Cycle(&num_cycles, mem); // 2 cycles
            Write_Byte(&num_cycles, mem, cpu.accumulator, effective_address);           // 1 cycle
            break;
        }
        case INS_STA_ABS_Y: // 5 cycles
        {
            const u16 effective_address = Address_Absolute_Y_5_Cycle(&num_cycles, mem); // 2 cycles
            Write_Byte(&num_cycles, mem, cpu.accumulator, effective_address);           // 1 cycle
            break;
        }
        case INS_STA_IND_X: // 6 cycles
        {
            const u16 effective_address = Address_Indirect_X(&num_cycles, mem); // 4 cycles
            Write_Byte(&num_cycles, mem, cpu.accumulator, effective_address);
            break;
        }
        case INS_STA_IND_Y: // 6 cycles
        {
            const u16 effective_address = Address_Indirect_Y_6_Cycles(&num_cycles, mem); // 4 cycles
            Write_Byte(&num_cycles, mem, cpu.accumulator, effective_address);            // 1 cycle
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

        // JMP - JuMP
        case INS_JMP_ABS:
        {
            const u16 address = Address_Absolute(&num_cycles, mem);
            cpu.program_counter = address;
            break;
        }
        case INS_JMP_IND:
        {
            /*
                1     PC      R  fetch opcode, increment PC
                2     PC      R  fetch pointer address low, increment PC
                3     PC      R  fetch pointer address high, increment PC
                4   pointer   R  fetch low address to latch
                5  pointer+1* R  fetch PCH, copy latch to PCL

                The PCH will always be fetched from the same page
                than PCL, i.e. page boundary crossing is not handled.
            */
            const u16 address = Address_Absolute(&num_cycles, mem);
            cpu.program_counter = Read_Word(&num_cycles, address, mem);
            break;
        }

            // JSR - Jump to SubRoutine
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
        // RTS - ReTurn from Subroutine
        case INS_RTS:
        {
            //  1    PC     R  fetch opcode, increment PC
            //  2    PC     R  read next instruction byte (and throw it away)
            //  3  $0100,S  R  increment S
            //  4  $0100,S  R  pull PCL from stack, increment S
            //  5  $0100,S  R  pull PCH from stack
            //  6    PC     R  increment PC

            // Pull top two bytes off the stack (PCL first)
            // move to address + 1
            const u16 return_address = Pop_Word_From_Stack(&num_cycles, mem);
            cpu.program_counter = return_address + 1;
            num_cycles -= 2;
            break;
        }
        // - Register Instructions -
        case INS_TAX: // Transfer Accumulator to Index X
        {
            cpu.index_reg_X = cpu.accumulator;
            num_cycles -= 1;
            Load_Register_Set_Status(cpu.accumulator);
            break;
        }
        case INS_TXA: // Transfer Index X to Accumulator
        {
            cpu.accumulator = cpu.index_reg_X;
            num_cycles -= 1;
            Load_Register_Set_Status(cpu.index_reg_X);
            break;
        }
        case INS_TAY: // Transfer Accumulator to Index Y
        {
            cpu.index_reg_Y = cpu.accumulator;
            num_cycles -= 1;
            Load_Register_Set_Status(cpu.index_reg_Y);
            break;
        }
        case INS_TYA: // Transfer Index Y to Accumulator
        {
            cpu.accumulator = cpu.index_reg_Y;
            num_cycles -= 1;
            Load_Register_Set_Status(cpu.index_reg_Y);
            break;
        }
        case INS_DEX: // (DEcrement X)
        {
            cpu.index_reg_X--;
            num_cycles -= 1;
            Load_Register_Set_Status(cpu.index_reg_X);
            break;
        }
        case INS_INX: // (INcrement X)
        {
            cpu.index_reg_X++;
            num_cycles -= 1;
            Load_Register_Set_Status(cpu.index_reg_X);
            break;
        }
        case INS_DEY: // (DEcrement Y)
        {
            cpu.index_reg_Y--;
            num_cycles -= 1;
            Load_Register_Set_Status(cpu.index_reg_Y);
            break;
        }
        case INS_INY: // (INcrement Y)
        {
            cpu.index_reg_Y++;
            num_cycles -= 1;
            Load_Register_Set_Status(cpu.index_reg_Y);
            break;
        }
        // - Stack Instructions -
        case INS_TSX: // Transfer Stack Pointer to Index X
        {
            cpu.index_reg_X = cpu.stack_pointer;
            Load_Register_Set_Status(cpu.index_reg_X);
            num_cycles -= 1;
            break;
        }
        case INS_TXS: // Transfer Index X to Stack Register
        {
            cpu.stack_pointer = cpu.index_reg_X;
            num_cycles -= 1;
            break;
        }
        case INS_PHA: // Push Accumulator on Stack
        {
            //  1    PC     R  fetch opcode, increment PC
            //  2    PC     R  read next instruction byte (and throw it away)
            //  3  $0100,S  W  push register on stack, decrement S

            Push_Byte_Onto_Stack(&num_cycles, cpu.accumulator, mem);
            num_cycles -= 1;
            break;
        }
        case INS_PLA: // Pull Accumulator from Stack
        {             // 4 cycles
            cpu.accumulator = Pop_Byte_From_Stack(&num_cycles, mem);
            Load_Register_Set_Status(cpu.accumulator);
            num_cycles--;
            break;
        }
        case INS_PHP: // Push Processor Status on Stack
        {
            Push_Byte_Onto_Stack(&num_cycles, cpu.PS, mem);
            num_cycles--;
            break;
        }
        case INS_PLP: // Pull Processor Status from Stack
        {             // 4 cycles
            cpu.PS = Pop_Byte_From_Stack(&num_cycles, mem);
            num_cycles--;
            break;
        }
        // ORA - OR Memory with Accumulator
        case INS_ORA_IM:
        {
            cpu.accumulator |= Fetch_Byte(&num_cycles, mem);
            Load_Register_Set_Status(cpu.accumulator);
            break;
        }
        case INS_ORA_ZP:
        {
            const u16 address = Address_Zero_Page(&num_cycles, mem);
            OR_Register(&num_cycles, address, mem);
            break;
        }
        case INS_ORA_ZP_X:
        {
            const u16 address = Address_Zero_Page_X(&num_cycles, mem);
            OR_Register(&num_cycles, address, mem);
            break;
        }
        case INS_ORA_ABS:
        {
            const u16 address = Address_Absolute(&num_cycles, mem);
            OR_Register(&num_cycles, address, mem);
            break;
        }
        case INS_ORA_ABS_X:
        {
            const u16 address = Address_Absolute_X(&num_cycles, mem);
            OR_Register(&num_cycles, address, mem);
            break;
        }
        case INS_ORA_ABS_Y:
        {
            const u16 address = Address_Absolute_Y(&num_cycles, mem);
            OR_Register(&num_cycles, address, mem);
            break;
        }
        case INS_ORA_IND_X:
        {
            const u16 address = Address_Indirect_X(&num_cycles, mem);
            OR_Register(&num_cycles, address, mem);
            break;
        }
        case INS_ORA_IND_Y:
        {
            const u16 address = Address_Indirect_Y(&num_cycles, mem);
            OR_Register(&num_cycles, address, mem);
            break;
        }
            // AND - bitwise AND with accumulator
        case INS_AND_IM:
        {
            cpu.accumulator &= Fetch_Byte(&num_cycles, mem);
            Load_Register_Set_Status(cpu.accumulator);
            break;
        }
        case INS_AND_ZP:
        {
            const u16 address = Address_Zero_Page(&num_cycles, mem);
            AND_Register(&num_cycles, address, mem);
            break;
        }
        case INS_AND_ZP_X:
        {
            const u16 address = Address_Zero_Page_X(&num_cycles, mem);
            AND_Register(&num_cycles, address, mem);
            break;
        }
        case INS_AND_ABS:
        {
            const u16 address = Address_Absolute(&num_cycles, mem);
            AND_Register(&num_cycles, address, mem);
            break;
        }
        case INS_AND_ABS_X:
        {
            const u16 address = Address_Absolute_X(&num_cycles, mem);
            AND_Register(&num_cycles, address, mem);
            break;
        }
        case INS_AND_ABS_Y:
        {
            const u16 address = Address_Absolute_Y(&num_cycles, mem);
            AND_Register(&num_cycles, address, mem);
            break;
        }
        case INS_AND_IND_X:
        {
            const u16 address = Address_Indirect_X(&num_cycles, mem);
            AND_Register(&num_cycles, address, mem);
            break;
        }
        case INS_AND_IND_Y:
        {
            const u16 address = Address_Indirect_Y(&num_cycles, mem);
            AND_Register(&num_cycles, address, mem);
            break;
        }
            // EOR - Exclusive OR
        case INS_EOR_IM:
        {
            cpu.accumulator ^= Fetch_Byte(&num_cycles, mem);
            Load_Register_Set_Status(cpu.accumulator);
            break;
        }
        case INS_EOR_ZP:
        {
            const u16 address = Address_Zero_Page(&num_cycles, mem);
            EOR_Register(&num_cycles, address, mem);
            break;
        }
        case INS_EOR_ZP_X:
        {
            const u16 address = Address_Zero_Page_X(&num_cycles, mem);
            EOR_Register(&num_cycles, address, mem);
            break;
        }
        case INS_EOR_ABS:
        {
            const u16 address = Address_Absolute(&num_cycles, mem);
            EOR_Register(&num_cycles, address, mem);
            break;
        }
        case INS_EOR_ABS_X:
        {
            const u16 address = Address_Absolute_X(&num_cycles, mem);
            EOR_Register(&num_cycles, address, mem);
            break;
        }
        case INS_EOR_ABS_Y:
        {
            const u16 address = Address_Absolute_Y(&num_cycles, mem);
            EOR_Register(&num_cycles, address, mem);
            break;
        }
        case INS_EOR_IND_X:
        {
            const u16 address = Address_Indirect_X(&num_cycles, mem);
            EOR_Register(&num_cycles, address, mem);
            break;
        }
        case INS_EOR_IND_Y:
        {
            const u16 address = Address_Indirect_Y(&num_cycles, mem);
            EOR_Register(&num_cycles, address, mem);
            break;
        }

        // BIT - test BITs
        case INS_BIT_ZP:
        {
            const u16 address = Address_Zero_Page(&num_cycles, mem);
            const u8 value = Read_Byte(&num_cycles, address, mem);
            cpu.Z = !(cpu.accumulator & value);
            cpu.N = (value & NEGATIVE_FLAG_BIT) != 0;
            cpu.V = (value & OVERFLOW_FLAG_BIT) != 0;
            break;
        }
        case INS_BIT_ABS:
        {
            const u16 address = Address_Absolute(&num_cycles, mem);
            const u8 value = Read_Byte(&num_cycles, address, mem);
            cpu.Z = !(cpu.accumulator & value);
            cpu.N = (value & NEGATIVE_FLAG_BIT) != 0;
            cpu.V = (value & OVERFLOW_FLAG_BIT) != 0;
            break;
        }
        // DEC (DECrement memory)
        case INS_DEC_ZP:
        {
            const u16 address = Address_Zero_Page(&num_cycles, mem);
            u8 value = Read_Byte(&num_cycles, address, mem);
            value -= 1;
            num_cycles -= 1;
            Write_Byte(&num_cycles, mem, value, address);
            Load_Register_Set_Status(value);
            break;
        }
        case INS_DEC_ZP_X:
        {
            const u16 address = Address_Zero_Page_X(&num_cycles, mem);
            u8 value = Read_Byte(&num_cycles, address, mem);
            value -= 1;
            num_cycles -= 1;
            Write_Byte(&num_cycles, mem, value, address);
            Load_Register_Set_Status(value);
            break;
        }
        case INS_DEC_ABS:
        {
            const u16 address = Address_Absolute(&num_cycles, mem);
            u8 value = Read_Byte(&num_cycles, address, mem);
            value -= 1;
            num_cycles -= 1;
            Write_Byte(&num_cycles, mem, value, address);
            Load_Register_Set_Status(value);
            break;
        }
        case INS_DEC_ABS_X:
        {
            const u16 address = Address_Absolute_X_5_Cycle(&num_cycles, mem);
            u8 value = Read_Byte(&num_cycles, address, mem);
            value -= 1;
            num_cycles -= 1;
            Write_Byte(&num_cycles, mem, value, address);
            Load_Register_Set_Status(value);
            break;
        }
        // INC (INCrement memory)
        case INS_INC_ZP:
        {
            const u16 address = Address_Zero_Page(&num_cycles, mem);
            u8 value = Read_Byte(&num_cycles, address, mem);
            value += 1;
            num_cycles -= 1;
            Write_Byte(&num_cycles, mem, value, address);
            Load_Register_Set_Status(value);
            break;
        }
        case INS_INC_ZP_X:
        {
            const u16 address = Address_Zero_Page_X(&num_cycles, mem);
            u8 value = Read_Byte(&num_cycles, address, mem);
            value += 1;
            num_cycles -= 1;
            Write_Byte(&num_cycles, mem, value, address);
            Load_Register_Set_Status(value);
            break;
        }
        case INS_INC_ABS:
        {
            const u16 address = Address_Absolute(&num_cycles, mem);
            u8 value = Read_Byte(&num_cycles, address, mem);
            value += 1;
            num_cycles -= 1;
            Write_Byte(&num_cycles, mem, value, address);
            Load_Register_Set_Status(value);
            break;
        }
        case INS_INC_ABS_X:
        {
            const u16 address = Address_Absolute_X_5_Cycle(&num_cycles, mem);
            u8 value = Read_Byte(&num_cycles, address, mem);
            value += 1;
            num_cycles -= 1;
            Write_Byte(&num_cycles, mem, value, address);
            Load_Register_Set_Status(value);
            break;
        }
        // Branch Instructions
        case INS_BPL: // BPL (Branch on PLus)
        {
            Branch_If(&num_cycles, mem, cpu.N, 0);
            break;
        }
        case INS_BMI: // BMI (Branch on MInus)
        {
            Branch_If(&num_cycles, mem, cpu.N, 1);
            break;
        }
        case INS_BVC: // BVC (Branch on oVerflow Clear)
        {
            Branch_If(&num_cycles, mem, cpu.V, 0);
            break;
        }
        case INS_BVS: // BVS (Branch on oVerflow Set)
        {
            Branch_If(&num_cycles, mem, cpu.V, 1);
            break;
        }
        case INS_BCC: // BCC (Branch on Carry Clear)
        {
            Branch_If(&num_cycles, mem, cpu.C, 0);
            break;
        }
        case INS_BCS: // BCS (Branch on Carry Set)
        {
            Branch_If(&num_cycles, mem, cpu.C, 1);
            break;
        }
        case INS_BNE: // BNE (Branch on Not Equal)
        {
            Branch_If(&num_cycles, mem, cpu.Z, 0);
            break;
        }
        case INS_BEQ: // BEQ (Branch on EQual)
        {
            Branch_If(&num_cycles, mem, cpu.Z, 1);
            break;
        }
        // Flag (Processor Status) Instructions
        case INS_CLC: // (CLear Carry)
        {
            cpu.C = 0;
            break;
        }
        case INS_SEC: // (SEt Carry)
        {
            cpu.C = 1;
            break;
        }
        case INS_CLI: // (CLear Interrupt)
        {
            cpu.I = 0;
            break;
        }
        case INS_SEI: // (SEt Interrupt)
        {
            cpu.I = 1;
            break;
        }
        case INS_CLV: // (CLear oVerflow)
        {
            cpu.V = 0;
            break;
        }
        case INS_CLD: // (CLear Decimal)
        {
            cpu.D = 0;
            break;
        }
        case INS_SED: // (SEt Decimal)
        {
            cpu.D = 1;
            break;
        }
        // NOP (No OPeration)
        case INS_NOP:
        {
            num_cycles -= 1;
            break;
        }
        // ADC (ADd with Carry)
        case INS_ADC_IM:
        {
            break;
        }
        case INS_ADC_ZP:
        {
            break;
        }
        case INS_ADC_ZP_X:
        {
            break;
        }
        case INS_ADC_ABS:
        {
            const u16 address = Address_Absolute(&num_cycles, mem);
            const u8 operand = Read_Byte(&num_cycles, address, mem);
            cpu.accumulator += operand;
            ADC(operand);
            // cpu.Z = 1;
            // cpu.N = 0;
            // cpu.C = 0;
            // cpu.V = 0;
            break;
        }
        case INS_ADC_ABS_X:
        {
            const u16 address = Address_Absolute_X(&num_cycles, mem);
            const u8 operand = Read_Byte(&num_cycles, address, mem);
            cpu.accumulator += operand;
            ADC(operand);
            break;
        }
        case INS_ADC_ABS_Y:
        {
            const u16 address = Address_Absolute_Y(&num_cycles, mem);
            const u8 operand = Read_Byte(&num_cycles, address, mem);
            cpu.accumulator += operand;
            ADC(operand);
            break;
        }
        case INS_ADC_IND_X:
        {
            const u16 address = Address_Indirect_X(&num_cycles, mem);
            const u8 operand = Read_Byte(&num_cycles, address, mem);
            cpu.accumulator += operand;
            ADC(operand);
            break;
        }
        case INS_ADC_IND_Y:
        {
            const u16 address = Address_Indirect_Y(&num_cycles, mem);
            const u8 operand = Read_Byte(&num_cycles, address, mem);
            cpu.accumulator += operand;
            ADC(operand);
            break;
        }
        default:
        {
            print_db("Instruction not handled %x\n", instruction);
            return num_cycles;
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