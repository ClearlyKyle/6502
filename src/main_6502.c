#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

typedef uint8_t u8;   // byte [0, 255]
typedef uint16_t u16; // word [0, 65,535]
typedef uint32_t u32; // [0, 4,294,967,295]

#define ASSERT( Condition, Text ) { if ( !Condition ) { throw -1; } }

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

    u8 C : 1; //Carry flag
    u8 Z : 1; //Zero flag
    u8 I : 1; //Interrupt flag
    u8 D : 1; //Decimal flag
    u8 B : 1; //Break flag
    u8 V : 1; //Overflow flag
    u8 N : 1; //Negative flag
} CPU;

// opcodes
#define INS_LDA_IM 0xA9
#define INS_LDA_ZP 0xA5
#define INS_LDA_ZPX 0xB5
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
    cpu->program_counter = 0xFFFC;
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

u8 fetch_byte(u32 *cycles, Memory *mem)
{
    assert(cpu.program_counter < MAX_MEM);

    u8 data = mem->data[cpu.program_counter];
    cpu.program_counter++;
    (*cycles)--;
    return data;
}

u16 fetch_word(u32 *cycles, Memory *mem)
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

u8 write_byte(u32 *cycles, Memory *mem, u8 data, u16 address)
{
    assert(address < MAX_MEM);

    mem->data[address] = data;
    cycles--;
}

// not executing a piece of code, only reading from memeory
u8 read_byte(u32 *cycles, u8 address, Memory *mem)
{
    assert(address < MAX_MEM);

    u8 data = mem->data[address];
    (*cycles)--;
    return data;
}

void write_word(u32 *cycles, u16 data, u32 address)
{   
    // move to the next address and set it equal to 
    mem.data[address + 1] = (data >> 8); // 1 cycle
    cycles--;
    mem.data[address] = (data & 0xFF);     // 1 cycle
    cycles--;

}

void LDA_set_status()
{
    cpu.Z = (cpu.accumulator == 0);
    cpu.N = (cpu.accumulator & 0b10000000) > 0;
}

// execute "num_cycles" the instruction in memory
void execute(u32 num_cycles, Memory *mem)
{
    while (num_cycles > 0)
    {
        const u8 instruction = fetch_byte(&num_cycles, mem);
        switch (instruction)
        {
        case INS_LDA_IM:
        {
            printf("[INSTRUCTION] INS_LDA_IM\n");

            const u8 value = fetch_byte(&num_cycles, mem);
            cpu.accumulator = value;
            LDA_set_status();
            break;
        }
        case INS_LDA_ZP:
        {
            const u8 zero_page_address = fetch_byte(&num_cycles, mem);

            cpu.accumulator = read_byte(&num_cycles, zero_page_address, mem);

            LDA_set_status();
            break;
        }
        case INS_LDA_ZPX:
        {
            u8 zero_page_address = fetch_byte(&num_cycles, mem);
            zero_page_address += cpu.index_reg_X;
            num_cycles--;

            cpu.accumulator = read_byte(&num_cycles, zero_page_address, mem);
            LDA_set_status();

            break;
        }
        case INS_JSR:
        {
            printf("[INSTRUCTION] INS_JSR\n");
            
            u16 sub_address = fetch_word(&num_cycles, mem);

            write_word(&num_cycles, (cpu.program_counter - 1), cpu.stack_pointer);
            // cpu.stack_pointer++;

            cpu.program_counter = sub_address;
            num_cycles--;
            break;
        }
        default:
        {
            printf("Instruction not handled %x\n", instruction);
        }
        }
        printf("cycle = %d\n", num_cycles);
        // num_cycles--;
    }
}

int main(int argc, char const *argv[])
{

    reset_cpu(&cpu, &mem);

    // start - little program
    mem.data[0xFFFC] = INS_JSR;
    mem.data[0xFFFD] = 0x42;
    mem.data[0xFFFE] = 0x42;
    mem.data[0x4242] = INS_LDA_IM;
    mem.data[0x4243] = 0x84;
    // end - little program
    execute(9, &mem);

    printf("Program Exit...\n");
    return 0;
}
