#include "Unity/unity.h"
#include "h6502.h"

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU(&cpu, &mem);
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

void TSX_Can_Transfer_The_Stack_Pointer_To_X_Register(void)
{
    cpu.program_counter = 0xFF00;

    cpu.Z = 1;
    cpu.N = 1;
    cpu.index_reg_X = 0x00;
    cpu.stack_pointer = 0x01;
    mem.data[0xFF00] = INS_TSX;

    const CPU cpu_before = cpu;
    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(0x01, cpu.index_reg_X);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void TSX_Can_Transfer_The_Stack_Pointer_To_X_Register_Setting_Flags(void)
{
    cpu.program_counter = 0xFF00;

    cpu.Z = 1;
    cpu.N = 1;
    cpu.index_reg_X = 0x00;
    cpu.stack_pointer = 0x00;
    mem.data[0xFF00] = INS_TSX;

    const CPU cpu_before = cpu;
    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(0x00, cpu.index_reg_X);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void TSX_Can_Transfer_A_Negative_Stack_Pointer_To_X_Register(void)
{
    cpu.program_counter = 0xFF00;

    cpu.Z = 0;
    cpu.N = 0;
    cpu.index_reg_X = 0x00;
    cpu.stack_pointer = 0b10000000; // binary
    mem.data[0xFF00] = INS_TSX;

    const CPU cpu_before = cpu;
    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(cpu.index_reg_X, 0b10000000);
    TEST_ASSERT_FALSE(cpu.Z); // Z
    TEST_ASSERT_TRUE(cpu.N);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(TSX_Can_Transfer_The_Stack_Pointer_To_X_Register);
    RUN_TEST(TSX_Can_Transfer_The_Stack_Pointer_To_X_Register_Setting_Flags);
    RUN_TEST(TSX_Can_Transfer_A_Negative_Stack_Pointer_To_X_Register);

    return UNITY_END();
}