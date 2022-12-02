#include "Unity/unity.h"
#include "h6502.h"

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU();
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

static void Verify_Unmodified_Flags(const CPU before, const CPU after)
{
    // get a copt of the cpu
    TEST_ASSERT_EQUAL_UINT8(before.C, after.C);
    TEST_ASSERT_EQUAL_UINT8(before.I, after.I);
    TEST_ASSERT_EQUAL_UINT8(before.D, after.D);
    TEST_ASSERT_EQUAL_UINT8(before.B, after.B);
    TEST_ASSERT_EQUAL_UINT8(before.V, after.V);
}

void RunLittleProgram(void)
{
    // start - little program
    mem.data[0xFFFC] = INS_JSR;
    mem.data[0xFFFD] = 0x42;
    mem.data[0xFFFE] = 0x42;
    mem.data[0x4242] = INS_LDA_IM;
    mem.data[0x4243] = 0x84;
    // end - little program

    const s32 NUM_CYCLES = 8;

    s32 cycles_used = Execute(NUM_CYCLES);
    TEST_ASSERT_EQUAL_INT32(NUM_CYCLES, cycles_used);

    // TEST_ASSERT_EQUAL(expected, actual)
    TEST_ASSERT_EQUAL(0x84, cpu.accumulator);
    TEST_ASSERT_EQUAL_UINT8(0x84, cpu.accumulator);
}

void CPU_Does_Nothing_When_Execute_Zero_Cycles(void)
{
    const s32 NUM_CYCLES = 0;

    s32 cycles_used = Execute(NUM_CYCLES);

    TEST_ASSERT_EQUAL_UINT32(0, cycles_used);
}

void CPU_Can_Execute_More_Cycles_Than_Requested_If_The_Instruction_Requires(void)
{
    mem.data[0xFFFC] = INS_LDA_IM;
    mem.data[0xFFFD] = 0x84;

    s32 cycles_used = Execute(1);
    TEST_ASSERT_EQUAL_INT32(2, cycles_used);
}

void Executing_A_Bad_Instruction_Does_Not_Start_Infinite_Loop(void)
{
    mem.data[0xFFFC] = 0x0; // invalud instruction
    mem.data[0xFFFD] = 0x0;

    const s32 cycles_used = Execute(1);
    TEST_ASSERT_EQUAL_INT32(1, cycles_used);
}

void LDA_Immediate_Can_Effect_The_Zero_Flag(void)
{
    cpu.Z           = 0;
    cpu.N           = 1;
    cpu.accumulator = 0x88; // junk value

    mem.data[0xFFFC] = INS_LDA_IM;
    mem.data[0xFFFD] = 0x0;

    // when:
    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // then:
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);

    TEST_ASSERT_TRUE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void Test_Loading_Register_Immediate(Opcode op, u8 *reg)
{
    cpu.Z = 1;
    cpu.N = 0;

    mem.data[0xFFFC] = op;
    mem.data[0xFFFD] = 0x84;

    const CPU before        = cpu;
    const int NUM_OF_CYCLES = 2;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // TEST_ASSERT_EQUAL(expected, actual)
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_UINT8(0x84, *reg);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_TRUE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void LDA_Immediate_Can_Load_A_Value_Into_A_Register(void)
{
    Test_Loading_Register_Immediate(INS_LDA_IM, &cpu.accumulator);
}

void LDX_Immediate_Can_Load_A_Value_Into_X_Register(void)
{
    Test_Loading_Register_Immediate(INS_LDX_IM, &cpu.index_reg_X);
}

void LDY_Immediate_Can_Load_A_Value_Into_Y_Register(void)
{
    Test_Loading_Register_Immediate(INS_LDY_IM, &cpu.index_reg_Y);
}

void Test_Loading_Register_Zero_Page(Opcode op, u8 *reg)
{
    cpu.Z = 1;
    cpu.N = 1;

    mem.data[0xFFFC] = op;
    mem.data[0xFFFD] = 0x42;
    mem.data[0x0042] = 0x37;

    const CPU before        = cpu;
    const int NUM_OF_CYCLES = 3;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    // TEST_ASSERT_EQUAL(expected, actual)
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);
    TEST_ASSERT_EQUAL_HEX8(0x37, *reg);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void LDA_ZP_Can_Load_A_Value_Into_A_Register(void)
{
    Test_Loading_Register_Zero_Page(INS_LDA_ZP, &cpu.accumulator);
}

void LDX_ZP_Can_Load_A_Value_Into_X_Register(void)
{
    Test_Loading_Register_Zero_Page(INS_LDX_ZP, &cpu.index_reg_X);
}

void LDY_ZP_Can_Load_A_Value_Into_Y_Register(void)
{
    Test_Loading_Register_Zero_Page(INS_LDY_ZP, &cpu.index_reg_Y);
}

void Test_Loading_Register_Zero_Page_X(Opcode op, u8 *reg)
{
    /*
        An instruction using zero page addressing mode has only an 8 bit address operand.
        addressing is limited to the first 256 bytes of memory (0x0000 to 0x00FF)

        > X register contains 0x0F
        > the instruction LDA 0x80,X is executed
        > the accumulator will be loaded from 0x008F

        e.g. 0x80 + 0x0F => 0x8F
    */
    cpu.Z           = 1;
    cpu.N           = 1;
    cpu.index_reg_X = 5;

    mem.data[0xFFFC] = op;
    mem.data[0xFFFD] = 0x42; // take this value "0x42" and add the "reg_X" value to it
    mem.data[0x0047] = 0x37; // read from the resulting memory address

    const CPU before        = cpu;
    const int NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);

    TEST_ASSERT_EQUAL_HEX8(0x37, *reg);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void Test_Loading_Register_Zero_Page_Y(Opcode op, u8 *reg)
{
    cpu.Z           = 1;
    cpu.N           = 1;
    cpu.index_reg_Y = 5;

    mem.data[0xFFFC] = op;
    mem.data[0xFFFD] = 0x42; // take this value "0x42" and add the "reg_X" value to it
    mem.data[0x0047] = 0x37; // read from the resulting memory address

    const CPU before        = cpu;
    const int NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);
    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);

    TEST_ASSERT_EQUAL_HEX8(0x37, *reg);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void LDA_ZP_X_Can_Load_A_Value_Into_A_Register(void)
{
    Test_Loading_Register_Zero_Page_X(INS_LDA_ZP_X, &cpu.accumulator);
}

