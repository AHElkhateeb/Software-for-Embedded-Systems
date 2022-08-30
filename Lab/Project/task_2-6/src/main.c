#include <util/delay.h>
#include "ses_lcd.h"
#include "ses_button.h"
#include "ses_led.h"
#include "ses_uart.h"
#include "ses_adc.h"

void printTime(float timeBoot) {
lcd_clear();
lcd_setCursor(0,0);
fprintf(lcdout,"Time since reset:");
lcd_setCursor(0,1);
fprintf(lcdout,"%.2f seconds",timeBoot);
}

void sendTime(float timeBoot) {
fprintf(uartout,"%.2f seconds since reset.\n",timeBoot);
}



int main(void)
{
	uart_init(57600);
	lcd_init();
    adc_init();
	button_init(false);
	led_redInit();
	led_greenInit();
	float timeBoot = 0.0;

	while (1)
	{
		printTime(timeBoot);
		sendTime(timeBoot);
		_delay_ms(2500);
        timeBoot ++;
        int16_t Tempvalue = adc_getTemperature();
        int16_t LightValue = adc_getLight();
        fprintf(uartout,"Temperature is %i  ْC\n",Tempvalue);
        fprintf(uartout,"Light intensity is %i\n",LightValue);

		if (button_isJoystickPressed())
		{
			led_greenOn();
		}
		else
		{
			led_greenOff();
		}

		if (button_isRotaryPressed())
		{
			led_redOn();
		}
		else
		{
			led_redOff();
		}
	}

	return 0;
}
