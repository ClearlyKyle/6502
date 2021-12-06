#include "Unity/unity.h"
#include "h6502.h"

// https://github.com/ThrowTheSwitch/Unity

void setUp(void) {}    /* Is run before every test, put unit init calls here. */
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

void test_AverageThreeBytes_should_AverageMidRangeValues(void)
{
    TEST_ASSERT_EQUAL_HEX8(40, 40);
}

void test_AverageThreeBytes_should_AverageHighValues(void)
{
    TEST_ASSERT_EQUAL_HEX8(80, 80);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_AverageThreeBytes_should_AverageMidRangeValues);
    RUN_TEST(test_AverageThreeBytes_should_AverageHighValues);
    return UNITY_END();
}