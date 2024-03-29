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

// LSR (Logical Shift Right)
void LSR_Can_Shift_The_Value_Of_One(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 0;
    cpu.N               = 1;
    cpu.accumulator     = 1;

    mem.data[0xFF00] = INS_LSR;

    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(cpu.accumulator, 0);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void LSR_Can_Shift_A_Zero_Into_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 1;
    cpu.accumulator     = 8;

    mem.data[0xFF00] = INS_LSR;

    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(cpu.accumulator, 4);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void LSR_ZP_Can_Shift_The_Value_Of_One(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 0;
    cpu.N               = 1;

    mem.data[0xFF00] = INS_LSR_ZP;
    mem.data[0xFF01] = 0x42;
    mem.data[0x0042] = 1;

    const s32 EXPECTED_CYCLES = 5;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x0042], 0);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void LSR_ZP_Can_Shift_A_Zero_Into_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 1;

    mem.data[0xFF00] = INS_LSR_ZP;
    mem.data[0xFF01] = 0x42;
    mem.data[0x0042] = 8;

    const s32 EXPECTED_CYCLES = 5;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x0042], 4);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void LSR_ZP_X_Can_Shift_The_Value_Of_One(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 0;
    cpu.N               = 1;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_LSR_ZP_X;
    mem.data[0xFF01]        = 0x42;
    mem.data[0x0042 + 0x10] = 1;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x0042 + 0x10], 0);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void LSR_ZP_X_Can_Shift_A_Zero_Into_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 1;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_LSR_ZP_X;
    mem.data[0xFF01]        = 0x42;
    mem.data[0x0042 + 0x10] = 8;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x0042 + 0x10], 4);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void LSR_ABS_Can_Shift_The_Value_Of_One(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 0;
    cpu.N               = 1;

    mem.data[0xFF00] = INS_LSR_ABS;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = 1;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x8000], 0);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void LSR_ABS_Can_Shift_A_Zero_Into_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 1;

    mem.data[0xFF00] = INS_LSR_ABS;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = 8;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x8000], 4);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void LSR_ABS_X_Can_Shift_The_Value_Of_One(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 0;
    cpu.N               = 1;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_LSR_ABS_X;
    mem.data[0xFF01]        = 0x00;
    mem.data[0xFF02]        = 0x80;
    mem.data[0x8000 + 0x10] = 1;

    const s32 EXPECTED_CYCLES = 7;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x8000 + 0x10], 0);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void LSR_ABS_X_Can_Shift_A_Zero_Into_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 1;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_LSR_ABS_X;
    mem.data[0xFF01]        = 0x00;
    mem.data[0xFF02]        = 0x80;
    mem.data[0x8000 + 0x10] = 8;

    const s32 EXPECTED_CYCLES = 7;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x8000 + 0x10], 4);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

