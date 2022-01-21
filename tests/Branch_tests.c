#include "Unity/unity.h"
#include "h6502.h"

#include <stdbool.h>

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU(&cpu, &mem);
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

// BEQ (Branch on EQual)
// If the zero flag is clear when the CPU encounters a BEQ instruction,
// the CPU will continue at the instruction following the BEQ rather than taking the jump
// branch the amount stored in the next address (signed!)
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

// BNE (Branch on Not Equal)
void BNE_Can_Branch_Forward_When_Zero_Is_Not_Set(void)
{
    cpu.program_counter = 0xFF00;
    cpu.Z = 0;

    mem.data[0xFF00] = INS_BNE;
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

void BNE_Does_Not_Branch_Forward_When_Zero_Is_Set(void)
{
    cpu.program_counter = 0xFF00;
    cpu.Z = 1;

    mem.data[0xFF00] = INS_BNE;
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

void BNE_Can_Branch_Forward_Into_New_Page_When_Zero_Is_Not_Set(void)
{
    cpu.program_counter = 0xFEFD;
    cpu.Z = 0;

    mem.data[0xFEFD] = INS_BNE;
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

void BNE_Can_Branch_Backwards_When_Zero_Is_Not_Set(void)
{
    cpu.program_counter = 0xFFCC;
    cpu.Z = 0;

    mem.data[0xFFCC] = INS_BNE;
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

void BNE_Does_Not_Branch_Backwards_When_Zero_Is_Set(void)
{
    cpu.program_counter = 0xFFCC;
    cpu.Z = 1;

    mem.data[0xFFCC] = INS_BNE;
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

void BNE_Can_Branch_Bakwards_Into_New_Page_When_Zero_Is_Not_Set(void)
{
    cpu.program_counter = 0xFF00;
    cpu.Z = 0;

    mem.data[0xFF00] = INS_BNE;
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

// BCC (Branch on Carry Clear)
void BCC_Can_Branch_Forward_When_Carry_Is_Not_Set(void)
{
    cpu.program_counter = 0xFF00;
    cpu.C = 0;

    mem.data[0xFF00] = INS_BCC;
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

void BCC_Does_Not_Branch_Forward_When_Carry_Is_Set(void)
{
    cpu.program_counter = 0xFF00;
    cpu.C = 1;

    mem.data[0xFF00] = INS_BCC;
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

void BCC_Can_Branch_Forward_Into_New_Page_When_Carry_Is_Not_Set(void)
{
    cpu.program_counter = 0xFEFD;
    cpu.C = 0;

    mem.data[0xFEFD] = INS_BCC;
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

void BCC_Can_Branch_Backwards_When_Carry_Is_Not_Set(void)
{
    cpu.program_counter = 0xFFCC;
    cpu.C = 0;

    mem.data[0xFFCC] = INS_BCC;
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

void BCC_Does_Not_Branch_Backwards_When_Carry_Is_Set(void)
{
    cpu.program_counter = 0xFFCC;
    cpu.C = 1;

    mem.data[0xFFCC] = INS_BCC;
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

void BCC_Can_Branch_Bakwards_Into_New_Page_When_Carry_Is_Not_Set(void)
{
    cpu.program_counter = 0xFF00;
    cpu.C = 0;

    mem.data[0xFF00] = INS_BCC;
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

// BCS (Branch on Carry Set)
void BCS_Can_Branch_Forward_When_Carry_Is_Set(void)
{
    // Branch Forward
    cpu.program_counter = 0xFF00;
    cpu.C = 1;

    mem.data[0xFF00] = INS_BCS;
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

void BCS_Does_Not_Branch_Forward_When_Carry_Is_Not_Set(void)
{
    // Dont branch forward
    cpu.program_counter = 0xFF00;
    cpu.C = 0;

    mem.data[0xFF00] = INS_BCS;
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

void BCS_Can_Branch_Forward_Into_New_Page_When_Carry_Is_Set(void)
{
    // branch forward when condition is met
    cpu.program_counter = 0xFEFD;
    cpu.C = 1;

    mem.data[0xFEFD] = INS_BCS;
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

void BCS_Can_Branch_Backwards_When_Carry_Is_Set(void)
{
    // Branch backwards when condition is set
    cpu.program_counter = 0xFFCC;
    cpu.C = 1;

    mem.data[0xFFCC] = INS_BCS;
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

void BCS_Does_Not_Branch_Backwards_When_Carry_Is_Not_Set(void)
{
    // Dont branch backwards when condition is set
    cpu.program_counter = 0xFFCC;
    cpu.C = 0;

    mem.data[0xFFCC] = INS_BCS;
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

void BCS_Can_Branch_Bakwards_Into_New_Page_When_Carry_Is_Set(void)
{
    // Branch backwards into a new page when condition is met
    cpu.program_counter = 0xFF00;
    cpu.C = 1;

    mem.data[0xFF00] = INS_BCS;
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

// BVC (Branch on oVerflow Clear)
void BVC_Can_Branch_Forward_When_Overflow_Is_Not_Set(void)
{
    // Branch Forward
    cpu.program_counter = 0xFF00;
    cpu.V = 0;

    mem.data[0xFF00] = INS_BVC;
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

void BVC_Does_Not_Branch_Forward_When_Overflow_Is_Set(void)
{
    // Dont branch forward
    cpu.program_counter = 0xFF00;
    cpu.V = 1;

    mem.data[0xFF00] = INS_BVC;
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

void BVC_Can_Branch_Forward_Into_New_Page_When_Overflow_Is_Not_Set(void)
{
    // branch forward when condition is met
    cpu.program_counter = 0xFEFD;
    cpu.V = 0;

    mem.data[0xFEFD] = INS_BVC;
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

void BVC_Can_Branch_Backwards_When_Overflow_Is_Not_Set(void)
{
    // Branch backwards when condition is set
    cpu.program_counter = 0xFFCC;
    cpu.V = 0;

    mem.data[0xFFCC] = INS_BVC;
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

void BVC_Does_Not_Branch_Backwards_When_Overflow_Is_Set(void)
{
    // Dont branch backwards when condition is set
    cpu.program_counter = 0xFFCC;
    cpu.V = 1;

    mem.data[0xFFCC] = INS_BVC;
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

void BVC_Can_Branch_Bakwards_Into_New_Page_When_Overflow_Is_Not_Set(void)
{
    // Branch backwards into a new page when condition is met
    cpu.program_counter = 0xFF00;
    cpu.V = 0;

    mem.data[0xFF00] = INS_BVC;
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

// BVS (Branch on oVerflow Set)
void BVS_Can_Branch_Forward_When_Overflow_Is_Set(void)
{
    // Branch Forward
    cpu.program_counter = 0xFF00;
    cpu.V = 1;

    mem.data[0xFF00] = INS_BVS;
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

void BVS_Does_Not_Branch_Forward_When_Overflow_Is_Not_Set(void)
{
    // Dont branch forward
    cpu.program_counter = 0xFF00;
    cpu.V = 0;

    mem.data[0xFF00] = INS_BVS;
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

void BVS_Can_Branch_Forward_Into_New_Page_When_Overflow_Is_Set(void)
{
    // branch forward when condition is met
    cpu.program_counter = 0xFEFD;
    cpu.V = 1;

    mem.data[0xFEFD] = INS_BVS;
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

void BVS_Can_Branch_Backwards_When_Overflow_Is_Set(void)
{
    // Branch backwards when condition is set for branching
    cpu.program_counter = 0xFFCC;
    cpu.V = 1;

    mem.data[0xFFCC] = INS_BVS;
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

void BVS_Does_Not_Branch_Backwards_When_Overflow_Is_Not_Set(void)
{
    // Dont branch backwards when condition is set for not branching
    cpu.program_counter = 0xFFCC;
    cpu.V = 0;

    mem.data[0xFFCC] = INS_BVS;
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

void BVS_Can_Branch_Bakwards_Into_New_Page_When_Overflow_Is_Set(void)
{
    // Branch backwards into a new page when condition is set for branching
    cpu.program_counter = 0xFF00;
    cpu.V = 1;

    mem.data[0xFF00] = INS_BVS;
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

int main(void)
{
    UNITY_BEGIN();

    // BEQ jump when Z = 1
    // BNE jump when Z = 0

    RUN_TEST(BEQ_Can_Branch_Forward_When_Zero_Is_Set);
    RUN_TEST(BEQ_Does_Not_Branch_Forward_When_Zero_Is_Not_Set);
    RUN_TEST(BEQ_Can_Branch_Forward_Into_New_Page_When_Zero_Is_Set);
    RUN_TEST(BEQ_Can_Branch_Backwards_When_Zero_Is_Set);
    RUN_TEST(BEQ_Does_Not_Branch_Backwards_When_Zero_Is_Not_Set);
    RUN_TEST(BEQ_Can_Branch_Bakwards_Into_New_Page_When_Zero_Is_Set);
    RUN_TEST(BEQ_Can_Branch_Backwards_When_Zero_Is_Set_From_Assemble_Code);

    RUN_TEST(BNE_Can_Branch_Forward_When_Zero_Is_Not_Set);
    RUN_TEST(BNE_Can_Branch_Backwards_When_Zero_Is_Not_Set);
    RUN_TEST(BNE_Does_Not_Branch_Forward_When_Zero_Is_Set);
    RUN_TEST(BNE_Does_Not_Branch_Backwards_When_Zero_Is_Set);
    RUN_TEST(BNE_Can_Branch_Forward_Into_New_Page_When_Zero_Is_Not_Set);
    RUN_TEST(BNE_Can_Branch_Bakwards_Into_New_Page_When_Zero_Is_Not_Set);

    RUN_TEST(BCC_Can_Branch_Forward_When_Carry_Is_Not_Set);
    RUN_TEST(BCC_Does_Not_Branch_Forward_When_Carry_Is_Set);
    RUN_TEST(BCC_Can_Branch_Forward_Into_New_Page_When_Carry_Is_Not_Set);
    RUN_TEST(BCC_Can_Branch_Backwards_When_Carry_Is_Not_Set);
    RUN_TEST(BCC_Does_Not_Branch_Backwards_When_Carry_Is_Set);
    RUN_TEST(BCC_Can_Branch_Bakwards_Into_New_Page_When_Carry_Is_Not_Set);

    RUN_TEST(BCS_Can_Branch_Forward_When_Carry_Is_Set);
    RUN_TEST(BCS_Does_Not_Branch_Forward_When_Carry_Is_Not_Set);
    RUN_TEST(BCS_Can_Branch_Forward_Into_New_Page_When_Carry_Is_Set);
    RUN_TEST(BCS_Can_Branch_Backwards_When_Carry_Is_Set);
    RUN_TEST(BCS_Does_Not_Branch_Backwards_When_Carry_Is_Not_Set);
    RUN_TEST(BCS_Can_Branch_Bakwards_Into_New_Page_When_Carry_Is_Set);

    RUN_TEST(BVC_Can_Branch_Forward_When_Overflow_Is_Not_Set);
    RUN_TEST(BVC_Does_Not_Branch_Forward_When_Overflow_Is_Set);
    RUN_TEST(BVC_Can_Branch_Forward_Into_New_Page_When_Overflow_Is_Not_Set);
    RUN_TEST(BVC_Can_Branch_Backwards_When_Overflow_Is_Not_Set);
    RUN_TEST(BVC_Does_Not_Branch_Backwards_When_Overflow_Is_Set);
    RUN_TEST(BVC_Can_Branch_Bakwards_Into_New_Page_When_Overflow_Is_Not_Set);

    RUN_TEST(BVS_Can_Branch_Forward_When_Overflow_Is_Set);
    RUN_TEST(BVS_Does_Not_Branch_Forward_When_Overflow_Is_Not_Set);
    RUN_TEST(BVS_Can_Branch_Forward_Into_New_Page_When_Overflow_Is_Set);
    RUN_TEST(BVS_Can_Branch_Backwards_When_Overflow_Is_Set);
    RUN_TEST(BVS_Does_Not_Branch_Backwards_When_Overflow_Is_Not_Set);
    RUN_TEST(BVS_Can_Branch_Bakwards_Into_New_Page_When_Overflow_Is_Set);

    return UNITY_END();
}