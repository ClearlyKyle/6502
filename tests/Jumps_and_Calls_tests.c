#include "Unity/unity.h"
#include "h6502.h"

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU(&cpu, &mem);
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

void Can_Jump_To_A_Subroutine_And_Jump_Back_Again(void)
{
    Reset_CPU(&cpu, &mem);
    cpu.program_counter = 0xFF00;
    mem.data[0xFF00] = INS_JSR;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = INS_RTS;
    mem.data[0xFF03] = INS_LDA_IM;
    mem.data[0xFF04] = 0x42;

    const CPU cpu_before = cpu;
    const s32 EXPECTED_CYCLES = 6 + 6 + 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_HEX8(0x42, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(cpu_before.stack_pointer, cpu.stack_pointer);
}

void JSR_Does_Not_Affect_The_Processor_Status(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    mem.data[0xFF00] = INS_JSR;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;

    const CPU cpu_before = cpu;
    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_NOT_EQUAL_UINT8(cpu_before.stack_pointer, cpu.stack_pointer);
    TEST_ASSERT_EQUAL_HEX16(0x8000, cpu.program_counter);

    // EXPECT_EQ(cpu.PS, CPUCopy.PS);
}

void RTS_Does_Not_Affect_The_Processor_Status(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    mem.data[0xFF00] = INS_JSR;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = INS_RTS;

    const CPU cpu_before = cpu;
    const s32 EXPECTED_CYCLES = 6 + 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    // EXPECT_EQ(cpu.PS, CPUCopy.PS);
    TEST_ASSERT_EQUAL_HEX16(0xFF03, cpu.program_counter);
}

void Jump_Absolute_Can_Jump_To_A_New_Location_In_The_Program(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    mem.data[0xFF00] = INS_JMP_ABS;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;

    const CPU cpu_before = cpu;
    const s32 EXPECTED_CYCLES = 3;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(cpu_before.stack_pointer, cpu.stack_pointer);
    // EXPECT_EQ(cpu.PS, CPUCopy.PS);
    TEST_ASSERT_EQUAL_HEX16(0x8000, cpu.program_counter);
}

void Jump_Indirect_Can_Jump_To_A_New_Location_In_The_Program(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    mem.data[0xFF00] = INS_JMP_IND;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = 0x00;
    mem.data[0x8001] = 0x90;

    const CPU cpu_before = cpu;
    const s32 EXPECTED_CYCLES = 5;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(cpu_before.stack_pointer, cpu.stack_pointer);
    // EXPECT_EQ(cpu.PS, CPUCopy.PS);
    TEST_ASSERT_EQUAL_HEX16(0x9000, cpu.program_counter);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(Can_Jump_To_A_Subroutine_And_Jump_Back_Again);
    RUN_TEST(JSR_Does_Not_Affect_The_Processor_Status);
    RUN_TEST(RTS_Does_Not_Affect_The_Processor_Status);
    RUN_TEST(Jump_Absolute_Can_Jump_To_A_New_Location_In_The_Program);
    RUN_TEST(Jump_Indirect_Can_Jump_To_A_New_Location_In_The_Program);

    return UNITY_END();
}