

// add constrains

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

// MOTOR wiring on SES board
#define MOTOR_PORT       	PORTG
#define MOTOR_PIN         	5



/* FUNCTION PROTOTYPES *******************************************************/

void joystick_pressed();
void motor_stopped();
void rpm_display(void *para);

struct taskDescriptor_s rpm_display_task = {&rpm_display, NULL, 0, 1000, 1, 0, NULL};        // creating a task that prints on LCD with an update rate of one second


/*PARAMETERS-------------------------------------------------------------------*/

static volatile bool JoystickPressed = false;
static volatile bool debounce_flag = true;


int main()
{
    DDR_REGISTER(MOTOR_PORT) |= (1<<MOTOR_PIN);                      //setting pin as an output pin
    button_setJoystickButtonCallback(&joystick_pressed);             // assinging callback function to buttons presses
    timer5_setCallback(&motor_stopped);                              // assinging callback function to timer 5 interrupt
    button_init(debounce_flag);                                      // initialize the button and configure the operation mode
    scheduler_init();
    led_greenInit(); 
    led_yellowInit();                                                         // initialize the yellow led                                                // initialize led, pwm, lcd, motor, timer and scheduler
    pwm_init();
    lcd_init();
    motorFrequency_init();
    timer5_start();
    scheduler_add(&rpm_display_task);                                // add task to scheduler
    uart_init(57600);
    sei();                                                           // enable global interrupts
    scheduler_run();                                                 // scheduler running in a superloop
}

// function that starts and stops the motor by joystick button presses
void joystick_pressed()
{ 
    JoystickPressed = !JoystickPressed;
    if(JoystickPressed)
    {
        duty_cycle = 0;                                     // set duty cycle to 0 "full speed"
    }
    else
    {
        duty_cycle = 255;                                      // set duty cycle to 255 "stop the motor"
    }
}

// function that turns on green led and clear time elapsed to donate a stopped motor
void motor_stopped()
{ 
    led_greenOn();
    clearTimeElapsed();
}

// function that prints the motor frequencies on LCD
void rpm_display(void *para)
{
    //fprintf(uartout, "Recent %u   Median %u\n", motorFrequency_getRecent(), motorFrequency_getMedian());
    lcd_clear();
    // first row
    lcd_setCursor(0, 0);
    fprintf(lcdout, "Motor Frequency : ");
    // second row
    lcd_setCursor(0, 1);
    fprintf(lcdout, "%u rpm", motorFrequency_getRecent()*60);
    // third row
    lcd_setCursor(0, 2);
    fprintf(lcdout, "Median is %u rpm", motorFrequency_getMedian()*60);
}

ISR(TIMER0_OVF_vect) {
    pwm_setDutyCycle(duty_cycle);                   // An interrupt routine used for updating the duty cycle "as mentioned in data sheet"
} 