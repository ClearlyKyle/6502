#include "Unity/unity.h"
#include "h6502.h"

#include <stdbool.h>

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU(&cpu, &mem);
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

static void Verify_Unmodified_Flags_From_LDA(const CPU before, const CPU after)
{
    // get a copt of the cpu
    TEST_ASSERT_EQUAL_UINT8(before.C, after.C);
    TEST_ASSERT_EQUAL_UINT8(before.I, after.I);
    TEST_ASSERT_EQUAL_UINT8(before.D, after.D);
    TEST_ASSERT_EQUAL_UINT8(before.B, after.B);
    TEST_ASSERT_EQUAL_UINT8(before.V, after.V);
}

enum LogicOperator
{
    AND,
    EOR,
    OR
};

static u8 Do_Logical_Operation(u8 A, u8 B, enum LogicOperator LogicalOp)
{
    switch (LogicalOp)
    {
    case AND:
        return A & B;
        break;
    case EOR:
        return A ^ B;
        break;
    case OR:
        return A | B;
        break;
    default:
        fprintf(stderr, "Invalid Logical Operator!\n");
        return -1;
    }
}

static void Logical_Operator_Immediate(enum LogicOperator opp)
{
    // given:
    cpu.accumulator = 0xCC;
    switch (opp)
    {
    case AND:
        mem.data[0xFFFC] = INS_AND_IM;
        break;
    case EOR:
        mem.data[0xFFFC] = INS_EOR_IM;
        break;
    case OR:
        mem.data[0xFFFC] = INS_ORA_IM;
        break;
    }
    mem.data[0xFFFD] = 0x84;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    const u8 expected_result = Do_Logical_Operation(0xCC, 0x84, opp);
    const bool expected_negative = (expected_result & 0x80) > 0; // 0x80 = 0b10000000

    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(expected_result, cpu.accumulator);
    TEST_ASSERT_EQUAL(expected_negative, cpu.N);

    TEST_ASSERT_FALSE(cpu.Z);

    Verify_Unmodified_Flags_From_LDA(before, cpu);
}

static void Logical_Operator_Zero_Page(enum LogicOperator opp)
{
    // given:
    cpu.accumulator = 0xCC;
    switch (opp)
    {
    case AND:
        mem.data[0xFFFC] = INS_AND_ZP;
        break;
    case EOR:
        mem.data[0xFFFC] = INS_EOR_ZP;
        break;
    case OR:
        mem.data[0xFFFC] = INS_ORA_ZP;
        break;
    }
    mem.data[0xFFFD] = 0x42;
    mem.data[0x0042] = 0x37;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 3;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    const u8 expected_result = Do_Logical_Operation(0xCC, 0x37, opp);
    const bool expected_negative = (expected_result & 0x80) > 0; // 0x80 = 0b10000000

    TEST_ASSERT_EQUAL_UINT8(expected_result, cpu.accumulator);
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL(expected_negative, cpu.N);
    TEST_ASSERT_FALSE(cpu.Z);

    Verify_Unmodified_Flags_From_LDA(before, cpu);
}

static void Logical_Operator_Zero_Page_X(enum LogicOperator opp)
{
    // given:
    cpu.accumulator = 0xCC;
    cpu.index_reg_X = 5;
    switch (opp)
    {
    case AND:
        mem.data[0xFFFC] = INS_AND_ZP_X;
        break;
    case EOR:
        mem.data[0xFFFC] = INS_EOR_ZP_X;
        break;
    case OR:
        mem.data[0xFFFC] = INS_ORA_ZP_X;
        break;
    }
    mem.data[0xFFFD] = 0x42;
    mem.data[0x0047] = 0x37;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    const u8 expected_result = Do_Logical_Operation(0xCC, 0x37, opp);
    const bool expected_negative = (expected_result & 0x80) > 0; // 0x80 = 0b10000000

    TEST_ASSERT_EQUAL_UINT8(expected_result, cpu.accumulator);
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL(expected_negative, cpu.N);
    TEST_ASSERT_FALSE(cpu.Z);

    Verify_Unmodified_Flags_From_LDA(before, cpu);
}

