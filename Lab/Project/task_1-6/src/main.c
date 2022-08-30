#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void)
{
    uint8_t var = 254;
    uint8_t A = 0;
    uint8_t B = 0;
    uint8_t xor = 0;

    // Revers bits
    for (int n = sizeof(var)*8/2 ; n > 0 ; n--)
    {
    A = (var >> (4-n)) & ((1 << 1) - 1);
    B = (var >> (n+3)) & ((1 << 1) - 1);
    xor = (A ^ B) << (4-n) | (A ^ B) << (n+3);
    var ^= xor;
    }

    printf("Bits reversed output is : %d \n", var);

}