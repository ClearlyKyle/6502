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

void ASL_ZP_Can_Shift_The_Value_Of_One(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 1;

    mem.data[0xFF00] = INS_ASL_ZP;
    mem.data[0xFF01] = 0x42;
    mem.data[0x0042] = 1;

    const s32 EXPECTED_CYCLES = 5;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);

    TEST_ASSERT_EQUAL(mem.data[0x0042], 2);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ASL_ZP_Can_Shift_A_Negative_Value(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 1;
    cpu.N               = 0;

    mem.data[0xFF00] = INS_ASL_ZP;
    mem.data[0xFF01] = 0x42;
    mem.data[0x0042] = 0b11000010;

    const s32 EXPECTED_CYCLES = 5;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);

    TEST_ASSERT_EQUAL(mem.data[0x0042], 0b10000100);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}

void ASL_ZP_X_Can_Shift_The_Value_Of_One(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 1;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_ASL_ZP_X;
    mem.data[0xFF01]        = 0x42;
    mem.data[0x0042 + 0x10] = 1;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);

    TEST_ASSERT_EQUAL(mem.data[0x0042 + 0x10], 2);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ASL_ZP_X_Can_Shift_A_Negative_Value(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 1;
    cpu.N               = 0;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_ASL_ZP_X;
    mem.data[0xFF01]        = 0x42;
    mem.data[0x0042 + 0x10] = 0b11000010;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);

    TEST_ASSERT_EQUAL(mem.data[0x0042 + 0x10], 0b10000100);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}

void ASL_ABS_Can_Shift_The_Value_Of_One(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 1;

    mem.data[0xFF00] = INS_ASL_ABS;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = 1;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);

    TEST_ASSERT_EQUAL(mem.data[0x8000], 2);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ASL_ABS_Can_Shift_A_Negative_Value(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 1;
    cpu.N               = 0;

    mem.data[0xFF00] = INS_ASL_ABS;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = 0b11000010;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);

    TEST_ASSERT_EQUAL(mem.data[0x8000], 0b10000100);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}

void ASL_ABS_X_Can_Shift_The_Value_Of_One(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 1;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_ASL_ABS_X;
    mem.data[0xFF01]        = 0x00;
    mem.data[0xFF02]        = 0x80;
    mem.data[0x8000 + 0x10] = 1;

    const s32 EXPECTED_CYCLES = 7;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);

    TEST_ASSERT_EQUAL(mem.data[0x8000 + 0x10], 2);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ASL_ABS_X_Can_Shift_A_Negative_Value(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 1;
    cpu.N               = 0;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_ASL_ABS_X;
    mem.data[0xFF01]        = 0x00;
    mem.data[0xFF02]        = 0x80;
    mem.data[0x8000 + 0x10] = 0b11000010;

    const s32 EXPECTED_CYCLES = 7;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);

    TEST_ASSERT_EQUAL(mem.data[0x8000 + 0x10], 0b10000100);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}
int main(void)
{
    UNITY_BEGIN();

    // Arithmetic shift left
    RUN_TEST(ASL_Can_Shift_The_Value_Of_One);
    RUN_TEST(ASL_Can_Shift_A_Negative_Value);
    RUN_TEST(ASL_ZP_Can_Shift_The_Value_Of_One);
    RUN_TEST(ASL_ZP_Can_Shift_A_Negative_Value);
    RUN_TEST(ASL_ZP_X_Can_Shift_The_Value_Of_One);
    RUN_TEST(ASL_ZP_X_Can_Shift_A_Negative_Value);
    RUN_TEST(ASL_ABS_Can_Shift_The_Value_Of_One);
    RUN_TEST(ASL_ABS_Can_Shift_A_Negative_Value);
    RUN_TEST(ASL_ABS_X_Can_Shift_The_Value_Of_One);
    RUN_TEST(ASL_ABS_X_Can_Shift_A_Negative_Value);

    return UNITY_END();
}