static void Logical_Operator_ABS(enum LogicOperator opp)
{
    // given:
    cpu.accumulator = 0xCC;
    cpu.Z = 1;
    cpu.N = 1;

    switch (opp)
    {
    case AND:
        mem.data[0xFFFC] = INS_AND_ABS;
        break;
    case EOR:
        mem.data[0xFFFC] = INS_EOR_ABS;
        break;
    case OR:
        mem.data[0xFFFC] = INS_ORA_ABS;
        break;
    }

    mem.data[0xFFFD] = 0x80;
    mem.data[0xFFFE] = 0x44; // 0x4480
    mem.data[0x4480] = 0x37;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    const u8 expected_result = Do_Logical_Operation(0xCC, 0x37, opp);
    const bool expected_negative = (expected_result & 0x80) > 0; // 0x80 = 0b10000000

    TEST_ASSERT_EQUAL_UINT8(expected_result, cpu.accumulator);
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL(expected_negative, cpu.N);
    TEST_ASSERT_FALSE(cpu.Z);

    Verify_Unmodified_Flags_From_LDA(before, cpu);
}

static void Logical_Operator_ABS_X(enum LogicOperator opp)
{
    // given:
    cpu.accumulator = 0xCC;
    cpu.Z = 1;
    cpu.N = 1;
    cpu.index_reg_X = 1;

    switch (opp)
    {
    case AND:
        mem.data[0xFFFC] = INS_AND_ABS_X;
        break;
    case EOR:
        mem.data[0xFFFC] = INS_EOR_ABS_X;
        break;
    case OR:
        mem.data[0xFFFC] = INS_ORA_ABS_X;
        break;
    }

    mem.data[0xFFFD] = 0x80;
    mem.data[0xFFFE] = 0x44; // 0x4480
    mem.data[0x4481] = 0x37;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    const u8 expected_result = Do_Logical_Operation(0xCC, 0x37, opp);
    const bool expected_negative = (expected_result & 0x80) > 0; // 0x80 = 0b10000000

    TEST_ASSERT_EQUAL_UINT8(expected_result, cpu.accumulator);
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL(expected_negative, cpu.N);
    TEST_ASSERT_FALSE(cpu.Z);

    Verify_Unmodified_Flags_From_LDA(before, cpu);
}

static void Logical_Operator_ABS_Y(enum LogicOperator opp)
{
    // given:
    cpu.accumulator = 0xCC;
    cpu.Z = 1;
    cpu.N = 1;
    cpu.index_reg_Y = 1;

    switch (opp)
    {
    case AND:
        mem.data[0xFFFC] = INS_AND_ABS_Y;
        break;
    case EOR:
        mem.data[0xFFFC] = INS_EOR_ABS_Y;
        break;
    case OR:
        mem.data[0xFFFC] = INS_ORA_ABS_Y;
        break;
    }

    mem.data[0xFFFD] = 0x80;
    mem.data[0xFFFE] = 0x44; // 0x4480
    mem.data[0x4481] = 0x37;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    const u8 expected_result = Do_Logical_Operation(0xCC, 0x37, opp);
    const bool expected_negative = (expected_result & 0x80) > 0; // 0x80 = 0b10000000

    TEST_ASSERT_EQUAL_UINT8(expected_result, cpu.accumulator);
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL(expected_negative, cpu.N);
    TEST_ASSERT_FALSE(cpu.Z);

    Verify_Unmodified_Flags_From_LDA(before, cpu);
}

