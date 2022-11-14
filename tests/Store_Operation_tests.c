#include "Unity/unity.h"
#include "h6502.h"

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU();
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

void Verify_Unmodified_Flags_Store_Register(const CPU before, const CPU after)
{
    // get a copt of the cpu
    TEST_ASSERT_EQUAL_UINT8(before.C, after.C);
    TEST_ASSERT_EQUAL_UINT8(before.I, after.I);
    TEST_ASSERT_EQUAL_UINT8(before.D, after.D);
    TEST_ASSERT_EQUAL_UINT8(before.B, after.B);
    TEST_ASSERT_EQUAL_UINT8(before.V, after.V);
}

void Test_Store_Register_ZP(Opcode op, u8 *reg)
{
    (*reg) = 0x2F;

    mem.data[0xFFFC] = op;
    mem.data[0xFFFD] = 0x80;
    mem.data[0x0080] = 0x00;

    const CPU before          = cpu;
    const int EXPECTED_CYCLES = 3;

    const u32 cycles_used = Execute(EXPECTED_CYCLES);

    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX8(mem.data[0x0080], 0x2F);

    Verify_Unmodified_Flags_Store_Register(before, cpu);
}

void Test_Store_Register_ZP_X(Opcode op, u8 *reg)
{
    (*reg)          = 0x42;
    cpu.index_reg_X = 0x0F;

    mem.data[0xFFFC] = op;
    mem.data[0xFFFD] = 0x80;
    mem.data[0x008F] = 0x00;

    const CPU before          = cpu;
    const int EXPECTED_CYCLES = 4;

    const u32 cycles_used = Execute(EXPECTED_CYCLES);

    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX8(0x42, mem.data[0x008F]);

    Verify_Unmodified_Flags_Store_Register(before, cpu);
}

void Test_Store_Register_ZP_Y(Opcode op, u8 *reg)
{
    (*reg)          = 0x42;
    cpu.index_reg_Y = 0x0F;

    mem.data[0xFFFC] = op;
    mem.data[0xFFFD] = 0x80;
    mem.data[0x008F] = 0x00;

    const CPU before          = cpu;
    const int EXPECTED_CYCLES = 4;

    const u32 cycles_used = Execute(EXPECTED_CYCLES);

    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX8(mem.data[0x008F], 0x42);

    Verify_Unmodified_Flags_Store_Register(before, cpu);
}