// ROL (ROtate Left)
void ROL_Can_Shift_A_Bit_Out_Of_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 1;
    cpu.accumulator     = 0;

    mem.data[0xFF00] = INS_ROL;

    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(cpu.accumulator, 1);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ROL_Can_Shift_A_Bit_Into_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 0;
    cpu.N               = 1;
    cpu.accumulator     = 0b10000000;

    mem.data[0xFF00] = INS_ROL;

    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(cpu.accumulator, 0);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ROL_Can_Shift_Zero_With_No_Carry(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 0;
    cpu.N               = 1;
    cpu.accumulator     = 0;

    mem.data[0xFF00] = INS_ROL;

    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(cpu.accumulator, 0);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ROL_Can_Shift_A_Value_That_Results_In_A_Negative_Value(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 0;
    cpu.N               = 0;
    cpu.accumulator     = 0b01110011;

    mem.data[0xFF00] = INS_ROL;

    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(cpu.accumulator, 0b11100111);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}

void ROL_ZP_Can_Shift_A_Bit_Out_Of_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 1;

    mem.data[0xFF00] = INS_ROL_ZP;
    mem.data[0xFF01] = 0x42;
    mem.data[0x0042] = 0;

    const s32 EXPECTED_CYCLES = 5;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x0042], 1);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ROL_ZP_Can_Shift_A_Bit_Into_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter       = 0xFF00;
    cpu.C                     = 0;
    cpu.Z                     = 0;
    cpu.N                     = 1;
    mem.data[0xFF00]          = INS_ROL_ZP;
    mem.data[0xFF01]          = 0x42;
    mem.data[0x0042]          = 0b10000000;
    const s32 EXPECTED_CYCLES = 5;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x0042], 0);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ROL_ZP_Can_Shift_Zero_With_No_Carry(void)
{
    // given:
    cpu.program_counter       = 0xFF00;
    cpu.C                     = 0;
    cpu.Z                     = 0;
    cpu.N                     = 1;
    mem.data[0xFF00]          = INS_ROL_ZP;
    mem.data[0xFF01]          = 0x42;
    mem.data[0x0042]          = 0;
    const s32 EXPECTED_CYCLES = 5;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x0042], 0);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ROL_ZP_Can_Shift_A_Value_That_Result_In_A_Negative_Value(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 0;
    cpu.N               = 0;

    mem.data[0xFF00] = INS_ROL_ZP;
    mem.data[0xFF01] = 0x42;
    mem.data[0x0042] = 0b01110011;

    const s32 EXPECTED_CYCLES = 5;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x0042], 0b11100111);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}

void ROL_ZP_X_Can_Shift_A_Bit_Out_Of_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 1;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_ROL_ZP_X;
    mem.data[0xFF01]        = 0x42;
    mem.data[0x0042 + 0x10] = 0;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x0042 + 0x10], 1);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ROL_ZP_X_Can_Shift_A_Bit_Into_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 0;
    cpu.N               = 1;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_ROL_ZP_X;
    mem.data[0xFF01]        = 0x42;
    mem.data[0x0042 + 0x10] = 0b10000000;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x0042 + 0x10], 0);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ROL_ZP_X_Can_Shift_Zero_With_No_Carry(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 0;
    cpu.N               = 1;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_ROL_ZP_X;
    mem.data[0xFF01]        = 0x42;
    mem.data[0x0042 + 0x10] = 0;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x0042 + 0x10], 0);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ROL_ZP_X_Can_Shift_A_Value_That_Result_In_A_Negative_Value(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 0;
    cpu.N               = 0;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_ROL_ZP_X;
    mem.data[0xFF01]        = 0x42;
    mem.data[0x0042 + 0x10] = 0b01110011;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    TEST_ASSERT_EQUAL(mem.data[0x0042 + 0x10], 0b11100111);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}

// ROR (ROtate Right)
void ROR_Can_Shift_The_Carry_Flag_Into_The_Operand(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 0;
    cpu.N               = 0;
    cpu.accumulator     = 0;

    mem.data[0xFF00] = INS_ROR;

    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    EXPECT_EQ(cpu.accumulator, 0b10000000);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}

void ROR_Can_Shift_A_Value_Into_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 0;
    cpu.N               = 0;
    cpu.accumulator     = 1;

    mem.data[0xFF00] = INS_ROR;

    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    EXPECT_EQ(cpu.accumulator, 0);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ROR_Can_Rotate_A_Number(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 0;
    cpu.accumulator     = 0b01101101;

    mem.data[0xFF00] = INS_ROR;

    const s32 EXPECTED_CYCLES = 2;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    EXPECT_EQ(cpu.accumulator, 0b10110110);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}

void ROR_ZP_Can_Shift_The_Carry_Flag_Into_The_Operand(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 0;
    cpu.N               = 0;

    mem.data[0xFF00] = INS_ROR_ZP;
    mem.data[0xFF01] = 0x42;
    mem.data[0x0042] = 0;

    const s32 EXPECTED_CYCLES = 5;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    EXPECT_EQ(mem.data[0x0042], 0b10000000);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}

