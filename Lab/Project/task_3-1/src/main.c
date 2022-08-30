#include "ses_button.h"
#include "ses_timer.h"
#include "ses_led.h"
#include <ses_uart.h>

void redgreen_toggle();
void greenred_toggle();
void softwareTimer(void);
int main ()
{

    button_init(true);
    uart_init(57600);
    led_redInit();
    led_greenInit();
    led_yellowInit();
    timer2_start();
    button_setRotaryButtonCallback(&redgreen_toggle);
    button_setJoystickButtonCallback(& greenred_toggle);
    timer2_setCallback(&softwareTimer);
    sei();
    while(true)
    {

    }
  
}

void redgreen_toggle()
{
    //fprintf(uartout,"Inredgreen\n");
    led_redOn();
    led_greenOff();
}

void greenred_toggle()
{
   // fprintf(uartout,"Ingreenred\n");
    led_greenOn();
    led_redOff();
}
void softwareTimer()
{
    led_yellowToggle();

}
