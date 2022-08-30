#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BIT4AND6MASK 0b01010000
#define BIT2AND7MASK 0b10000100
#define BIT5AND7MASK 0b10100000
int main(void)

{
    uint8_t varr = 0b01101100;
    uint8_t num = 8;
    uint8_t out = 0 ;
    uint8_t temp = 0;

    for (int i = 0 ; i < num ; i++)
    {
        temp = (varr & (1<<i))>>i;
        out |= (temp<<num-i-1);
    }

    printf("Reversed Output is : %d \n", out);

    uint8_t var = 0; // declear and initialize the variable

    // set bit 3
    var |= (1 << 3);
    printf("Bit 3 is set Output is : %d \n", var);

    //Set bits 4 and 6 (with/without bit shifting)
    var |= BIT4AND6MASK; // Without shifting
    var |= (0b101 << 4); // with shifting
    var |= (1<<4 | 1<<6);
    printf("Bit 4 and 6 are set Output is : %d \n", var);

    //Clear bit 2
    var &= ~(1 << 2);
    printf("Bit 2 is cleared Output is : %d \n", var);

    //clear bits 2 and 7
    var &= ~BIT2AND7MASK;
    var &= ~(1<<2 | 1<< 7);
    printf("Bit 2 and 7 are cleared Output is : %d \n", var);

    // Toggle bit 3
    var ^= (1 << 3);
    printf("Bit 3 is toggled Output is : %d \n", var);
    
    var = 0;
    //Set bit 2 and clear bits 5 and 7 at the same time
    var = (var | (1 << 2)) | (var&~(1<<5 | 1<<7));
    printf("Bit 2 is set and 5 and 7 are cleared Output is : %d \n", var);

    // Swap bits 3-5 and bits 0-2
    uint8_t A = var & ((1 << 3) - 1);
    uint8_t B = (var >> 3) & ((1 << 3) - 1);
    uint8_t xor = (A ^ B) | (A ^ B) << 3; // can be written in one statement just divided for understandibility
    var ^= xor;
    printf("Bits 3-5 and 0-2 swapped Output is : %d \n", var);
    

    // Swap bits 3-5 and bits 0-2 (will be revised by Osama)
    var ^= ((((var & (1 << 0)) >> 0) ^ ((var & (1 << 2)) >> 2)) ? ((((var & (1 << 3)) >> 3) ^ ((var & (1 << 5)) >> 5)) ? (1 << 0) | (1 << 2) | (1 << 3) | (1 << 5) : (1 << 0) | (1 << 2)) : ((((var & (1 << 3)) >> 3) ^ ((var & (1 << 5)) >> 5)) ? (1 << 3) | (1 << 5) : 0));;
    printf("Bit 0-2 are swaped Output is : %d \n", var);

        var = 128;

        var = (var | (1<<0)^(1<<7));
        printf("Output is : %d \n", var);
/*------------------------------------------
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
------------------------------------------------ */ 

}