void ROR_ZP_Can_Shift_A_Value_Into_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 0;
    cpu.N               = 0;

    mem.data[0xFF00] = INS_ROR_ZP;
    mem.data[0xFF01] = 0x42;
    mem.data[0x0042] = 1;

    const s32 EXPECTED_CYCLES = 5;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    EXPECT_EQ(mem.data[0x0042], 0);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ROR_ZP_Can_Rotate_A_Number(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 0;

    mem.data[0xFF00] = INS_ROR_ZP;
    mem.data[0xFF01] = 0x42;
    mem.data[0x0042] = 0b01101101;

    const s32 EXPECTED_CYCLES = 5;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    EXPECT_EQ(mem.data[0x0042], 0b10110110);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}

void ROR_ZP_X_Can_Shift_The_Carry_Flag_Into_The_Operand(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 0;
    cpu.N               = 0;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_ROR_ZP_X;
    mem.data[0xFF01]        = 0x42;
    mem.data[0x0042 + 0x10] = 0;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    EXPECT_EQ(mem.data[0x0042 + 0x10], 0b10000000);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}

void ROR_ZP_X_Can_Shift_A_Value_Into_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 0;
    cpu.N               = 0;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_ROR_ZP_X;
    mem.data[0xFF01]        = 0x42;
    mem.data[0x0042 + 0x10] = 1;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    EXPECT_EQ(mem.data[0x0042 + 0x10], 0);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ROR_ZP_X_Can_Rotate_A_Number(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 0;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_ROR_ZP_X;
    mem.data[0xFF01]        = 0x42;
    mem.data[0x0042 + 0x10] = 0b01101101;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    EXPECT_EQ(mem.data[0x0042 + 0x10], 0b10110110);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}

void ROR_ABS_Can_Shift_The_Carry_Flag_Into_The_Operand(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 0;
    cpu.N               = 0;

    mem.data[0xFF00] = INS_ROR_ABS;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = 0;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    EXPECT_EQ(mem.data[0x8000], 0b10000000);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}

void ROR_ABS_Can_Shift_A_Value_Into_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 0;
    cpu.N               = 0;

    mem.data[0xFF00] = INS_ROR_ABS;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = 1;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    EXPECT_EQ(mem.data[0x8000], 0);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
}

void ROR_ABS_Can_Rotate_A_Number(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 1;
    cpu.N               = 0;

    mem.data[0xFF00] = INS_ROR_ABS;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = 0b01101101;

    const s32 EXPECTED_CYCLES = 6;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    EXPECT_EQ(mem.data[0x8000], 0b10110110);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}

void ROR_ABS_X_Can_Shift_The_Carry_Flag_Into_The_Operand(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 1;
    cpu.Z               = 0;
    cpu.N               = 0;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_ROR_ABS_X;
    mem.data[0xFF01]        = 0x00;
    mem.data[0xFF02]        = 0x80;
    mem.data[0x8000 + 0x10] = 0;

    const s32 EXPECTED_CYCLES = 7;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    EXPECT_EQ(mem.data[0x8000 + 0x10], 0b10000000);

    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);
}