static void Load_Register_ABS_Y_When_Crossing_Page_Boundary(enum LogicOperator opp)
{
    // given:
    cpu.accumulator = 0xCC;
    cpu.index_reg_Y = 0xFF;

    switch (opp)
    {
    case AND:
        mem.data[0xFFFC] = INS_AND_ABS_Y;
        break;
    case EOR:
        mem.data[0xFFFC] = INS_EOR_ABS_Y;
        break;
    case OR:
        mem.data[0xFFFC] = INS_ORA_ABS_Y;
        break;
    }

    mem.data[0xFFFD] = 0x02;
    mem.data[0xFFFE] = 0x44; // 0x4402
    mem.data[0x4501] = 0x37; // 0x4402+0xFF crosses page boundary!

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 5;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    const u8 expected_result = Do_Logical_Operation(0xCC, 0x37, opp);
    const bool expected_negative = (expected_result & 0x80) > 0; // 0x80 = 0b10000000

    TEST_ASSERT_EQUAL_UINT8(expected_result, cpu.accumulator);
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL(expected_negative, cpu.N);
    TEST_ASSERT_FALSE(cpu.Z);

    Verify_Unmodified_Flags_From_LDA(before, cpu);
}

static void Load_Register_ABS_X_When_Crossing_Page_Boundary(enum LogicOperator opp)
{
    // given:
    cpu.accumulator = 0xCC;
    cpu.index_reg_X = 0xFF;

    switch (opp)
    {
    case AND:
        mem.data[0xFFFC] = INS_AND_ABS_X;
        break;
    case EOR:
        mem.data[0xFFFC] = INS_EOR_ABS_X;
        break;
    case OR:
        mem.data[0xFFFC] = INS_ORA_ABS_X;
        break;
    }

    mem.data[0xFFFD] = 0x02;
    mem.data[0xFFFE] = 0x44; // 0x4402
    mem.data[0x4501] = 0x37; // 0x4402+0xFF crosses page boundary!

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 5;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    const u8 expected_result = Do_Logical_Operation(0xCC, 0x37, opp);
    const bool expected_negative = (expected_result & 0x80) > 0; // 0x80 = 0b10000000

    TEST_ASSERT_EQUAL_UINT8(expected_result, cpu.accumulator);
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL(expected_negative, cpu.N);
    TEST_ASSERT_FALSE(cpu.Z);

    Verify_Unmodified_Flags_From_LDA(before, cpu);
}

static void Logical_Operator_Indirect_X(enum LogicOperator opp)
{
    // given:
    cpu.accumulator = 0xCC;
    cpu.index_reg_X = 0x04;
    cpu.Z = 1;
    cpu.N = 1;

    switch (opp)
    {
    case AND:
        mem.data[0xFFFC] = INS_AND_IND_X;
        break;
    case EOR:
        mem.data[0xFFFC] = INS_EOR_IND_X;
        break;
    case OR:
        mem.data[0xFFFC] = INS_ORA_IND_X;
        break;
    }

    mem.data[0xFFFD] = 0x02;
    mem.data[0x0006] = 0x00; // 0x2 + 0x4
    mem.data[0x0007] = 0x80;
    mem.data[0x8000] = 0x37;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 6;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    const u8 expected_result = Do_Logical_Operation(0xCC, 0x37, opp);
    const bool expected_negative = (expected_result & 0x80) > 0; // 0x80 = 0b10000000

    TEST_ASSERT_EQUAL_UINT8(expected_result, cpu.accumulator);
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL(expected_negative, cpu.N);
    TEST_ASSERT_FALSE(cpu.Z);

    Verify_Unmodified_Flags_From_LDA(before, cpu);
}

static void Logical_Operator_Indirect_Y(enum LogicOperator opp)
{
    // given:
    cpu.accumulator = 0xCC;
    cpu.index_reg_Y = 0x04;
    cpu.Z = 1;
    cpu.N = 1;

    switch (opp)
    {
    case AND:
        mem.data[0xFFFC] = INS_AND_IND_Y;
        break;
    case EOR:
        mem.data[0xFFFC] = INS_EOR_IND_Y;
        break;
    case OR:
        mem.data[0xFFFC] = INS_ORA_IND_Y;
        break;
    }

    mem.data[0xFFFD] = 0x02;
    mem.data[0x0002] = 0x00;
    mem.data[0x0003] = 0x80;
    mem.data[0x8004] = 0x37; // 0x8000 + 0x4

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 5;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    const u8 expected_result = Do_Logical_Operation(0xCC, 0x37, opp);
    const bool expected_negative = (expected_result & 0x80) > 0; // 0x80 = 0b10000000

    TEST_ASSERT_EQUAL_UINT8(expected_result, cpu.accumulator);
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL(expected_negative, cpu.N);
    TEST_ASSERT_FALSE(cpu.Z);

    Verify_Unmodified_Flags_From_LDA(before, cpu);
}

