#include "Unity/unity.h"
#include "h6502.h"

#include <stdbool.h>

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU(&cpu, &mem);
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

void BEQ_Can_Branch_Forward_When_Zero_Is_Set(void)
{
    cpu.program_counter = 0xFF00;
    cpu.Z = 1;

    mem.data[0xFF00] = INS_BEQ;
    mem.data[0xFF01] = 0x1;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 3;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX16(0xFF03, cpu.program_counter);
    TEST_ASSERT_EQUAL_UINT8(before.PS, cpu.PS);
}

void BEQ_Does_Not_Branch_Forward_When_Zero_Is_Not_Set(void)
{
    cpu.program_counter = 0xFF00;
    cpu.Z = 0;

    mem.data[0xFF00] = INS_BEQ;
    mem.data[0xFF01] = 0x1;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX16(0xFF02, cpu.program_counter);
    TEST_ASSERT_EQUAL_UINT8(before.PS, cpu.PS);
}

void BEQ_Can_Branch_Forward_Into_New_Page_When_Zero_Is_Set(void)
{
    cpu.program_counter = 0xFEFD;
    cpu.Z = 1;

    mem.data[0xFEFD] = INS_BEQ;
    mem.data[0xFEFE] = 0x1;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 5;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX16(0xFF00, cpu.program_counter);
    TEST_ASSERT_EQUAL_UINT8(before.PS, cpu.PS);
}

void BEQ_Can_Branch_Backwards_When_Zero_Is_Set(void)
{
    cpu.program_counter = 0xFFCC;
    cpu.Z = 1;

    mem.data[0xFFCC] = INS_BEQ;
    mem.data[0xFFCD] = -1 * 0x2;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 3;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX16(0xFFCC, cpu.program_counter);
    TEST_ASSERT_EQUAL_UINT8(before.PS, cpu.PS);
}

void BEQ_Does_Not_Branch_Backwards_When_Zero_Is_Not_Set(void)
{
    cpu.program_counter = 0xFFCC;
    cpu.Z = 0;

    mem.data[0xFFCC] = INS_BEQ;
    mem.data[0xFFCD] = -1 * 0x2;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX16(0xFFCE, cpu.program_counter);
    TEST_ASSERT_EQUAL_UINT8(before.PS, cpu.PS);
}

void BEQ_Can_Branch_Bakwards_Into_New_Page_When_Zero_Is_Set(void)
{
    cpu.program_counter = 0xFF00;
    cpu.Z = 1;

    mem.data[0xFF00] = INS_BEQ;
    mem.data[0xFF01] = -1 * 0x3;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 5;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX16(0xFEFF, cpu.program_counter);
    TEST_ASSERT_EQUAL_UINT8(before.PS, cpu.PS);
}

void BEQ_Can_Branch_Backwards_When_Zero_Is_Set_From_Assemble_Code(void)
{
    // given:
    cpu.program_counter = 0xFFCC;
    cpu.Z = 1;

    /*
    loop
    lda #0
    beq loop
    */

    mem.data[0xFFCC] = 0xA9;
    mem.data[0xFFCC + 1] = 0x00;
    mem.data[0xFFCC + 2] = 0xF0;
    mem.data[0xFFCC + 3] = 0xFC;

    // when:
    const CPU before = cpu;
    const s32 NUM_OF_CYCLES = 5;

    const s32 cycles_used = Execute(NUM_OF_CYCLES, &mem);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX16(0xFFCC, cpu.program_counter);
    TEST_ASSERT_EQUAL_UINT8(before.PS, cpu.PS);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(BEQ_Can_Branch_Forward_When_Zero_Is_Set);
    RUN_TEST(BEQ_Does_Not_Branch_Forward_When_Zero_Is_Not_Set);
    RUN_TEST(BEQ_Can_Branch_Forward_Into_New_Page_When_Zero_Is_Set);
    RUN_TEST(BEQ_Can_Branch_Backwards_When_Zero_Is_Set);
    RUN_TEST(BEQ_Does_Not_Branch_Backwards_When_Zero_Is_Not_Set);
    RUN_TEST(BEQ_Can_Branch_Bakwards_Into_New_Page_When_Zero_Is_Set);
    RUN_TEST(BEQ_Can_Branch_Backwards_When_Zero_Is_Set_From_Assemble_Code);

    return UNITY_END();
}