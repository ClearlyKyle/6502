#include "Unity/unity.h"
#include "h6502.h"

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    reset_cpu(&cpu, &mem);
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

    execute(9, &mem);

    // TEST_ASSERT_EQUAL(expected, actual)
    TEST_ASSERT_EQUAL(0x84, cpu.accumulator);
    TEST_ASSERT_EQUAL_HEX8(0x84, cpu.accumulator);
}

void LDA_Immediate_Can_Load_A_Value_Into_A_Register(void)
{
    mem.data[0xFFFC] = INS_LDA_IM;
    mem.data[0xFFFD] = 0x84;

    execute(2, &mem);

    // TEST_ASSERT_EQUAL(expected, actual)
    TEST_ASSERT_EQUAL_HEX8(0x84, cpu.accumulator);
}

void LDAZ_Can_Load_A_Value_Into_A_Register(void)
{
    mem.data[0xFFFC] = INS_LDA_ZP;
    mem.data[0xFFFD] = 0x42;
    mem.data[0x0042] = 0x37;

    execute(3, &mem);

    // TEST_ASSERT_EQUAL(expected, actual)
    TEST_ASSERT_EQUAL_HEX8(0x37, cpu.accumulator);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(RunLittleProgram);
    RUN_TEST(LDA_Immediate_Can_Load_A_Value_Into_A_Register);
    RUN_TEST(LDAZ_Can_Load_A_Value_Into_A_Register);
    return UNITY_END();
}