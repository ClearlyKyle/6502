#include "Unity/unity.h"
#include "h6502.h"

#include <stdbool.h>

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU();
    cpu.program_counter = 0xFF00;
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

static void Verify_Unmodified_Flags(const CPU before, const CPU after)
{
    // get a copt of the cpu
    TEST_ASSERT_EQUAL_UINT8(before.C, after.C);
    TEST_ASSERT_EQUAL_UINT8(before.I, after.I);
    TEST_ASSERT_EQUAL_UINT8(before.D, after.D);
    TEST_ASSERT_EQUAL_UINT8(before.B, after.B);
    TEST_ASSERT_EQUAL_UINT8(before.V, after.V);
}

// INX
void INX_Can_Increment_A_Zero_Value(void)
{
    cpu.index_reg_X = 0x00;
    cpu.Z           = 1;
    cpu.N           = 1;

    mem.data[0xFF00] = INS_INX;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x01, cpu.index_reg_X);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void INX_Can_Increment_0xFF(void)
{
    cpu.index_reg_X = 0xFF;
    cpu.Z           = 0;
    cpu.N           = 1;

    mem.data[0xFF00] = INS_INX;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x00, cpu.index_reg_X);

    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void INX_Can_Increment_A_Negative_Value(void)
{
    cpu.index_reg_X = 0x88; // 0b10001000
    cpu.Z           = 1;
    cpu.N           = 0;

    mem.data[0xFF00] = INS_INX;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x89, cpu.index_reg_X);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

// INY
void INY_Can_Increment_A_Zero_Value(void)
{
    cpu.index_reg_Y = 0x00;
    cpu.Z           = 1;
    cpu.N           = 1;

    mem.data[0xFF00] = INS_INY;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x01, cpu.index_reg_Y);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void INY_Can_Increment_0xFF(void)
{
    cpu.index_reg_Y = 0xFF;
    cpu.Z           = 0;
    cpu.N           = 1;

    mem.data[0xFF00] = INS_INY;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x00, cpu.index_reg_Y);

    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void INY_Can_Increment_A_Negative_Value(void)
{
    cpu.index_reg_Y = 0x88; // 0b10001000
    cpu.Z           = 1;
    cpu.N           = 0;

    mem.data[0xFF00] = INS_INY;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x89, cpu.index_reg_Y);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

// DEY
void DEY_Can_Decrement_A_Zero_Value(void)
{
    cpu.index_reg_Y = 0x00;
    cpu.Z           = 1;
    cpu.N           = 0;

    mem.data[0xFF00] = INS_DEY;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0xFF, cpu.index_reg_Y);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void DEY_Can_Decrement_0xFF(void)
{
    cpu.index_reg_Y = 0xFF;
    cpu.Z           = 1;
    cpu.N           = 0;

    mem.data[0xFF00] = INS_DEY;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0xFE, cpu.index_reg_Y);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void DEY_Can_Decrement_A_Negative_Value(void)
{
    cpu.index_reg_Y = 0x89; // 0b10001001
    cpu.Z           = 1;
    cpu.N           = 0;

    mem.data[0xFF00] = INS_DEY;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x88, cpu.index_reg_Y);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

// DEX
void DEX_Can_Decrement_A_Zero_Value(void)
{
    cpu.index_reg_X = 0x00;
    cpu.Z           = 1;
    cpu.N           = 0;

    mem.data[0xFF00] = INS_DEX;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0xFF, cpu.index_reg_X);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void DEX_Can_Decrement_0xFF(void)
{
    cpu.index_reg_X = 0xFF;
    cpu.Z           = 1;
    cpu.N           = 0;

    mem.data[0xFF00] = INS_DEX;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0xFE, cpu.index_reg_X);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void DEX_Can_Decrement_A_Negative_Value(void)
{
    cpu.index_reg_X = 0x89; // 0b10001001
    cpu.Z           = 1;
    cpu.N           = 0;

    mem.data[0xFF00] = INS_DEX;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x88, cpu.index_reg_X);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

