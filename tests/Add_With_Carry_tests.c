#include "Unity/unity.h"
#include "h6502.h"

#include <stdbool.h>

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU(&cpu, &mem);
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

static void Verify_Unmodified_Flags(const CPU before, const CPU after)
{
    TEST_ASSERT_EQUAL_UINT8(before.I, after.I);
    TEST_ASSERT_EQUAL_UINT8(before.D, after.D);
    TEST_ASSERT_EQUAL_UINT8(before.B, after.B);
}

void ADC_Can_Ass_Zero_To_Zero_To_Get_Zero(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.accumulator = 0;
    cpu.C = 0;
    cpu.Z = 1;
    cpu.N = 1;

    mem.data[0xFF00] = INS_ADC_ABS;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = 0x00;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x00, cpu.accumulator);
    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
    TEST_ASSERT_FALSE(cpu.V);

    Verify_Unmodified_Flags(before, cpu);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(ADC_Can_Ass_Zero_To_Zero_To_Get_Zero);

    return UNITY_END();
}