static void Logical_Operator_Indirect_Y_When_Crossing_Page_Boundary(enum LogicOperator opp)
{
    // given:
    cpu.accumulator = 0xCC;
    cpu.index_reg_Y = 0xFF;

    switch (opp)
    {
    case AND:
        mem.data[0xFFFC] = INS_AND_IND_Y;
        break;
    case EOR:
        mem.data[0xFFFC] = INS_EOR_IND_Y;
        break;
    case OR:
        mem.data[0xFFFC] = INS_ORA_IND_Y;
        break;
    }

    mem.data[0xFFFD] = 0x02;
    mem.data[0x0002] = 0x02;
    mem.data[0x0003] = 0x80;
    mem.data[0x8101] = 0x37; // 0x8002 + 0xFF

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 6;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    const u8 expected_result = Do_Logical_Operation(0xCC, 0x37, opp);
    const bool expected_negative = (expected_result & 0x80) > 0; // 0x80 = 0b10000000

    TEST_ASSERT_EQUAL_UINT8(expected_result, cpu.accumulator);
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL(expected_negative, cpu.N);
    TEST_ASSERT_FALSE(cpu.Z);

    Verify_Unmodified_Flags_From_LDA(before, cpu);
}

static void Logical_Operator_Zero_Page_X_When_It_Wraps(enum LogicOperator opp)
{
    // given:
    cpu.accumulator = 0xCC;
    cpu.index_reg_X = 0xFF;

    switch (opp)
    {
    case AND:
        mem.data[0xFFFC] = INS_AND_ZP_X;
        break;
    case EOR:
        mem.data[0xFFFC] = INS_EOR_ZP_X;
        break;
    case OR:
        mem.data[0xFFFC] = INS_ORA_ZP_X;
        break;
    }

    mem.data[0xFFFD] = 0x80;
    mem.data[0x007F] = 0x37;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    const u8 expected_result = Do_Logical_Operation(0xCC, 0x37, opp);
    const bool expected_negative = (expected_result & 0x80) > 0; // 0x80 = 0b10000000

    TEST_ASSERT_EQUAL_UINT8(expected_result, cpu.accumulator);
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL(expected_negative, cpu.N);
    TEST_ASSERT_FALSE(cpu.Z);

    Verify_Unmodified_Flags_From_LDA(before, cpu);
}

void Test_Logical_Operator_EOR_Immediate_Can_Affect_Zero_Flag(void)
{
    // given:
    cpu.accumulator = 0xCC;
    mem.data[0xFFFC] = INS_EOR_IM;
    mem.data[0xFFFD] = cpu.accumulator;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags_From_LDA(before, cpu);
}

// Immediate
void Test_Logical_Operator_AND_On_A_Register_Immediate(void)
{
    Logical_Operator_Immediate(AND);
}
void Test_Logical_Operator_ORA_On_A_Register_Immediate(void)
{
    Logical_Operator_Immediate(OR);
}
void Test_Logical_Operator_EOR_On_A_Register_Immediate(void)
{
    Logical_Operator_Immediate(EOR);
}

// Zero Page
void Test_Logical_Operator_AND_On_A_Register_ZP(void)
{
    Logical_Operator_Zero_Page(AND);
}
void Test_Logical_Operator_ORA_On_A_Register_ZP(void)
{
    Logical_Operator_Zero_Page(OR);
}
void Test_Logical_Operator_EOR_On_A_Register_ZP(void)
{
    Logical_Operator_Zero_Page(EOR);
}

