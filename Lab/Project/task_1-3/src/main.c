#include <stdint.h>
#include <avr/io.h>

//wait function//
void wait(uint16_t millis)
{
    uint32_t i = millis / 1000 * 16000000 / 9;
    uint32_t w;
    for (w = i; w > 0; w--)
    {
        //prevent code optimization by using inline assembler
        asm volatile("nop"); // one cycle with no operation
    }
}

/**Toggles the red LED of the SES-board*/
int main(void)
{
    DDRG |= 0x02;
    while (1)
    {
        wait(1000);
        PORTG ^= 0x02;
    }
    return 0;
}
