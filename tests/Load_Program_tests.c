#include "Unity/unity.h"
#include "h6502.h"

#include <stdbool.h>

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) /* Is run before every test, put unit init calls here. */
{
    Reset_CPU(&cpu, &mem);
}
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

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

    RUN_TEST(Test_Load_Program_Into_Correct_Area_Of_Memory);

    return UNITY_END();
}