// Zero Page X
void Test_Logical_Operator_AND_On_A_Register_ZP_X(void)
{
    Logical_Operator_Zero_Page_X(AND);
}
void Test_Logical_Operator_ORA_On_A_Register_ZP_X(void)
{
    Logical_Operator_Zero_Page_X(OR);
}
void Test_Logical_Operator_EOR_On_A_Register_ZP_X(void)
{
    Logical_Operator_Zero_Page_X(EOR);
}

// Zero Page X when it wraps
void Test_Logical_Operator_EOR_Can_Load_A_Register_When_Wraps_ZP_X(void)
{
    Logical_Operator_Zero_Page_X_When_It_Wraps(EOR);
}
void Test_Logical_Operator_ORA_Can_Load_A_Register_When_Wraps_ZP_X(void)
{
    Logical_Operator_Zero_Page_X_When_It_Wraps(OR);
}
void Test_Logical_Operator_AND_Can_Load_A_Register_When_Wraps_ZP_X(void)
{
    Logical_Operator_Zero_Page_X_When_It_Wraps(AND);
}

// Absolute
void Test_Logical_Operator_EOR_On_A_Register_ABS(void)
{
    Logical_Operator_ABS(EOR);
}
void Test_Logical_Operator_ORA_On_A_Register_ABS(void)
{
    Logical_Operator_ABS(OR);
}
void Test_Logical_Operator_AND_On_A_Register_ABS(void)
{
    Logical_Operator_ABS(AND);
}

// Absolute X
void Test_Logical_Operator_EOR_On_A_Register_ABS_X(void)
{
    Logical_Operator_ABS_X(EOR);
}
void Test_Logical_Operator_ORA_On_A_Register_ABS_X(void)
{
    Logical_Operator_ABS_X(OR);
}
void Test_Logical_Operator_AND_On_A_Register_ABS_X(void)
{
    Logical_Operator_ABS_X(AND);
}

// Absolute X when crossing page boundary
void Test_Logical_Operator_EOR_On_A_Register_ABS_X_When_Crossing_Page_Boundary(void)
{
    Load_Register_ABS_X_When_Crossing_Page_Boundary(EOR);
}
void Test_Logical_Operator_ORA_On_A_Register_ABS_X_When_Crossing_Page_Boundary(void)
{
    Load_Register_ABS_X_When_Crossing_Page_Boundary(OR);
}
void Test_Logical_Operator_AND_On_A_Register_ABS_X_When_Crossing_Page_Boundary(void)
{
    Load_Register_ABS_X_When_Crossing_Page_Boundary(AND);
}

// Absolute Y
void Test_Logical_Operator_EOR_On_A_Register_ABS_Y(void)
{
    Logical_Operator_ABS_Y(EOR);
}
void Test_Logical_Operator_ORA_On_A_Register_ABS_Y(void)
{
    Logical_Operator_ABS_Y(OR);
}
void Test_Logical_Operator_AND_On_A_Register_ABS_Y(void)
{
    Logical_Operator_ABS_Y(AND);
}

// Absolute Y when crossing page boundary
void Test_Logical_Operator_EOR_On_A_Register_ABS_Y_When_Crossing_Page_Boundary(void)
{
    Load_Register_ABS_Y_When_Crossing_Page_Boundary(EOR);
}
void Test_Logical_Operator_ORA_On_A_Register_ABS_Y_When_Crossing_Page_Boundary(void)
{
    Load_Register_ABS_Y_When_Crossing_Page_Boundary(OR);
}
void Test_Logical_Operator_AND_On_A_Register_ABS_Y_When_Crossing_Page_Boundary(void)
{
    Load_Register_ABS_Y_When_Crossing_Page_Boundary(AND);
}

