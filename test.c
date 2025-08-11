/* This file is not a part of simulation */

#include <stdio.h>
#include <stdint.h>

int main() {

    uint8_t n = -5;
    uint8_t m = (~n) + 0x1;
    printf("%d\n", m);
}