void LDX_ZP_Y_Can_Load_A_Value_Into_X_Register(void)
{
    Test_Loading_Register_Zero_Page_Y(INS_LDX_ZP_Y, &cpu.index_reg_X);
}

void LDY_ZP_X_Can_Load_A_Value_Into_Y_Register(void)
{
    Test_Loading_Register_Zero_Page_X(INS_LDY_ZP_X, &cpu.index_reg_Y);
}

void LDA_ZP_X_Can_Load_A_Value_Into_A_Register_When_It_wraps(void)
{
    cpu.index_reg_X = 0xFF;

    mem.data[0xFFFC] = INS_LDA_ZP_X;
    mem.data[0xFFFD] = 0x80; // take this value "0x42" and add the "reg_X" value to it
    mem.data[0x007F] = 0x37; // read from the resulting memory address

    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);

    // TEST_ASSERT_EQUAL(expected, actual)
    TEST_ASSERT_EQUAL_HEX8(0x37, cpu.accumulator);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void Test_Loading_Register_Absolute(Opcode op, u8 *reg)
{
    cpu.Z = 1;
    cpu.N = 1;

    mem.data[0xFFFC] = op;
    mem.data[0xFFFD] = 0x80;
    mem.data[0xFFFE] = 0x44; // load from 0x4480
    mem.data[0x4480] = 0x37;

    const CPU before        = cpu;
    const int NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);

    // TEST_ASSERT_EQUAL(expected, actual)
    TEST_ASSERT_EQUAL_HEX8(0x37, *reg);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void LDA_ABS_Can_Load_A_Value_Into_The_A_Register(void)
{
    Test_Loading_Register_Absolute(INS_LDA_ABS, &cpu.accumulator);
}

void LDX_ABS_Can_Load_A_Value_Into_The_X_Register(void)
{
    Test_Loading_Register_Absolute(INS_LDX_ABS, &cpu.index_reg_X);
}

void LDY_ABS_Can_Load_A_Value_Into_The_Y_Register(void)
{
    Test_Loading_Register_Absolute(INS_LDY_ABS, &cpu.index_reg_Y);
}

void Test_Loading_Register_Absolute_X(Opcode op, u8 *reg)
{
    // Using the two values in the next two address (0xFFFD and 0xFFFD)
    // join them to make a new address and go to that address plus the value
    // of the X Register(1) (0x80 + 0x44 -> 0x4480 + 1 -> 0x4481)
    // load the value(0x37) at this address to the A register

    cpu.Z           = 1;
    cpu.N           = 1;
    cpu.index_reg_X = 1;

    mem.data[0xFFFC] = op;
    mem.data[0xFFFD] = 0x80;
    mem.data[0xFFFE] = 0x44; // load A from 0x4480
    mem.data[0x4481] = 0x37;

    const CPU before        = cpu;
    const int NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);

    TEST_ASSERT_EQUAL_HEX8(0x37, *reg);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void Test_Loading_Register_Absolute_Y(Opcode op, u8 *reg)
{
    cpu.Z           = 1;
    cpu.N           = 1;
    cpu.index_reg_Y = 1;

    mem.data[0xFFFC] = op;
    mem.data[0xFFFD] = 0x80;
    mem.data[0xFFFE] = 0x44; // Go to 0x4480
    mem.data[0x4481] = 0x37; // LDA from : 0x4480 + Y_reg -> 0x4481

    const CPU before        = cpu;
    const int NUM_OF_CYCLES = 4;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);

    TEST_ASSERT_EQUAL_HEX8(0x37, *reg);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void LDA_ABS_X_Can_Load_A_Value_Into_The_A_Register(void)
{
    Test_Loading_Register_Absolute_X(INS_LDA_ABS_X, &cpu.accumulator);
}

