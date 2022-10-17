#ifndef SES_PWM_H
#define SES_PWM_H

/*INCLUDES-------------------------------------------------------------------*/

#include "ses_common.h"

/* DEFINES & MACROS **********************************************************/

static volatile uint8_t duty_cycle= 255;

/* FUNCTION PROTOTYPES *******************************************************/

//Function that initializes PWM by enabling timer0 and choosing fast PWM mode
void pwm_init(void);

//Function that updates the duty cycle
void pwm_setDutyCycle(uint8_t dutyCycle);

#endif