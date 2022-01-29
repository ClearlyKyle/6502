#include "Unity/unity.h"
#include "h6502.h"

#include <stdbool.h>

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU(&cpu, &mem);
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

static void Verify_Unmodified_Flags(const CPU before, const CPU after)
{
    TEST_ASSERT_EQUAL_UINT8(before.I, after.I);
    TEST_ASSERT_EQUAL_UINT8(before.D, after.D);
    TEST_ASSERT_EQUAL_UINT8(before.B, after.B);
}

void ADC_Can_Add_Zero_To_Zero_To_Get_Zero(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.accumulator = 0;
    cpu.C = 0;
    cpu.Z = 1;
    cpu.N = 1;

    mem.data[0xFF00] = INS_ADC_ABS;
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = 0x00;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x00, cpu.accumulator);
    TEST_ASSERT_FALSE(cpu.C);
    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
    TEST_ASSERT_FALSE(cpu.V);

    Verify_Unmodified_Flags(before, cpu);
}

struct ADC_Test_Data
{
    // before
    bool Carry;
    u8 Accumulator;
    u8 Operand;
    u8 Answer;

    // After
    bool ExpectC;
    bool ExpectZ;
    bool ExpectN; // Overflow
    bool ExpectV;
};

enum Operation
{
    Add,
    Subtract
};

static void Test_ADC_ABS(struct ADC_Test_Data test, enum Operation op)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.accumulator = test.Accumulator;
    cpu.C = test.Carry;
    cpu.Z = !test.ExpectZ;
    cpu.N = !test.ExpectN;
    cpu.V = !test.ExpectV;

    // mem.data[0xFF00] = (op == Add) ? INS_ADC_ABS : INS_SBC_ABS;
    if (op == Add)
    {
        mem.data[0xFF00] = INS_ADC_ABS;
    }
    mem.data[0xFF01] = 0x00;
    mem.data[0xFF02] = 0x80;
    mem.data[0x8000] = test.Operand;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(test.Answer, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(test.ExpectC, cpu.C);
    TEST_ASSERT_EQUAL_UINT8(test.ExpectZ, cpu.Z);
    TEST_ASSERT_EQUAL_UINT8(test.ExpectN, cpu.N);
    TEST_ASSERT_EQUAL_UINT8(test.ExpectV, cpu.V);

    Verify_Unmodified_Flags(before, cpu);
}

void ADC_ABS_Can_Add_Zero_To_Zero_And_Get_Zero(void)
{
    struct ADC_Test_Data Test;
    Test.Carry = false;
    Test.Accumulator = 0;
    Test.Operand = 0;
    Test.Answer = 0;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = true;
    Test_ADC_ABS(Test, Add);
}

void ADC_ABS_Can_Add_Carry_And_Zero_To_Zero_And_Get_One(void)
{
    struct ADC_Test_Data Test;
    Test.Carry = true;
    Test.Accumulator = 0;
    Test.Operand = 0;
    Test.Answer = 1;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    Test_ADC_ABS(Test, Add);
}

void ADC_ABS_Can_Add_Two_Unsigned_Numbers(void)
{
    struct ADC_Test_Data Test;
    Test.Carry = true;
    Test.Accumulator = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    Test_ADC_ABS(Test, Add);
}

void ADC_ABS_Can_Add_A_Positive_And_Negative_Number(void)
{
    // A: 00010100 20
    // O: 11101111 -17
    // =: 00000011
    // C:1 N:0 V:0 Z:0

    struct ADC_Test_Data Test;
    Test.Carry = true;
    Test.Accumulator = 20;
    Test.Operand = (u8)(-17);
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    Test_ADC_ABS(Test, Add);
}

void ADC_ABS_Can_Add_One_To_FF_And_It_Will_Cause_A_Carry(void)
{
    struct ADC_Test_Data Test;
    Test.Carry = false;
    Test.Accumulator = 0xFF;
    Test.Operand = 1;
    Test.Answer = 0;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = true;
    Test_ADC_ABS(Test, Add);
}

void ADC_ABS_Will_Set_Negative_Flag_When_Result_Is_Negative(void)
{
    struct ADC_Test_Data Test;
    Test.Carry = false;
    Test.Accumulator = 0;
    Test.Operand = (u8)(-1);
    Test.Answer = (u8)(-1);
    Test.ExpectC = false;
    Test.ExpectN = true;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    Test_ADC_ABS(Test, Add);
}

void ADC_ABS_Will_Set_Overflow_Flag_When_Signed_Negative_Addtion_Fails(void)
{
    // A: 10000000 -128
    // O: 11111111 -1
    // =: 01111111 127
    // C:1 N:0 V:1 Z:0
    struct ADC_Test_Data Test;
    Test.Carry = false;
    Test.Accumulator = (u8)(-128);
    Test.Operand = (u8)(-1);
    Test.Answer = 127;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = true;
    Test.ExpectZ = false;
    Test_ADC_ABS(Test, Add);
}

void ADC_ABS_Will_Set_Overflow_Flag_When_Signed_Negative_Addtion_Passed_Due_To_Inital_Carry_Flag(void)
{
    // C: 00000001
    // A: 10000000 -128
    // O: 11111111 -1
    // =: 10000000 -128
    // C:1 N:1 V:0 Z:0

    struct ADC_Test_Data Test;
    Test.Carry = true;
    Test.Accumulator = (u8)(-128);
    Test.Operand = (u8)(-1);
    Test.Answer = (u8)(-128);
    Test.ExpectC = true;
    Test.ExpectN = true;
    Test.ExpectV = false;
    Test.ExpectZ = false;
    Test_ADC_ABS(Test, Add);
}

void ADC_ABS_Will_Set_Overflow_Flag_When_Signed_Positive_Addtion_Fails(void)
{
    // A: 01111111 127
    // O: 00000001 1
    // =: 10000000 128
    // C:0 N:1 V:1 Z:0
    struct ADC_Test_Data Test;
    Test.Carry = false;
    Test.Accumulator = 127;
    Test.Operand = 1;
    Test.Answer = 128;
    Test.ExpectC = false;
    Test.ExpectN = true;
    Test.ExpectV = true;
    Test.ExpectZ = false;
    Test_ADC_ABS(Test, Add);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(ADC_ABS_Can_Add_Zero_To_Zero_And_Get_Zero);
    RUN_TEST(ADC_ABS_Can_Add_Carry_And_Zero_To_Zero_And_Get_One);
    RUN_TEST(ADC_ABS_Can_Add_Two_Unsigned_Numbers);
    RUN_TEST(ADC_ABS_Can_Add_A_Positive_And_Negative_Number);
    RUN_TEST(ADC_ABS_Can_Add_One_To_FF_And_It_Will_Cause_A_Carry);
    RUN_TEST(ADC_ABS_Will_Set_Negative_Flag_When_Result_Is_Negative);
    RUN_TEST(ADC_ABS_Will_Set_Overflow_Flag_When_Signed_Negative_Addtion_Fails);
    RUN_TEST(ADC_ABS_Will_Set_Overflow_Flag_When_Signed_Negative_Addtion_Passed_Due_To_Inital_Carry_Flag);
    RUN_TEST(ADC_ABS_Will_Set_Overflow_Flag_When_Signed_Positive_Addtion_Fails);

    return UNITY_END();
}