// Indirect X
void Test_Logical_Operator_EOR_On_A_Register_IND_X(void)
{
    Logical_Operator_Indirect_X(EOR);
}
void Test_Logical_Operator_ORA_On_A_Register_IND_X(void)
{
    Logical_Operator_Indirect_X(OR);
}
void Test_Logical_Operator_AND_On_A_Register_IND_X(void)
{
    Logical_Operator_Indirect_X(AND);
}

// Indirect Y
void Test_Logical_Operator_EOR_On_A_Register_IND_Y(void)
{
    Logical_Operator_Indirect_Y(EOR);
}
void Test_Logical_Operator_ORA_On_A_Register_IND_Y(void)
{
    Logical_Operator_Indirect_Y(OR);
}
void Test_Logical_Operator_AND_On_A_Register_IND_Y(void)
{
    Logical_Operator_Indirect_Y(AND);
}

// Indirect Y when crossing page boundary
void Test_Logical_Operator_EOR_On_A_Register_IND_Y_When_Crossing_Page_Boundary(void)
{
    Logical_Operator_Indirect_Y_When_Crossing_Page_Boundary(EOR);
}
void Test_Logical_Operator_ORA_On_A_Register_IND_Y_When_Crossing_Page_Boundary(void)
{
    Logical_Operator_Indirect_Y_When_Crossing_Page_Boundary(OR);
}
void Test_Logical_Operator_AND_On_A_Register_IND_Y_When_Crossing_Page_Boundary(void)
{
    Logical_Operator_Indirect_Y_When_Crossing_Page_Boundary(AND);
}

// Bit tests
void Test_BIT_ZP(void)
{
    // given:
    cpu.V = 0;
    cpu.N = 0;
    cpu.accumulator = 0xCC;

    mem.data[0xFFFC] = INS_BIT_ZP;
    mem.data[0xFFFD] = 0x42;
    mem.data[0x0042] = 0xCC;

    // when:
    const s32 NUM_OF_CYCLES = 3;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX8(0xCC, cpu.accumulator);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.V);
    TEST_ASSERT_TRUE(cpu.N);
}

void Test_BIT_ZP_Result_Zero(void)

{
    // given:
    cpu.V = 1;
    cpu.N = 1;
    cpu.accumulator = 0xCC;

    mem.data[0xFFFC] = INS_BIT_ZP;
    mem.data[0xFFFD] = 0x42;
    mem.data[0x0042] = 0x33;

    // when
    const s32 NUM_OF_CYCLES = 3;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0xCC, cpu.accumulator);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.V);
    TEST_ASSERT_FALSE(cpu.N);
}

void Test_BIT_ZP_Result_Zero_Bits_6_and_7_Zero(void)
{
    // given:
    cpu.V = 0;
    cpu.N = 0;
    cpu.accumulator = 0x33;

    mem.data[0xFFFC] = INS_BIT_ZP;
    mem.data[0xFFFD] = 0x42;
    mem.data[0x0042] = 0xCC;

    // when:
    const s32 NUM_OF_CYCLES = 3;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x33, cpu.accumulator);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.V);
    TEST_ASSERT_TRUE(cpu.N);
}

void Test_BIT_ZP_Result_Zero_Bits_6_and_7_Mixed(void)
{
    // given:
    cpu.V = 0;
    cpu.N = 1;

    mem.data[0xFFFC] = INS_BIT_ZP;
    mem.data[0xFFFD] = 0x42;
    mem.data[0x0042] = 0x40; // 0x40 = 0b01000000

    // when:
    const s32 NUM_OF_CYCLES = 3;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_TRUE(cpu.V);
    TEST_ASSERT_FALSE(cpu.N);
}

void Test_Bit_ABS(void)
{
    // given:
    cpu.V = 0;
    cpu.N = 0;
    cpu.accumulator = 0xCC;

    mem.data[0xFFFC] = INS_BIT_ABS;
    mem.data[0xFFFD] = 0x00;
    mem.data[0xFFFE] = 0x80;
    mem.data[0x8000] = 0xCC;

    // when:
    const s32 NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0xCC, cpu.accumulator);

    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.V);
    TEST_ASSERT_TRUE(cpu.N);
}