void LDY_ABS_X_Can_Load_A_Value_Into_The_Y_Register(void)
{
    Test_Loading_Register_Absolute_X(INS_LDY_ABS_X, &cpu.index_reg_Y);
}

void LDX_ABS_Y_Can_Load_A_Value_Into_The_X_Register(void)
{
    Test_Loading_Register_Absolute_Y(INS_LDX_ABS_Y, &cpu.index_reg_X);
}

void LDA_ABS_Y_Can_Load_A_Value_Into_The_A_Register(void)
{
    Test_Loading_Register_Absolute_Y(INS_LDA_ABS_Y, &cpu.accumulator);
}

void Test_Loading_Register_Absolute_X_Crossing_Page_Boundary(Opcode op, u8 *reg)
{
    cpu.Z           = 1;
    cpu.N           = 1;
    cpu.index_reg_X = 0x1;

    mem.data[0xFFFC] = op;
    mem.data[0xFFFD] = 0xFF;
    mem.data[0xFFFE] = 0x44; // load A from 0x4402 then add X Register
    mem.data[0x4500] = 0x37; // 0x4402 + X_reg(0xFF) -> 0x4501 corss page boundary!

    const CPU before        = cpu;
    const int NUM_OF_CYCLES = 5;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);

    TEST_ASSERT_EQUAL_HEX8(0x37, *reg);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void Test_Loading_Register_Absolute_Y_Crossing_Page_Boundary(Opcode op, u8 *reg)
{
    cpu.Z           = 1;
    cpu.N           = 1;
    cpu.index_reg_Y = 0x1;

    mem.data[0xFFFC] = op;
    mem.data[0xFFFD] = 0xFF;
    mem.data[0xFFFE] = 0x44; // load from 0x4402
    mem.data[0x4500] = 0x37; // 0x4402 + 0xFF - cross page boundary

    const CPU before        = cpu;
    const int NUM_OF_CYCLES = 5;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);

    TEST_ASSERT_EQUAL_HEX8(0x37, *reg);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void LDA_ABS_X_Can_Load_A_Value_Into_The_A_Register_When_Cross_Page_Boundary(void)
{
    Test_Loading_Register_Absolute_X_Crossing_Page_Boundary(INS_LDA_ABS_X, &cpu.accumulator);
}

void LDY_ABS_X_Can_Load_A_Value_Into_The_Y_Register_When_Cross_Page_Boundary(void)
{
    Test_Loading_Register_Absolute_X_Crossing_Page_Boundary(INS_LDY_ABS_X, &cpu.index_reg_Y);
}

void LDA_ABS_Y_Can_Load_A_Value_Into_The_A_Register_When_Cross_Page_Boundary(void)
{
    Test_Loading_Register_Absolute_Y_Crossing_Page_Boundary(INS_LDA_ABS_Y, &cpu.accumulator);
}

void LDX_ABS_Y_Can_Load_A_Value_Into_The_X_Register_When_Cross_Page_Boundary(void)
{
    Test_Loading_Register_Absolute_Y_Crossing_Page_Boundary(INS_LDX_ABS_Y, &cpu.index_reg_X);
}

// LDA Indirect
void LDA_IND_X_Can_Load_A_Value_Into_The_A_Register(void)
{
    cpu.index_reg_X = 0x04;

    mem.data[0xFFFC] = INS_LDA_IND_X;
    mem.data[0xFFFD] = 0x02;
    mem.data[0x0006] = 0x00; // 0x02 + 0x04 (X Register) < go to this address
    mem.data[0x0007] = 0x80; // 0x00 + 0x80 < go to this address
    mem.data[0x8000] = 0x37; // < set this value to the A register

    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 6;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);

    TEST_ASSERT_EQUAL_HEX8(0x37, cpu.accumulator);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

