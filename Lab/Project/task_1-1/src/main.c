#include <avr/io.h>
#include <util/delay.h>

/**Toggles the red LED of the SES-board*/
/* I was here ya |Abo 7meed w rbna ystor*/

int main(void)
{
	DDRG |= 0x02;
	while (1)
	{
		/* ana 5alet el timer 1000000 millisecond */
		/* La2 keteer 5aleha 850000 millisecond 7lw */
		_delay_ms(1000);
		PORTG ^= 0x02;
	}
	return 0;
}
