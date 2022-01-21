#include "Unity/unity.h"
#include "h6502.h"

#include <stdbool.h>

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU(&cpu, &mem);
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

void NOP_Will_Do_Nothing_But_Consume_A_Cycle(void)
{
    // given:
    cpu.program_counter = 0xFF00;

    mem.data[0xFF00] = INS_NOP;

    // when:
    const CPU before = cpu;
    const s32 NUM_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_CYCLES, cycles_used);

    TEST_ASSERT_EQUAL_UINT8(before.PS, cpu.PS);
    TEST_ASSERT_EQUAL_UINT16(before.program_counter + 0x01, cpu.program_counter);
    TEST_ASSERT_EQUAL_UINT8(before.accumulator, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(before.index_reg_X, cpu.index_reg_X);
    TEST_ASSERT_EQUAL_UINT8(before.index_reg_Y, cpu.index_reg_Y);
    TEST_ASSERT_EQUAL_UINT8(before.stack_pointer, cpu.stack_pointer);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(NOP_Will_Do_Nothing_But_Consume_A_Cycle);

    return UNITY_END();
}