// DEC
void DEC_Can_Decrement_A_Value_In_ZP(void)
{
    cpu.Z = 1;
    cpu.N = 1;

    mem.data[0xFF00] = INS_DEC_ZP;
    mem.data[0xFF01] = 0x42;
    mem.data[0x0042] = 0x57;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 5;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x56, mem.data[0x0042]);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void DEC_Can_Decrement_A_Value_In_ZP_X(void)
{
    cpu.index_reg_X = 0x10;
    cpu.Z           = 1;
    cpu.N           = 1;

    mem.data[0xFF00]        = INS_DEC_ZP_X;
    mem.data[0xFF01]        = 0x42;
    mem.data[0x0042 + 0x10] = 0x57;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 6;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x56, mem.data[0x0042 + 0x10]);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void DEC_Can_Decrement_A_Value_ABS(void)
{
    cpu.Z = 1;
    cpu.N = 1;

    mem.data[0xFF00] = INS_DEC_ABS;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = 0x57;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 6;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x56, mem.data[0x8000]);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void DEC_Can_Decrement_A_Value_ABS_X(void)
{
    cpu.index_reg_X = 0x10;
    cpu.Z           = 1;
    cpu.N           = 1;

    mem.data[0xFF00]        = INS_DEC_ABS_X;
    mem.data[0xFF01]        = 0x00;
    mem.data[0xFF02]        = 0x80;
    mem.data[0x8000 + 0x10] = 0x57;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 7;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x56, mem.data[0x8000 + 0x10]);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

// INC
void INC_Can_Increment_A_Value_In_ZP(void)
{
    cpu.Z = 1;
    cpu.N = 1;

    mem.data[0xFF00] = INS_INC_ZP;
    mem.data[0xFF01] = 0x42;
    mem.data[0x0042] = 0x57;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 5;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x58, mem.data[0x0042]);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void INC_Can_Increment_A_Value_In_ZP_X(void)
{
    cpu.index_reg_X = 0x10;
    cpu.Z           = 1;
    cpu.N           = 1;

    mem.data[0xFF00]        = INS_INC_ZP_X;
    mem.data[0xFF01]        = 0x42;
    mem.data[0x0042 + 0x10] = 0x57;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 6;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x58, mem.data[0x0042 + 0x10]);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void INC_Can_Increment_A_Value_In_ABS(void)
{
    cpu.Z = 1;
    cpu.N = 1;

    mem.data[0xFF00] = INS_INC_ABS;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = 0x57;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 6;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x58, mem.data[0x8000]);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void INC_Can_Increment_A_Value_In_ABS_X(void)
{
    cpu.index_reg_X = 0x10;
    cpu.Z           = 1;
    cpu.N           = 1;

    mem.data[0xFF00]        = INS_INC_ABS_X;
    mem.data[0xFF01]        = 0x00;
    mem.data[0xFF02]        = 0x80;
    mem.data[0x8000 + 0x10] = 0x57;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 7;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x58, mem.data[0x8000 + 0x10]);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void Test_Load_A_Program_That_Can_INC(void)
{
    /*
    * = $1000
    lda #00
    sta $42
    start
    inc $42
    ldx $42
    inx
    jmp start
    */
    u8        test_program[]  = {0x0, 0x10, 0xA9, 0x00, 0x85, 0x42, 0xE6, 0x42,
                         0xA6, 0x42, 0xE8, 0x4C, 0x04, 0x10};
    const int number_of_bytes = sizeof(test_program) / sizeof(test_program[0]);

    const u16 start_address = Load_Program(test_program, number_of_bytes);
    cpu.program_counter     = start_address;

    // then:
    for (s32 clock = 1000; clock > 0;)
    {
        clock -= Execute(1);
    }
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(INX_Can_Increment_A_Zero_Value);
    RUN_TEST(INX_Can_Increment_0xFF);
    RUN_TEST(INX_Can_Increment_A_Negative_Value);

    RUN_TEST(INY_Can_Increment_A_Zero_Value);
    RUN_TEST(INY_Can_Increment_0xFF);
    RUN_TEST(INY_Can_Increment_A_Negative_Value);

    RUN_TEST(DEY_Can_Decrement_A_Zero_Value);
    RUN_TEST(DEY_Can_Decrement_0xFF);
    RUN_TEST(DEY_Can_Decrement_A_Negative_Value);

    RUN_TEST(DEX_Can_Decrement_A_Zero_Value);
    RUN_TEST(DEX_Can_Decrement_0xFF);
    RUN_TEST(DEX_Can_Decrement_A_Negative_Value);

    RUN_TEST(DEC_Can_Decrement_A_Value_In_ZP);
    RUN_TEST(DEC_Can_Decrement_A_Value_In_ZP_X);
    RUN_TEST(DEC_Can_Decrement_A_Value_ABS);
    RUN_TEST(DEC_Can_Decrement_A_Value_ABS_X);

    RUN_TEST(INC_Can_Increment_A_Value_In_ZP);
    RUN_TEST(INC_Can_Increment_A_Value_In_ZP_X);
    RUN_TEST(INC_Can_Increment_A_Value_In_ABS);
    RUN_TEST(INC_Can_Increment_A_Value_In_ABS_X);

    RUN_TEST(Test_Load_A_Program_That_Can_INC);

    return UNITY_END();
}