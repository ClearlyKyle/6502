#include "Unity/unity.h"
#include "h6502.h"

#include <stdbool.h>

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU(&cpu, &mem);
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

void BEQ_Can_Branch_Forward_When_Zero_Is_Set(void)
{
    cpu.Z = 1;

    mem.data[0xFF00] = INS_BEQ;
    mem.data[0xFF01] = 0x1;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 3;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT16(0xFF03, cpu.program_counter);
    TEST_ASSERT_EQUAL_UINT8(before.PS, cpu.PS);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(BEQ_Can_Branch_Forward_When_Zero_Is_Set);

    return UNITY_END();
}