void Test_Bit_ABS_Result_Zero(void)
{
    // given:
    cpu.V = 1;
    cpu.N = 1;
    cpu.accumulator = 0xCC;

    mem.data[0xFFFC] = INS_BIT_ABS;
    mem.data[0xFFFD] = 0x00;
    mem.data[0xFFFE] = 0x80;
    mem.data[0x8000] = 0x33;

    // when:
    const s32 NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0xCC, cpu.accumulator);

    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.V);
    TEST_ASSERT_FALSE(cpu.N);
}

void Test_BIT_ABS_Result_Zero_Bit_6_And_7_Zero(void)
{
    // given:
    cpu.V = 0;
    cpu.N = 0;
    cpu.accumulator = 0x33;

    mem.data[0xFFFC] = INS_BIT_ABS;
    mem.data[0xFFFD] = 0x00;
    mem.data[0xFFFE] = 0x80;
    mem.data[0x8000] = 0xCC;

    // when:
    const s32 NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x33, cpu.accumulator);

    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.V);
    TEST_ASSERT_TRUE(cpu.N);
}

void Test_BIT_ABS_Result_Zero_Bit_6_And_7_Mixed(void)
{
    // given:
    cpu.V = 1;
    cpu.N = 0;

    mem.data[0xFFFC] = INS_BIT_ABS;
    mem.data[0xFFFD] = 0x00;
    mem.data[0xFFFE] = 0x80;
    mem.data[0x8000] = 0x80; // 0x80 = 0b10000000

    // when:
    const s32 NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);

    TEST_ASSERT_FALSE(cpu.V);
    TEST_ASSERT_TRUE(cpu.N);
}