void LDA_IND_Y_Can_Load_A_Value_Into_The_A_Register(void)
{
    /*
    > take the value(0x02) in the next address(0xFFFD)
    > go to the values address(0x02)
    > take the value(0x00) at that address(0x0002) and add it to the next addresses(0x0003) value(0x80)
    > go to the resulting address(0x00 + 0x80 -> 0x8000)
    > now add the value of the Y register to this address (0x8000 + 0x04 -> 0x8004)
    > now add the value(0x37) at this address to the A register
    */

    cpu.index_reg_Y = 0x04;

    mem.data[0xFFFC] = INS_LDA_IND_Y;
    mem.data[0xFFFD] = 0x02;
    mem.data[0x0002] = 0x00; //
    mem.data[0x0003] = 0x80; //
    mem.data[0x8004] = 0x37; // 0x8000 + 0x4

    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 5;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);

    TEST_ASSERT_EQUAL_HEX8(0x37, cpu.accumulator);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);
    Verify_Unmodified_Flags(before, cpu);
}

void LDA_IND_Y_Can_Load_A_Value_Into_The_A_Register_When_Cross_Page_Boundary(void)
{
    cpu.index_reg_Y = 0xFF;

    mem.data[0xFFFC] = INS_LDA_IND_Y;
    mem.data[0xFFFD] = 0x02;
    mem.data[0x0002] = 0x02; //
    mem.data[0x0003] = 0x80; //
    mem.data[0x8101] = 0x37; // 0x8002 + 0xFF

    const CPU before        = cpu;
    const s32 NUM_OF_CYCLES = 6;

    const s32 cycles_used = Execute(NUM_OF_CYCLES);

    TEST_ASSERT_EQUAL_INT32(NUM_OF_CYCLES, cycles_used);

    TEST_ASSERT_EQUAL_HEX8(0x37, cpu.accumulator);
    TEST_ASSERT_FALSE(cpu.Z);
    TEST_ASSERT_FALSE(cpu.N);

    Verify_Unmodified_Flags(before, cpu);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(RunLittleProgram);
    RUN_TEST(CPU_Does_Nothing_When_Execute_Zero_Cycles);
    RUN_TEST(CPU_Can_Execute_More_Cycles_Than_Requested_If_The_Instruction_Requires);

    RUN_TEST(Executing_A_Bad_Instruction_Does_Not_Start_Infinite_Loop);

    RUN_TEST(LDA_Immediate_Can_Effect_The_Zero_Flag);

    RUN_TEST(LDA_Immediate_Can_Load_A_Value_Into_A_Register);
    RUN_TEST(LDX_Immediate_Can_Load_A_Value_Into_X_Register);
    RUN_TEST(LDY_Immediate_Can_Load_A_Value_Into_Y_Register);

    RUN_TEST(LDA_ZP_Can_Load_A_Value_Into_A_Register);
    RUN_TEST(LDX_ZP_Can_Load_A_Value_Into_X_Register);
    RUN_TEST(LDY_ZP_Can_Load_A_Value_Into_Y_Register);

    RUN_TEST(LDA_ZP_X_Can_Load_A_Value_Into_A_Register);
    RUN_TEST(LDX_ZP_Y_Can_Load_A_Value_Into_X_Register);
    RUN_TEST(LDY_ZP_X_Can_Load_A_Value_Into_Y_Register);
    RUN_TEST(LDA_ZP_X_Can_Load_A_Value_Into_A_Register_When_It_wraps);

    RUN_TEST(LDA_ABS_Can_Load_A_Value_Into_The_A_Register);
    RUN_TEST(LDX_ABS_Can_Load_A_Value_Into_The_X_Register);
    RUN_TEST(LDY_ABS_Can_Load_A_Value_Into_The_Y_Register);

    RUN_TEST(LDA_ABS_X_Can_Load_A_Value_Into_The_A_Register);
    RUN_TEST(LDY_ABS_X_Can_Load_A_Value_Into_The_Y_Register);
    RUN_TEST(LDX_ABS_Y_Can_Load_A_Value_Into_The_X_Register);
    RUN_TEST(LDA_ABS_Y_Can_Load_A_Value_Into_The_A_Register);

    RUN_TEST(LDA_ABS_X_Can_Load_A_Value_Into_The_A_Register_When_Cross_Page_Boundary);
    RUN_TEST(LDY_ABS_X_Can_Load_A_Value_Into_The_Y_Register_When_Cross_Page_Boundary);

    RUN_TEST(LDA_ABS_Y_Can_Load_A_Value_Into_The_A_Register_When_Cross_Page_Boundary);
    RUN_TEST(LDX_ABS_Y_Can_Load_A_Value_Into_The_X_Register_When_Cross_Page_Boundary);

    RUN_TEST(LDA_IND_X_Can_Load_A_Value_Into_The_A_Register);
    RUN_TEST(LDA_IND_Y_Can_Load_A_Value_Into_The_A_Register);
    RUN_TEST(LDA_IND_Y_Can_Load_A_Value_Into_The_A_Register_When_Cross_Page_Boundary);

    return UNITY_END();
}