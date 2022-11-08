#include "Unity/unity.h"
#include "h6502.h"

#include <stdbool.h>

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU();
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

struct CMP_Test_Data
{
    u8 register_value;
    u8 operand;

    bool expect_C;
    bool expect_Z;
    bool expect_N;
};

enum Register
{
    REGISTER_A,
    REGISTER_X,
    REGISTER_Y
};

static void Check_Unaffected_Registers(const CPU before)
{
    TEST_ASSERT_EQUAL_UINT8(before.I, cpu.I);
    TEST_ASSERT_EQUAL_UINT8(before.D, cpu.D);
    TEST_ASSERT_EQUAL_UINT8(before.B, cpu.B);
    TEST_ASSERT_EQUAL_UINT8(before.V, cpu.V);
}

static struct CMP_Test_Data Compare_Two_Identical_Values(void)
{
    struct CMP_Test_Data test;
    test.register_value = 26;
    test.operand        = 26;
    test.expect_Z       = true;
    test.expect_N       = false;
    test.expect_C       = true;
    return test;
}

static struct CMP_Test_Data Compare_A_Large_Positive_To_A_Small_Positive(void)
{
    struct CMP_Test_Data Test;
    Test.register_value = 48;
    Test.operand        = 26;
    Test.expect_Z       = false;
    Test.expect_N       = false;
    Test.expect_C       = true;
    return Test;
}

static struct CMP_Test_Data Compare_A_Negative_Number_To_A_Positive(void)
{
    struct CMP_Test_Data Test;
    Test.register_value = 130; // Negative number!
    Test.operand        = 26;
    Test.expect_Z       = false;
    Test.expect_N       = false;
    Test.expect_C       = true;
    return Test;
}

static struct CMP_Test_Data Compare_Two_Values_That_Result_In_A_Negative_Flag_Set(void)
{
    struct CMP_Test_Data Test;
    Test.register_value = 8;
    Test.operand        = 26;
    Test.expect_Z       = false;
    Test.expect_N       = true;
    Test.expect_C       = false;
    return Test;
}

static void CMP_IM(struct CMP_Test_Data test, enum Register register_to_compare)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = !test.expect_C;
    cpu.Z               = !test.expect_Z;
    cpu.N               = !test.expect_N;

    u8 *reg    = &cpu.accumulator;
    u8  opcode = INS_CMP_IM;
    switch (register_to_compare)
    {
    case REGISTER_X:
        reg    = &cpu.index_reg_X;
        opcode = INS_CPX_IM;
        break;
    case REGISTER_Y:
        reg    = &cpu.index_reg_Y;
        opcode = INS_CPY_IM;
        break;
    };
    *reg = test.register_value;

    mem.data[0xFF00] = opcode;
    mem.data[0xFF01] = test.operand;

    const s32 EXPECTED_CYCLES = 2;
    const CPU cpu_before      = cpu;

    // when:
    const s32 cycles_used = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(test.register_value, *reg);
    TEST_ASSERT_EQUAL_UINT8(test.expect_Z, cpu.Z);
    TEST_ASSERT_EQUAL_UINT8(test.expect_N, cpu.N);
    TEST_ASSERT_EQUAL_UINT8(test.expect_C, cpu.C);

    Check_Unaffected_Registers(cpu_before);
}

static void CMP_ZP(struct CMP_Test_Data test, enum Register register_to_compare)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = !test.expect_C;
    cpu.Z               = !test.expect_Z;
    cpu.N               = !test.expect_N;

    u8 *reg    = &cpu.accumulator;
    u8  opcode = INS_CMP_ZP;
    switch (register_to_compare)
    {
    case REGISTER_X:
        reg    = &cpu.index_reg_X;
        opcode = INS_CPX_ZP;
        break;
    case REGISTER_Y:
        reg    = &cpu.index_reg_Y;
        opcode = INS_CPY_ZP;
        break;
    };
    *reg = test.register_value;

    mem.data[0xFF00] = opcode;
    mem.data[0xFF01] = 0x42;
    mem.data[0x0042] = test.operand;

    const s32 EXPECTED_CYCLES = 3;
    const CPU cpu_before      = cpu;

    // when:
    const s32 cycles_used = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(test.register_value, *reg);
    TEST_ASSERT_EQUAL_UINT8(test.expect_Z, cpu.Z);
    TEST_ASSERT_EQUAL_UINT8(test.expect_N, cpu.N);
    TEST_ASSERT_EQUAL_UINT8(test.expect_C, cpu.C);

    Check_Unaffected_Registers(cpu_before);
}