void ROR_ABS_X_Can_Shift_A_Value_Into_The_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = 0;
    cpu.Z               = 0;
    cpu.N               = 0;
    cpu.index_reg_X     = 0x10;

    mem.data[0xFF00]        = INS_ROR_ABS_X;
    mem.data[0xFF01]        = 0x00;
    mem.data[0xFF02]        = 0x80;
    mem.data[0x8000 + 0x10] = 1;

    const s32 EXPECTED_CYCLES = 7;

    // when:
    const s32 actual_cycles = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, actual_cycles);
    EXPECT_EQ(mem.data[0x8000 + 0x10], 0);

    TEST_ASSERT_TRUE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
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

    // LSR (Logical Shift Right)
    RUN_TEST(LSR_Can_Shift_The_Value_Of_One);
    RUN_TEST(LSR_Can_Shift_A_Zero_Into_The_Carry_Flag);
    RUN_TEST(LSR_ZP_Can_Shift_The_Value_Of_One);
    RUN_TEST(LSR_ZP_Can_Shift_A_Zero_Into_The_Carry_Flag);
    RUN_TEST(LSR_ZP_X_Can_Shift_The_Value_Of_One);
    RUN_TEST(LSR_ZP_X_Can_Shift_A_Zero_Into_The_Carry_Flag);
    RUN_TEST(LSR_ABS_Can_Shift_The_Value_Of_One);
    RUN_TEST(LSR_ABS_Can_Shift_A_Zero_Into_The_Carry_Flag);
    RUN_TEST(LSR_ABS_X_Can_Shift_The_Value_Of_One);
    RUN_TEST(LSR_ABS_X_Can_Shift_A_Zero_Into_The_Carry_Flag);

    // ROL (ROtate Left)
    RUN_TEST(ROL_Can_Shift_A_Bit_Out_Of_The_Carry_Flag);
    RUN_TEST(ROL_Can_Shift_A_Bit_Into_The_Carry_Flag);
    RUN_TEST(ROL_Can_Shift_Zero_With_No_Carry);
    RUN_TEST(ROL_Can_Shift_A_Value_That_Results_In_A_Negative_Value);
    RUN_TEST(ROL_ZP_Can_Shift_A_Bit_Out_Of_The_Carry_Flag);
    RUN_TEST(ROL_ZP_Can_Shift_A_Bit_Into_The_Carry_Flag);
    RUN_TEST(ROL_ZP_Can_Shift_Zero_With_No_Carry);
    RUN_TEST(ROL_ZP_Can_Shift_A_Value_That_Result_In_A_Negative_Value);
    RUN_TEST(ROL_ZP_X_Can_Shift_A_Bit_Out_Of_The_Carry_Flag);
    RUN_TEST(ROL_ZP_X_Can_Shift_A_Bit_Into_The_Carry_Flag);
    RUN_TEST(ROL_ZP_X_Can_Shift_Zero_With_No_Carry);
    RUN_TEST(ROL_ZP_X_Can_Shift_A_Value_That_Result_In_A_Negative_Value);

    // ROR (ROtate Right)
    RUN_TESTS(ROR_Can_Shift_The_Carry_Flag_Into_The_Operand);
    RUN_TESTS(ROR_Can_Shift_A_Value_Into_The_Carry_Flag);
    RUN_TESTS(ROR_Can_Rotate_A_Number);
    RUN_TESTS(ROR_ZP_Can_Shift_The_Carry_Flag_Into_The_Operand);
    RUN_TESTS(ROR_ZP_Can_Shift_A_Value_Into_The_Carry_Flag);
    RUN_TESTS(ROR_ZP_Can_Rotate_A_Number);
    RUN_TESTS(ROR_ZP_X_Can_Shift_The_Carry_Flag_Into_The_Operand);
    RUN_TESTS(ROR_ZP_X_Can_Shift_A_Value_Into_The_Carry_Flag);
    RUN_TESTS(ROR_ZP_X_Can_Rotate_A_Number);
    RUN_TESTS(ROR_ABS_Can_Shift_The_Carry_Flag_Into_The_Operand);
    RUN_TESTS(ROR_ABS_Can_Shift_A_Value_Into_The_Carry_Flag);
    RUN_TESTS(ROR_ABS_Can_Rotate_A_Number);
    RUN_TESTS(ROR_ABS_X_Can_Shift_The_Carry_Flag_Into_The_Operand);
    RUN_TESTS(ROR_ABS_X_Can_Shift_A_Value_Into_The_Carry_Flag);
    return UNITY_END();
}