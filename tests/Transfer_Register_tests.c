#include "Unity/unity.h"
#include "h6502.h"

#include <stdbool.h>

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU(&cpu, &mem);
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

    // TEST_ASSERT_EQUAL_UINT8(before.PS, after.PS);
}

void TAX_Can_Transfer_Non_Negative_Non_Zero_Value(void)
{
    cpu.accumulator = 0x42;
    cpu.index_reg_X = 0x32;
    cpu.Z = 1;
    cpu.N = 1;

    mem.data[0xFF00] = INS_TAX;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x42, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(0x42, cpu.index_reg_X);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void TAX_Can_Transfer_Non_Negative_Zero_Value(void)
{
    cpu.accumulator = 0x00;
    cpu.index_reg_X = 0x32;
    cpu.Z = 0;
    cpu.N = 1;

    mem.data[0xFF00] = INS_TAX;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x00, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(0x00, cpu.index_reg_X);

    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void TAX_Can_Transfer_Negative_Value(void)
{
    cpu.accumulator = 0x8B; // 0b10001011
    cpu.index_reg_X = 0x32;
    cpu.Z = 1;
    cpu.N = 0;

    mem.data[0xFF00] = INS_TAX;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x8B, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(0x8B, cpu.index_reg_X);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void TAY_Can_Transfer_Non_Negative_Non_Zero_Value(void)
{
    cpu.accumulator = 0x42;
    cpu.index_reg_Y = 0x32;
    cpu.Z = 1;
    cpu.N = 1;

    mem.data[0xFF00] = INS_TAY;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x42, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(0x42, cpu.index_reg_Y);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void TAY_Can_Transfer_Non_Negative_Zero_Value(void)
{
    cpu.accumulator = 0x00;
    cpu.index_reg_Y = 0x32;
    cpu.Z = 0;
    cpu.N = 1;

    mem.data[0xFF00] = INS_TAY;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x00, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(0x00, cpu.index_reg_Y);

    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void TAY_Can_Transfer_Negative_Value(void)
{
    cpu.accumulator = 0x8B; // 0b10001011
    cpu.index_reg_Y = 0x32;
    cpu.Z = 1;
    cpu.N = 0;

    mem.data[0xFF00] = INS_TAY;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x8B, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(0x8B, cpu.index_reg_Y);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(TAX_Can_Transfer_Non_Negative_Non_Zero_Value);
    RUN_TEST(TAX_Can_Transfer_Non_Negative_Zero_Value);
    RUN_TEST(TAX_Can_Transfer_Negative_Value);

    RUN_TEST(TAY_Can_Transfer_Non_Negative_Non_Zero_Value);
    RUN_TEST(TAY_Can_Transfer_Non_Negative_Zero_Value);
    RUN_TEST(TAY_Can_Transfer_Negative_Value);

    return UNITY_END();
}