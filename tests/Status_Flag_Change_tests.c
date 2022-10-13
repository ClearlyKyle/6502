#include "Unity/unity.h"
#include "h6502.h"

#include <stdbool.h>

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU(&cpu, &mem);
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

void CLC_Will_Clear_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C = true;

    mem.data[0xFF00] = INS_CLC;

    // when:
    const CPU before = cpu;
    const s32 NUM_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_CYCLES, cycles_used);
    TEST_ASSERT_FALSE(cpu.C);

    // TEST_ASSERT_EQUAL_UINT8(before.C, cpu.C);
    TEST_ASSERT_EQUAL_UINT8(before.Z, cpu.Z);
    TEST_ASSERT_EQUAL_UINT8(before.I, cpu.I);
    TEST_ASSERT_EQUAL_UINT8(before.D, cpu.D);
    TEST_ASSERT_EQUAL_UINT8(before.B, cpu.B);
    TEST_ASSERT_EQUAL_UINT8(before.V, cpu.V);
    TEST_ASSERT_EQUAL_UINT8(before.N, cpu.N);
}

void SEC_Will_Set_Carry_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.C = false;

    mem.data[0xFF00] = INS_SEC;

    // when:
    const CPU before = cpu;
    const s32 NUM_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_CYCLES, cycles_used);
    TEST_ASSERT_TRUE(cpu.C);

    // TEST_ASSERT_EQUAL_UINT8(before.C, cpu.C);
    TEST_ASSERT_EQUAL_UINT8(before.Z, cpu.Z);
    TEST_ASSERT_EQUAL_UINT8(before.I, cpu.I);
    TEST_ASSERT_EQUAL_UINT8(before.D, cpu.D);
    TEST_ASSERT_EQUAL_UINT8(before.B, cpu.B);
    TEST_ASSERT_EQUAL_UINT8(before.V, cpu.V);
    TEST_ASSERT_EQUAL_UINT8(before.N, cpu.N);
}

void CLD_Will_Clear_Decimal_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.D = true;

    mem.data[0xFF00] = INS_CLD;

    // when:
    const CPU before = cpu;
    const s32 NUM_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_CYCLES, cycles_used);
    TEST_ASSERT_FALSE(cpu.D);

    TEST_ASSERT_EQUAL_UINT8(before.C, cpu.C);
    TEST_ASSERT_EQUAL_UINT8(before.Z, cpu.Z);
    TEST_ASSERT_EQUAL_UINT8(before.I, cpu.I);
    // TEST_ASSERT_EQUAL_UINT8(before.D, cpu.D);
    TEST_ASSERT_EQUAL_UINT8(before.B, cpu.B);
    TEST_ASSERT_EQUAL_UINT8(before.V, cpu.V);
    TEST_ASSERT_EQUAL_UINT8(before.N, cpu.N);
}

void SED_Will_Set_Decimal_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.D = false;

    mem.data[0xFF00] = INS_SED;

    // when:
    const CPU before = cpu;
    const s32 NUM_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_CYCLES, cycles_used);
    TEST_ASSERT_TRUE(cpu.D);

    TEST_ASSERT_EQUAL_UINT8(before.C, cpu.C);
    TEST_ASSERT_EQUAL_UINT8(before.Z, cpu.Z);
    TEST_ASSERT_EQUAL_UINT8(before.I, cpu.I);
    // TEST_ASSERT_EQUAL_UINT8(before.D, cpu.D);
    TEST_ASSERT_EQUAL_UINT8(before.B, cpu.B);
    TEST_ASSERT_EQUAL_UINT8(before.V, cpu.V);
    TEST_ASSERT_EQUAL_UINT8(before.N, cpu.N);
}

void CLI_Will_Clear_Interrupt_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.I = true;

    mem.data[0xFF00] = INS_CLI;

    // when:
    const CPU before = cpu;
    const s32 NUM_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_CYCLES, cycles_used);
    TEST_ASSERT_FALSE(cpu.I);

    TEST_ASSERT_EQUAL_UINT8(before.C, cpu.C);
    TEST_ASSERT_EQUAL_UINT8(before.Z, cpu.Z);
    // TEST_ASSERT_EQUAL_UINT8(before.I, cpu.I);
    TEST_ASSERT_EQUAL_UINT8(before.D, cpu.D);
    TEST_ASSERT_EQUAL_UINT8(before.B, cpu.B);
    TEST_ASSERT_EQUAL_UINT8(before.V, cpu.V);
    TEST_ASSERT_EQUAL_UINT8(before.N, cpu.N);
}

void SEI_Will_Set_Interrupt_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.I = false;

    mem.data[0xFF00] = INS_SEI;

    // when:
    const CPU before = cpu;
    const s32 NUM_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_CYCLES, cycles_used);
    TEST_ASSERT_TRUE(cpu.I);

    TEST_ASSERT_EQUAL_UINT8(before.C, cpu.C);
    TEST_ASSERT_EQUAL_UINT8(before.Z, cpu.Z);
    // TEST_ASSERT_EQUAL_UINT8(before.I, cpu.I);
    TEST_ASSERT_EQUAL_UINT8(before.D, cpu.D);
    TEST_ASSERT_EQUAL_UINT8(before.B, cpu.B);
    TEST_ASSERT_EQUAL_UINT8(before.V, cpu.V);
    TEST_ASSERT_EQUAL_UINT8(before.N, cpu.N);
}

void CLV_Will_Clear_Overflow_Flag(void)
{
    // given:
    cpu.program_counter = 0xFF00;
    cpu.V = true;

    mem.data[0xFF00] = INS_CLV;

    // when:
    const CPU before = cpu;
    const s32 NUM_CYCLES = 2;
    
    const s32 cycles_used = Execute(NUM_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_CYCLES, cycles_used);
    TEST_ASSERT_FALSE(cpu.V);

    TEST_ASSERT_EQUAL_UINT8(before.C, cpu.C);
    TEST_ASSERT_EQUAL_UINT8(before.Z, cpu.Z);
    TEST_ASSERT_EQUAL_UINT8(before.I, cpu.I);
    TEST_ASSERT_EQUAL_UINT8(before.D, cpu.D);
    TEST_ASSERT_EQUAL_UINT8(before.B, cpu.B);
    // TEST_ASSERT_EQUAL_UINT8(before.V, cpu.V);
    TEST_ASSERT_EQUAL_UINT8(before.N, cpu.N);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(CLC_Will_Clear_Carry_Flag);
    RUN_TEST(SEC_Will_Set_Carry_Flag);
    RUN_TEST(CLD_Will_Clear_Decimal_Flag);
    RUN_TEST(SED_Will_Set_Decimal_Flag);
    RUN_TEST(CLI_Will_Clear_Interrupt_Flag);
    RUN_TEST(SEI_Will_Set_Interrupt_Flag);
    RUN_TEST(CLV_Will_Clear_Overflow_Flag);

    return UNITY_END();
}