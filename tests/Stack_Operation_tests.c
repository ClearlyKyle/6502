#include "Unity/unity.h"
#include "h6502.h"

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU();
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

void TSX_Can_Transfer_The_Stack_Pointer_To_X_Register(void)
{
    cpu.program_counter = 0xFF00;

    cpu.Z             = 1;
    cpu.N             = 1;
    cpu.index_reg_X   = 0x00;
    cpu.stack_pointer = 0x01;
    mem.data[0xFF00]  = INS_TSX;

    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(0x01, cpu.index_reg_X);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void TSX_Can_Transfer_The_Stack_Pointer_To_X_Register_Setting_Flags(void)
{
    cpu.program_counter = 0xFF00;

    cpu.Z             = 1;
    cpu.N             = 1;
    cpu.index_reg_X   = 0x00;
    cpu.stack_pointer = 0x00;
    mem.data[0xFF00]  = INS_TSX;

    // when:
    const s32 EXPECTED_CYCLES = 2;

    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(0x00, cpu.index_reg_X);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void TSX_Can_Transfer_A_Negative_Stack_Pointer_To_X_Register(void)
{
    cpu.program_counter = 0xFF00;

    cpu.Z             = 0;
    cpu.N             = 0;
    cpu.index_reg_X   = 0x00;
    cpu.stack_pointer = NEGATIVE_FLAG_BIT; // binary
    mem.data[0xFF00]  = INS_TSX;

    // when:
    const s32 EXPECTED_CYCLES = 2;

    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(cpu.index_reg_X, NEGATIVE_FLAG_BIT);
    TEST_ASSERT_FALSE(cpu.Z); // Z
    TEST_ASSERT_TRUE(cpu.N);
}

void TXS_Can_Transfer_X_Register_To_Stack_Pointer(void)
{
    cpu.program_counter = 0xFF00;

    cpu.index_reg_X   = 0xFF;
    cpu.stack_pointer = 0;
    mem.data[0xFF00]  = INS_TXS;

    const CPU cpu_before      = cpu;
    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(0xFF, cpu.stack_pointer);
    TEST_ASSERT_EQUAL_UINT8(cpu_before.PS, cpu.PS);
}

void PHA_Can_Push_A_Register_Onto_Stack(void)
{
    cpu.program_counter = 0xFF00;

    cpu.accumulator  = 0x42;
    mem.data[0xFF00] = INS_PHA;

    const CPU cpu_before      = cpu;
    const s32 EXPECTED_CYCLES = 3;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(0x42, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(mem.data[SP_To_Address() + 1], cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(cpu_before.PS, cpu.PS);
    TEST_ASSERT_EQUAL_UINT8(0xFE, cpu.stack_pointer);
}

void PLA_Can_Pull_A_Value_From_Stack_And_Put_Into_A_Register(void)
{
    cpu.program_counter = 0xFF00;

    cpu.accumulator   = 0x00;
    cpu.stack_pointer = 0xFE;

    mem.data[0x01FF] = 0x42;
    mem.data[0xFF00] = INS_PLA;

    // when:
    const s32 EXPECTED_CYCLES = 4;

    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(0x42, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(0xFF, cpu.stack_pointer);
}

void PLA_Can_Pull_A_Zero_Value_From_Stack_And_Put_Into_A_Register(void)
{
    cpu.program_counter = 0xFF00;

    cpu.accumulator   = 0x42;
    cpu.stack_pointer = 0xFE;
    cpu.Z             = 0;
    cpu.N             = 1;

    mem.data[0x01FF] = 0x00;
    mem.data[0xFF00] = INS_PLA;

    // when:
    const s32 EXPECTED_CYCLES = 4;

    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(0x00, cpu.accumulator);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
    TEST_ASSERT_EQUAL_UINT8(0xFF, cpu.stack_pointer);
}

void PLA_Can_Pull_A_Negative_Value_From_Stack_And_Put_Into_A_Register(void)
{
    cpu.program_counter = 0xFF00;

    cpu.accumulator   = 0x42;
    cpu.stack_pointer = 0xFE;
    cpu.N             = 0;
    cpu.Z             = 1;

    mem.data[0x01FF] = 0x81; // 0x81 = 0b10000001
    mem.data[0xFF00] = INS_PLA;

    // when:
    const s32 EXPECTED_CYCLES = 4;

    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(0x81, cpu.accumulator); // 0x81 = 0b10000001
    TEST_ASSERT_TRUE(cpu.N);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_EQUAL_UINT8(0xFF, cpu.stack_pointer);
}

void PHP_Can_Push_Processor_Status_Onto_Stack(void)
{
    cpu.program_counter = 0xFF00;

    cpu.PS           = 0xCC; // random
    mem.data[0xFF00] = INS_PHP;

    const CPU cpu_before      = cpu;
    const s32 EXPECTED_CYCLES = 3;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(0xCC, mem.data[SP_To_Address() + 1]);
    TEST_ASSERT_EQUAL_UINT8(cpu_before.PS, cpu.PS);
    TEST_ASSERT_EQUAL_UINT8(0xFE, cpu.stack_pointer);
}

void PLP_Can_Pull_Value_From_Stack_And_Put_Into_Processor_Status(void)
{
    cpu.program_counter = 0xFF00;

    cpu.stack_pointer = 0xFE;
    cpu.PS            = 0;

    mem.data[0x01FF] = 0x42;
    mem.data[0xFF00] = INS_PLP;

    // when:
    const s32 EXPECTED_CYCLES = 4;

    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL_UINT8(0x42, cpu.PS);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(TSX_Can_Transfer_The_Stack_Pointer_To_X_Register);
    RUN_TEST(TSX_Can_Transfer_The_Stack_Pointer_To_X_Register_Setting_Flags);
    RUN_TEST(TSX_Can_Transfer_A_Negative_Stack_Pointer_To_X_Register);

    RUN_TEST(TXS_Can_Transfer_X_Register_To_Stack_Pointer);

    RUN_TEST(PHA_Can_Push_A_Register_Onto_Stack);

    RUN_TEST(PLA_Can_Pull_A_Value_From_Stack_And_Put_Into_A_Register);
    RUN_TEST(PLA_Can_Pull_A_Zero_Value_From_Stack_And_Put_Into_A_Register);
    RUN_TEST(PLA_Can_Pull_A_Negative_Value_From_Stack_And_Put_Into_A_Register);

    RUN_TEST(PHP_Can_Push_Processor_Status_Onto_Stack);

    RUN_TEST(PLP_Can_Pull_Value_From_Stack_And_Put_Into_Processor_Status);

    return UNITY_END();
}