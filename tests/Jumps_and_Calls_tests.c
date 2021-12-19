#include "Unity/unity.h"
#include "h6502.h"

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU(&cpu, &mem);
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

void RunLittleProgram(void)
{
    // start - little program
    mem.data[0xFFFC] = INS_JSR;
    mem.data[0xFFFD] = 0x42;
    mem.data[0xFFFE] = 0x42;
    mem.data[0x4242] = INS_LDA_IM;
    mem.data[0x4243] = 0x84;
    // end - little program

    const int NUM_CYCLES = 8;

    s32 cycles_used = Execute(NUM_CYCLES, &mem);
    TEST_ASSERT_EQUAL_INT32(NUM_CYCLES, cycles_used);

    // TEST_ASSERT_EQUAL(expected, actual)
    TEST_ASSERT_EQUAL(0x84, cpu.accumulator);
    TEST_ASSERT_EQUAL_HEX8(0x84, cpu.accumulator);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(RunLittleProgram);

    return UNITY_END();
}