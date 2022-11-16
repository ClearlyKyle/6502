#include "Unity/unity.h"
#include "h6502.h"

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU();
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

// Arithmetic shift left
void ASL_Can_Shift_The_Value_Of_One(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 1;
    cpu.accumulator     = 1;

    mem.data[0xFF00] = INS_ASL;

    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(cpu.accumulator, 2);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ASL_Can_Shift_A_Negative_Value(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 1;
    cpu.N               = 0;
    cpu.accumulator     = 0b11000010;

    mem.data[0xFF00] = INS_ASL;

    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);

    TEST_ASSERT_EQUAL_INT8(cpu.accumulator, 0b10000100);
    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}
int main(void)
{
    UNITY_BEGIN();

    // Arithmetic shift left
    RUN_TEST(ASL_Can_Shift_The_Value_Of_One);

    return UNITY_END();
}