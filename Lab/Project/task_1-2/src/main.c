#include <stdint.h>
#include <avr/io.h>

void wait(uint16_t millis)
{
    for (uint32_t i = 3200 * millis; i > 0; i--) // for loop for the specified the number of empty clock cycles
    {
        asm volatile("nop"); // one cycle with no operation
        //volatile prevent the compiler from removing this line as an optimization step
        // We can also acheive the same thing by setting the settings for the compiler to low optimization mode
    }
}

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
