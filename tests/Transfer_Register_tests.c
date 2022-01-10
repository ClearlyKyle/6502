#include "Unity/unity.h"
#include "h6502.h"

#include <stdbool.h>

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU(&cpu, &mem);
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

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
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(TAX_Can_Transfer_Non_Negative_Non_Zero_Value);

    return UNITY_END();
}