static void CMP_ZP_X(struct CMP_Test_Data test)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = !test.expect_C;
    cpu.Z               = !test.expect_Z;
    cpu.N               = !test.expect_N;
    cpu.accumulator     = test.register_value;
    cpu.index_reg_X     = 4;

    mem.data[0xFF00]       = INS_CMP_ZP_X;
    mem.data[0xFF01]       = 0x42;
    mem.data[0x0042 + 0x4] = test.operand;

    const s32 EXPECTED_CYCLES = 4;
    const CPU cpu_before      = cpu;

    // when:
    const s32 cycles_used = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(test.register_value, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(4, cpu.index_reg_X);

    TEST_ASSERT_EQUAL_UINT8(test.expect_Z, cpu.Z);
    TEST_ASSERT_EQUAL_UINT8(test.expect_N, cpu.N);
    TEST_ASSERT_EQUAL_UINT8(test.expect_C, cpu.C);

    Check_Unaffected_Registers(cpu_before);
}

static void CMP_ABS(struct CMP_Test_Data test, enum Register register_to_compare)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = !test.expect_C;
    cpu.Z               = !test.expect_Z;
    cpu.N               = !test.expect_N;

    u8 *reg    = &cpu.accumulator;
    u8  opcode = INS_CMP_ABS;
    switch (register_to_compare)
    {
    case REGISTER_X:
        reg    = &cpu.index_reg_X;
        opcode = INS_CPX_ABS;
        break;
    case REGISTER_Y:
        reg    = &cpu.index_reg_Y;
        opcode = INS_CPY_ABS;
        break;
    };
    *reg = test.register_value;

    mem.data[0xFF00] = opcode;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = test.operand;

    const s32 EXPECTED_CYCLES = 4;
    const CPU cpu_before      = cpu;

    // when:
    const s32 cycles_used = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(test.register_value, *reg);

    TEST_ASSERT_EQUAL_UINT8(test.expect_Z, cpu.Z);
    TEST_ASSERT_EQUAL_UINT8(test.expect_N, cpu.N);
    TEST_ASSERT_EQUAL_UINT8(test.expect_C, cpu.C);

    Check_Unaffected_Registers(cpu_before);
}

static void CMP_ABS_X(struct CMP_Test_Data test)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = !test.expect_C;
    cpu.Z               = !test.expect_Z;
    cpu.N               = !test.expect_N;
    cpu.accumulator     = test.register_value;
    cpu.index_reg_X     = 4;

    mem.data[0xFF00]     = INS_CMP_ABS_X;
    mem.data[0xFF01]     = 0x00;
    mem.data[0xFF02]     = 0x80;
    mem.data[0x8000 + 4] = test.operand;

    const s32 EXPECTED_CYCLES = 4;
    const CPU cpu_before      = cpu;

    // when:
    const s32 cycles_used = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(test.register_value, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(4, cpu.index_reg_X);

    TEST_ASSERT_EQUAL_UINT8(test.expect_Z, cpu.Z);
    TEST_ASSERT_EQUAL_UINT8(test.expect_N, cpu.N);
    TEST_ASSERT_EQUAL_UINT8(test.expect_C, cpu.C);

    Check_Unaffected_Registers(cpu_before);
}

