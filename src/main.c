#include <stdio.h>
#include <stdlib.h>

#include "h6502.h"

int main(int argc, char const *argv[])
{

    reset_cpu(&cpu, &mem);

    // start - little program
    mem.data[0xFFFC] = INS_JSR;
    mem.data[0xFFFD] = 0x42;
    mem.data[0xFFFE] = 0x42;
    mem.data[0x4242] = INS_LDA_IM;
    mem.data[0x4243] = 0x84;
    // end - little program
    execute(9, &mem);

    printf("Program Exit...\n");
    return 0;
}