void Test_Store_Register_ABS(Opcode op, u8 *reg)
{
    (*reg)           = 0x2F;
    mem.data[0xFFFC] = op;

    mem.data[0xFFFD] = 0x00;
    mem.data[0xFFFE] = 0x80;
    mem.data[0x8000] = 0x00;

    const CPU before          = cpu;
    const int EXPECTED_CYCLES = 4;

    const u32 cycles_used = Execute(EXPECTED_CYCLES);

    TEST_ASSERT_EQUAL_INT32(EXPECTED_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX8(mem.data[0x8000], 0x2F);

    Verify_Unmodified_Flags_Store_Register(before, cpu);
}

void STA_ZP_Can_Store_The_A_Register_Into_Memory(void)
{
    Test_Store_Register_ZP(INS_STA_ZP, &cpu.accumulator);
}

void STX_ZP_Can_Store_The_X_Register_Into_Memory(void)
{
    Test_Store_Register_ZP(INS_STX_ZP, &cpu.index_reg_X);
}

void STX_ZP_Can_Store_The_Y_Register_Into_Memory(void)
{
    Test_Store_Register_ZP_Y(INS_STX_ZP_Y, &cpu.index_reg_X);
}

void STY_ZP_Can_Store_The_Y_Register_Into_Memory(void)
{
    Test_Store_Register_ZP(INS_STY_ZP, &cpu.index_reg_Y);
}

// ABS
void STA_ABS_Can_Store_The_A_Register_Into_Memory(void)
{
    Test_Store_Register_ABS(INS_STA_ABS, &cpu.accumulator);
}

void STX_ABS_Can_Store_The_X_Register_Into_Memory(void)
{
    Test_Store_Register_ABS(INS_STX_ABS, &cpu.index_reg_X);
}

void STY_ABS_Can_Store_The_Y_Register_Into_Memory(void)
{
    Test_Store_Register_ABS(INS_STY_ABS, &cpu.index_reg_Y);
}

void STA_ZP_X_Can_Store_The_A_Register_Into_Memory(void)
{
    Test_Store_Register_ZP_X(INS_STA_ZP_X, &cpu.accumulator);
}

void STY_ZP_X_Can_Store_The_Y_Register_Into_Memory(void)
{
    Test_Store_Register_ZP_X(INS_STY_ZP_X, &cpu.index_reg_Y);
}

void STA_ABS_X_Can_Store_The_A_Register_Into_Memory(void)
{
    // given:
    cpu.accumulator  = 0x42;
    cpu.index_reg_X  = 0x0F;
    mem.data[0xFFFC] = INS_STA_ABS_X;
    mem.data[0xFFFD] = 0x00;
    mem.data[0xFFFE] = 0x80;

    const CPU before        = cpu;
    const int NUM_OF_CYCLES = 5;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX8(mem.data[0x800F], 0x42);

    Verify_Unmodified_Flags_Store_Register(before, cpu);
}

void STA_ABS_Y_Can_Store_The_A_Register_Into_Memory(void)
{
    // given:
    cpu.accumulator  = 0x42;
    cpu.index_reg_Y  = 0x0F;
    mem.data[0xFFFC] = INS_STA_ABS_Y;
    mem.data[0xFFFD] = 0x00;
    mem.data[0xFFFE] = 0x80;

    const CPU before        = cpu;
    const int NUM_OF_CYCLES = 5;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX8(mem.data[0x800F], 0x42);

    Verify_Unmodified_Flags_Store_Register(before, cpu);
}

void STA_IND_X_Can_Store_The_A_Register_Into_Memory(void)
{
    // given:
    cpu.accumulator  = 0x42;
    cpu.index_reg_X  = 0x0F;
    mem.data[0xFFFC] = INS_STA_IND_X;
    mem.data[0xFFFD] = 0x20;
    mem.data[0x002F] = 0x00;
    mem.data[0x0030] = 0x80;
    mem.data[0x8000] = 0x00;

    const CPU before        = cpu;
    const int NUM_OF_CYCLES = 6;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX8(mem.data[0x8000], 0x42);

    Verify_Unmodified_Flags_Store_Register(before, cpu);
}

void STA_IND_Y_Can_Store_The_A_Register_Into_Memory(void)
{
    // given:
    cpu.accumulator         = 0x42;
    cpu.index_reg_Y         = 0x0F;
    mem.data[0xFFFC]        = INS_STA_IND_Y;
    mem.data[0xFFFD]        = 0x20;
    mem.data[0x0020]        = 0x00;
    mem.data[0x0021]        = 0x80;
    mem.data[0x8000 + 0x0F] = 0x00;

    const CPU before        = cpu;
    const int NUM_OF_CYCLES = 6;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX8(0x42, mem.data[0x8000 + 0x0F]);

    Verify_Unmodified_Flags_Store_Register(before, cpu);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(STA_ZP_Can_Store_The_A_Register_Into_Memory);
    RUN_TEST(STX_ZP_Can_Store_The_X_Register_Into_Memory);
    RUN_TEST(STY_ZP_Can_Store_The_Y_Register_Into_Memory);
    RUN_TEST(STX_ZP_Can_Store_The_Y_Register_Into_Memory);
    RUN_TEST(STY_ZP_Can_Store_The_Y_Register_Into_Memory);

    RUN_TEST(STA_ABS_Can_Store_The_A_Register_Into_Memory);
    RUN_TEST(STX_ABS_Can_Store_The_X_Register_Into_Memory);
    RUN_TEST(STY_ABS_Can_Store_The_Y_Register_Into_Memory);

    RUN_TEST(STA_ZP_X_Can_Store_The_A_Register_Into_Memory);
    RUN_TEST(STY_ZP_X_Can_Store_The_Y_Register_Into_Memory);

    RUN_TEST(STA_ABS_X_Can_Store_The_A_Register_Into_Memory);
    RUN_TEST(STA_ABS_Y_Can_Store_The_A_Register_Into_Memory);

    RUN_TEST(STA_IND_X_Can_Store_The_A_Register_Into_Memory);
    RUN_TEST(STA_IND_Y_Can_Store_The_A_Register_Into_Memory);

    return UNITY_END();
}