static void CMP_ABS_Y(struct CMP_Test_Data test)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = !test.expect_C;
    cpu.Z               = !test.expect_Z;
    cpu.N               = !test.expect_N;
    cpu.accumulator     = test.register_value;
    cpu.index_reg_Y     = 4;

    mem.data[0xFF00]     = INS_CMP_ABS_Y;
    mem.data[0xFF01]     = 0x00;
    mem.data[0xFF02]     = 0x80;
    mem.data[0x8000 + 4] = test.operand;

    const s32 EXPECTED_CYCLES = 4;
    const CPU cpu_before      = cpu;

    // when:
    const s32 cycles_used = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(test.register_value, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(4, cpu.index_reg_Y);

    TEST_ASSERT_EQUAL_UINT8(test.expect_Z, cpu.Z);
    TEST_ASSERT_EQUAL_UINT8(test.expect_N, cpu.N);
    TEST_ASSERT_EQUAL_UINT8(test.expect_C, cpu.C);

    Check_Unaffected_Registers(cpu_before);
}

static void CMP_IND_X(struct CMP_Test_Data test)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C               = !test.expect_C;
    cpu.Z               = !test.expect_Z;
    cpu.N               = !test.expect_N;
    cpu.accumulator     = test.register_value;
    cpu.index_reg_X     = 4;

    mem.data[0xFF00]   = INS_CMP_IND_X;
    mem.data[0xFF01]   = 0x42;
    mem.data[0x42 + 4] = 0x00;
    mem.data[0x42 + 5] = 0x80;
    mem.data[0x8000]   = test.operand;

    const s32 EXPECTED_CYCLES = 6;
    const CPU cpu_before      = cpu;

    // when:
    const s32 cycles_used = Execute(EXPECTED_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(test.register_value, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(4, cpu.index_reg_X);

    TEST_ASSERT_EQUAL_UINT8(test.expect_Z, cpu.Z);
    TEST_ASSERT_EQUAL_UINT8(test.expect_N, cpu.N);
    TEST_ASSERT_EQUAL_UINT8(test.expect_C, cpu.C);

    Check_Unaffected_Registers(cpu_before);
}
// Immediate
void CMP_IM_Can_Compare_Two_Identical_Values(void)
{
    struct CMP_Test_Data test = Compare_Two_Identical_Values();
    CMP_IM(test, REGISTER_A);
}

void CMP_IM_Can_Compare_A_Large_Positive_To_A_Small_Positive(void)
{
    struct CMP_Test_Data test = Compare_A_Large_Positive_To_A_Small_Positive();
    CMP_IM(test, REGISTER_A);
}

void CMP_IM_Can_Compare_A_Negative_Number_To_A_Positive(void)
{
    struct CMP_Test_Data test = Compare_A_Negative_Number_To_A_Positive();
    CMP_IM(test, REGISTER_A);
}

void CMP_IM_Can_Compare_Two_Values_That_Result_In_A_Negative_Flag_Set(void)
{
    struct CMP_Test_Data test = Compare_Two_Values_That_Result_In_A_Negative_Flag_Set();
    CMP_IM(test, REGISTER_A);
}

// Zero Page
void CMP_ZP_Can_Compare_Two_Identical_Values(void)
{
    struct CMP_Test_Data test = Compare_Two_Identical_Values();
    CMP_ZP(test, REGISTER_A);
}

void CMP_ZP_Can_Compare_A_Large_Positive_To_A_Small_Positive(void)
{
    struct CMP_Test_Data test = Compare_A_Large_Positive_To_A_Small_Positive();
    CMP_ZP(test, REGISTER_A);
}

void CMP_ZP_Can_Compare_A_Negative_Number_To_A_Positive(void)
{
    struct CMP_Test_Data test = Compare_A_Negative_Number_To_A_Positive();
    CMP_ZP(test, REGISTER_A);
}

void CMP_ZP_Can_Compare_Two_Values_That_Result_In_A_Negative_Flag_Set(void)
{
    struct CMP_Test_Data test = Compare_Two_Values_That_Result_In_A_Negative_Flag_Set();
    CMP_ZP(test, REGISTER_A);
}

// Zero Page X
void CMP_ZP_X_Can_Compare_Two_Identical_Values(void)
{
    struct CMP_Test_Data test = Compare_Two_Identical_Values();
    CMP_ZP_X(test);
}

void CMP_ZP_X_Can_Compare_A_Large_Positive_To_A_Small_Positive(void)
{
    struct CMP_Test_Data test = Compare_A_Large_Positive_To_A_Small_Positive();
    CMP_ZP_X(test);
}

void CMP_ZP_X_Can_Compare_A_Negative_Number_To_A_Positive(void)
{
    struct CMP_Test_Data test = Compare_A_Negative_Number_To_A_Positive();
    CMP_ZP_X(test);
}

void CMP_ZP_X_Can_Compare_Two_Values_That_Result_In_A_Negative_Flag_Set(void)
{
    struct CMP_Test_Data test = Compare_Two_Values_That_Result_In_A_Negative_Flag_Set();
    CMP_ZP_X(test);
}

// Absolute
void CMP_ABS_Can_Compare_Two_Identical_Values(void)
{
    struct CMP_Test_Data test = Compare_Two_Identical_Values();
    CMP_ABS(test, REGISTER_A);
}

void CMP_ABS_Can_Compare_A_Large_Positive_To_A_Small_Positive(void)
{
    struct CMP_Test_Data test = Compare_A_Large_Positive_To_A_Small_Positive();
    CMP_ABS(test, REGISTER_A);
}

void CMP_ABS_Can_Compare_A_Negative_Number_To_A_Positive(void)
{
    struct CMP_Test_Data test = Compare_A_Negative_Number_To_A_Positive();
    CMP_ABS(test, REGISTER_A);
}

void CMP_ABS_Can_Compare_Two_Values_That_Result_In_A_Negative_Flag_Set(void)
{
    struct CMP_Test_Data test = Compare_Two_Values_That_Result_In_A_Negative_Flag_Set();
    CMP_ABS(test, REGISTER_A);
}

// Absolute X
void CMP_ABS_X_Can_Compare_Two_Identical_Values(void)
{
    struct CMP_Test_Data test = Compare_Two_Identical_Values();
    CMP_ABS_X(test);
}

void CMP_ABS_X_Can_Compare_A_Large_Positive_To_A_Small_Positive(void)
{
    struct CMP_Test_Data test = Compare_A_Large_Positive_To_A_Small_Positive();
    CMP_ABS_X(test);
}

void CMP_ABS_X_Can_Compare_A_Negative_Number_To_A_Positive(void)
{
    struct CMP_Test_Data test = Compare_A_Negative_Number_To_A_Positive();
    CMP_ABS_X(test);
}

void CMP_ABS_X_Can_Compare_Two_Values_That_Result_In_A_Negative_Flag_Set(void)
{
    struct CMP_Test_Data test = Compare_Two_Values_That_Result_In_A_Negative_Flag_Set();
    CMP_ABS_X(test);
}

// Absolute Y
void CMP_ABS_Y_Can_Compare_Two_Identical_Values(void)
{
    struct CMP_Test_Data test = Compare_Two_Identical_Values();
    CMP_ABS_Y(test);
}

void CMP_ABS_Y_Can_Compare_A_Large_Positive_To_A_Small_Positive(void)
{
    struct CMP_Test_Data test = Compare_A_Large_Positive_To_A_Small_Positive();
    CMP_ABS_Y(test);
}

void CMP_ABS_Y_Can_Compare_A_Negative_Number_To_A_Positive(void)
{
    struct CMP_Test_Data test = Compare_A_Negative_Number_To_A_Positive();
    CMP_ABS_Y(test);
}

void CMP_ABS_Y_Can_Compare_Two_Values_That_Result_In_A_Negative_Flag_Set(void)
{
    struct CMP_Test_Data test = Compare_Two_Values_That_Result_In_A_Negative_Flag_Set();
    CMP_ABS_Y(test);
}

// Indirect X
void CMP_IND_X_Can_Compare_Two_Identical_Values(void)
{
    struct CMP_Test_Data test = Compare_Two_Identical_Values();
    CMP_IND_X(test);
}

void CMP_IND_X_Can_Compare_A_Large_Positive_To_A_Small_Positive(void)
{
    struct CMP_Test_Data test = Compare_A_Large_Positive_To_A_Small_Positive();
    CMP_IND_X(test);
}

void CMP_IND_X_Can_Compare_A_Negative_Number_To_A_Positive(void)
{
    struct CMP_Test_Data test = Compare_A_Negative_Number_To_A_Positive();
    CMP_IND_X(test);
}

void CMP_IND_X_Can_Compare_Two_Values_That_Result_In_A_Negative_Flag_Set(void)
{
    struct CMP_Test_Data test = Compare_Two_Values_That_Result_In_A_Negative_Flag_Set();
    CMP_IND_X(test);
}
int main(void)
{
    UNITY_BEGIN();

    // Immediate
    RUN_TEST(CMP_IM_Can_Compare_Two_Identical_Values);
    RUN_TEST(CMP_IM_Can_Compare_A_Large_Positive_To_A_Small_Positive);
    RUN_TEST(CMP_IM_Can_Compare_A_Negative_Number_To_A_Positive);
    RUN_TEST(CMP_IM_Can_Compare_Two_Values_That_Result_In_A_Negative_Flag_Set);

    // Zero Page
    RUN_TEST(CMP_ZP_Can_Compare_Two_Identical_Values);
    RUN_TEST(CMP_ZP_Can_Compare_A_Large_Positive_To_A_Small_Positive);
    RUN_TEST(CMP_ZP_Can_Compare_A_Negative_Number_To_A_Positive);
    RUN_TEST(CMP_ZP_Can_Compare_Two_Values_That_Result_In_A_Negative_Flag_Set);

    // Zero Page X
    RUN_TEST(CMP_ZP_X_Can_Compare_Two_Identical_Values);
    RUN_TEST(CMP_ZP_X_Can_Compare_A_Large_Positive_To_A_Small_Positive);
    RUN_TEST(CMP_ZP_X_Can_Compare_A_Negative_Number_To_A_Positive);
    RUN_TEST(CMP_ZP_X_Can_Compare_Two_Values_That_Result_In_A_Negative_Flag_Set);

    // Absolute
    RUN_TEST(CMP_ABS_Can_Compare_Two_Identical_Values);
    RUN_TEST(CMP_ABS_Can_Compare_A_Large_Positive_To_A_Small_Positive);
    RUN_TEST(CMP_ABS_Can_Compare_A_Negative_Number_To_A_Positive);
    RUN_TEST(CMP_ABS_Can_Compare_Two_Values_That_Result_In_A_Negative_Flag_Set);

    // Absolute X
    RUN_TEST(CMP_ABS_X_Can_Compare_Two_Identical_Values);
    RUN_TEST(CMP_ABS_X_Can_Compare_A_Large_Positive_To_A_Small_Positive);
    RUN_TEST(CMP_ABS_X_Can_Compare_A_Negative_Number_To_A_Positive);
    RUN_TEST(CMP_ABS_X_Can_Compare_Two_Values_That_Result_In_A_Negative_Flag_Set);

    // Absolute Y
    RUN_TEST(CMP_ABS_Y_Can_Compare_Two_Identical_Values);
    RUN_TEST(CMP_ABS_Y_Can_Compare_A_Large_Positive_To_A_Small_Positive);
    RUN_TEST(CMP_ABS_Y_Can_Compare_A_Negative_Number_To_A_Positive);
    RUN_TEST(CMP_ABS_Y_Can_Compare_Two_Values_That_Result_In_A_Negative_Flag_Set);

    // Indirect X
    RUN_TEST(CMP_IND_X_Can_Compare_Two_Identical_Values);
    RUN_TEST(CMP_IND_X_Can_Compare_A_Large_Positive_To_A_Small_Positive);
    RUN_TEST(CMP_IND_X_Can_Compare_A_Negative_Number_To_A_Positive);
    RUN_TEST(CMP_IND_X_Can_Compare_Two_Values_That_Result_In_A_Negative_Flag_Set);
    return UNITY_END();
}