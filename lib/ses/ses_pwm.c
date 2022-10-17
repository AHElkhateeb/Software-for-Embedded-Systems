/* INCLUDES ******************************************************************/
#include "ses_pwm.h"

/*FUNCTION DEFINITION ********************************************************/

void pwm_init(void)
{

    TCCR0A = 0x00; //reset the register
    TCCR0B = 0x00; //reset the register

    TCCR0A |= (1 << WGM00);               // choose fast PWM mode
    TCCR0A |= (1 << WGM01);
    TCCR0B &= ~(1 << WGM02);
    TCCR0A |= (1 << COM0B1);             // choose Compare mode
    TCCR0A |= (1 << COM0B0);
    TCCR0B |= (1 << CS00);              // disable prescaler
    TIMSK0 |= (1<< TOIE0);

    PRR0 &= ~(1 << PRTIM0);         //clear the PRTIM0 bit to enable Timer0
}

void pwm_setDutyCycle(uint8_t dutyCycle)
{
    OCR0B = dutyCycle;                             // set duty cycle
}