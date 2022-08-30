#include "ses_lcd.h"
#include "ses_uart.h"
#include <util/delay.h>

int main ()
{
    uart_init(57600);
    lcd_init();

    while(true)
    {
        fprintf(uartout, "START\n");
        fprintf(lcdout, "START");
        _delay_ms(1000);
    }
}