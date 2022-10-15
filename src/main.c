#include <stdio.h>
#include <stdlib.h>

#include "h6502.h"

int main(void)
{
    Reset_CPU();

    u8 data[MAX_MEM];

    // start - little program
    data[0xFFFC] = INS_JSR;
    data[0xFFFD] = 0x42;
    data[0xFFFE] = 0x42;
    data[0x4242] = INS_LDA_IM;
    data[0x4243] = 0x84;
    // end - little program

    Load_Program(data, MAX_MEM);

    Execute(9);

    Display_CPU_State();

    printf("Program Exit...\n");
    return 0;
}