void Test_Load_Program_Into_Correct_Area_Of_Memory(void)
{
    const u8 program[] = {0x00, 0x10, 0xA9, 0xFF, 0x85, 0x90, 0x8D, 0x00, 0x80, 0x49, 0xCC, 0x4C, 0x02, 0x10};
    const int number_of_bytes = 14;

    Load_Program(program, &mem, number_of_bytes);

    Memory mem_expected;
    Initialise_Memory(&mem_expected); // set all to 0x00
    mem_expected.data[0x0FFF] = 0x00;
    mem_expected.data[0x1000] = 0xA9;
    mem_expected.data[0x1001] = 0xFF;
    mem_expected.data[0x1002] = 0x85;
    mem_expected.data[0x1003] = 0x90;
    mem_expected.data[0x1004] = 0x8D;
    mem_expected.data[0x1005] = 0x00;
    mem_expected.data[0x1006] = 0x80;
    mem_expected.data[0x1007] = 0x49;
    mem_expected.data[0x1008] = 0xCC;
    mem_expected.data[0x1009] = 0x4C;
    mem_expected.data[0x100A] = 0x02;
    mem_expected.data[0x100B] = 0x10;
    mem_expected.data[0x100C] = 0x00; // end

    TEST_ASSERT_EQUAL_UINT8_ARRAY(mem_expected.data, mem.data, MAX_MEM);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(Test_Logical_Operator_AND_On_A_Register_Immediate);
    RUN_TEST(Test_Logical_Operator_ORA_On_A_Register_Immediate);
    RUN_TEST(Test_Logical_Operator_EOR_On_A_Register_Immediate);
    RUN_TEST(Test_Logical_Operator_EOR_Immediate_Can_Affect_Zero_Flag);

    RUN_TEST(Test_Logical_Operator_AND_On_A_Register_ZP);
    RUN_TEST(Test_Logical_Operator_ORA_On_A_Register_ZP);
    RUN_TEST(Test_Logical_Operator_EOR_On_A_Register_ZP);

    RUN_TEST(Test_Logical_Operator_AND_On_A_Register_ZP_X);
    RUN_TEST(Test_Logical_Operator_ORA_On_A_Register_ZP_X);
    RUN_TEST(Test_Logical_Operator_EOR_On_A_Register_ZP_X);

    RUN_TEST(Test_Logical_Operator_EOR_Can_Load_A_Register_When_Wraps_ZP_X);
    RUN_TEST(Test_Logical_Operator_ORA_Can_Load_A_Register_When_Wraps_ZP_X);
    RUN_TEST(Test_Logical_Operator_AND_Can_Load_A_Register_When_Wraps_ZP_X);

    RUN_TEST(Test_Logical_Operator_EOR_On_A_Register_ABS);
    RUN_TEST(Test_Logical_Operator_ORA_On_A_Register_ABS);
    RUN_TEST(Test_Logical_Operator_AND_On_A_Register_ABS);

    RUN_TEST(Test_Logical_Operator_EOR_On_A_Register_ABS_X);
    RUN_TEST(Test_Logical_Operator_ORA_On_A_Register_ABS_X);
    RUN_TEST(Test_Logical_Operator_AND_On_A_Register_ABS_X);

    RUN_TEST(Test_Logical_Operator_EOR_On_A_Register_ABS_X_When_Crossing_Page_Boundary);
    RUN_TEST(Test_Logical_Operator_ORA_On_A_Register_ABS_X_When_Crossing_Page_Boundary);
    RUN_TEST(Test_Logical_Operator_AND_On_A_Register_ABS_X_When_Crossing_Page_Boundary);

    RUN_TEST(Test_Logical_Operator_EOR_On_A_Register_ABS_Y);
    RUN_TEST(Test_Logical_Operator_ORA_On_A_Register_ABS_Y);
    RUN_TEST(Test_Logical_Operator_AND_On_A_Register_ABS_Y);

    RUN_TEST(Test_Logical_Operator_EOR_On_A_Register_ABS_Y_When_Crossing_Page_Boundary);
    RUN_TEST(Test_Logical_Operator_ORA_On_A_Register_ABS_Y_When_Crossing_Page_Boundary);
    RUN_TEST(Test_Logical_Operator_AND_On_A_Register_ABS_Y_When_Crossing_Page_Boundary);

    RUN_TEST(Test_Logical_Operator_EOR_On_A_Register_IND_X);
    RUN_TEST(Test_Logical_Operator_ORA_On_A_Register_IND_X);
    RUN_TEST(Test_Logical_Operator_AND_On_A_Register_IND_X);

    RUN_TEST(Test_Logical_Operator_EOR_On_A_Register_IND_Y);
    RUN_TEST(Test_Logical_Operator_ORA_On_A_Register_IND_Y);
    RUN_TEST(Test_Logical_Operator_AND_On_A_Register_IND_Y);

    RUN_TEST(Test_Logical_Operator_EOR_On_A_Register_IND_Y_When_Crossing_Page_Boundary);
    RUN_TEST(Test_Logical_Operator_ORA_On_A_Register_IND_Y_When_Crossing_Page_Boundary);
    RUN_TEST(Test_Logical_Operator_AND_On_A_Register_IND_Y_When_Crossing_Page_Boundary);

    // Bit Tests
    RUN_TEST(Test_BIT_ZP);
    RUN_TEST(Test_BIT_ZP_Result_Zero);
    RUN_TEST(Test_BIT_ZP_Result_Zero_Bits_6_and_7_Zero);
    RUN_TEST(Test_BIT_ZP_Result_Zero_Bits_6_and_7_Mixed);

    RUN_TEST(Test_Bit_ABS);
    RUN_TEST(Test_Bit_ABS_Result_Zero);
    RUN_TEST(Test_BIT_ABS_Result_Zero_Bit_6_And_7_Zero);
    RUN_TEST(Test_BIT_ABS_Result_Zero_Bit_6_And_7_Mixed);

    // Program Loading testing
    RUN_TEST(Test_Load_Program_Into_Correct_Area_Of_Memory);

    return UNITY_END();
}