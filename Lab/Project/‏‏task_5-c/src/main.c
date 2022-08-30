
/*INCLUDES-------------------------------------------------------------------*/
#include <ses_scheduler.h>
#include <ses_led.h>
#include <ses_button.h>
#include <ses_lcd.h>
#include <ses_pwm.h>
#include "util/atomic.h"
#include <ses_timer.h>
#include <ses_motorFrequency.h>
#include <ses_uart.h>

/* DEFINES & MACROS **********************************************************/
#define MOTOR_PORT       	PORTG
#define MOTOR_PIN         	5

/* FUNCTION PROTOTYPES *******************************************************/

void update_pid(void *para);
void joystick_pressed();
void motor_stopped();
uint16_t max(uint16_t num1, uint16_t num2);
uint16_t min(uint16_t num1, uint16_t num2);
void plotfreq();

/** function that return true if the joystick button is pressed
 */
struct taskDescriptor_s updateTimeStep_task = {&update_pid, NULL, 0, 5, 1, 0, NULL};

/*PARAMETERS-------------------------------------------------------------------*/

static volatile int current_error= 0;
static volatile int last_error= 0;
static volatile int Integral_error= 0;
static volatile int Integration_limit= 1000;
static volatile uint16_t target_freq = 300;
static volatile uint16_t Kp= 30;
static volatile uint16_t Ki= 1;
static volatile uint16_t Kd= 3;
static volatile bool JoystickPressed = false;
static volatile bool debounce_flag = true;

int main()
{
    DDR_REGISTER(MOTOR_PORT) |= (1<<MOTOR_PIN);                      //setting pin as output pin
    button_setJoystickButtonCallback(&joystick_pressed);             // assinging callback fuctions to buttons presses
    timer5_setCallback(&motor_stopped);
    button_init(debounce_flag);                                      // initialize the button and configure the operation mode
    scheduler_init();
    scheduler_add(&updateTimeStep_task);
    pwm_init();
    lcd_init();
    motorFrequency_init();
    timer5_start();
    sei();

}

void plotfreq()
{
        // Use the LCD pixel to graph here
}

void joystick_pressed()
{ 
    
    JoystickPressed = !JoystickPressed;
    if(JoystickPressed)
    {
        duty_cycle = 100;                                     // set duty cycle to 100% "full speed"
    }
    else
    {
        duty_cycle = 0;                                     // set duty cycle to 0% "stop the motor"
    }
}

void motor_stopped()
{ 
    led_greenOn();
    clearTimeElapsed();
}


void update_pid(void *para)
{
    current_error = target_freq - motorFrequency_getRecent();
    Integral_error = max(min((Integral_error+current_error),Integration_limit),-Integration_limit);
    duty_cycle = Kp*current_error + Ki*Integral_error + Kd*(last_error-current_error);

    if(duty_cycle>100){duty_cycle=100;}
    if(duty_cycle<0){duty_cycle=0;}

    last_error = current_error;
}

uint16_t max(uint16_t num1, uint16_t num2)
{
    return (num1 > num2 ) ? num1 : num2;
}

uint16_t min(uint16_t num1, uint16_t num2) 
{
    return (num1 > num2 ) ? num2 : num1;
}