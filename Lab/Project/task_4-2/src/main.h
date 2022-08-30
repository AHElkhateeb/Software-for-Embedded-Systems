
/*INCLUDES-------------------------------------------------------------------*/
#include <ses_scheduler.h>
#include <ses_led.h>
#include <ses_button.h>
#include <ses_lcd.h>
#include "util/atomic.h"

/* FUNCTION PROTOTYPES *******************************************************/
/** function to toggle the given led 
 */
void led_toggle(void *para);

/** function that return true if the joystick button is pressed
 */
void joystick_pressed();

/** fuction that turn the yellow led off
 */
void yellow_led_off(void *para);

/** function that return true if the rotary button is pressed
 */
void rotary_pressed();

/** function print the time on teh lcd in the tenth of the seconds
 */
void stop_watch(void *para);

// Enumeration for the LEDs
enum Colors
{
    RED = 0,
    GREEN,
    YELLOW
};

/*TASKS DESCRITPTIONS-------------------------------------------------------------------*/
struct taskDescriptor_s led_toggle_task = {&led_toggle, GREEN, 0, 2000, 1, 0, NULL};
struct taskDescriptor_s yellow_led_off_task = {&yellow_led_off, NULL, 5000, 5000, 0, 0, NULL};
struct taskDescriptor_s stop_watch_task = {&stop_watch, NULL, 0, 100, 1, 0, NULL};
struct taskDescriptor_s debounce_task = {&button_checkState, NULL, 0, 5, 1, 0, NULL};

/*PARAMETES-------------------------------------------------------------------*/
static volatile bool stop_watch_state = false;
static volatile uint16_t stop_watch_time_sec = 0;
static volatile uint8_t stop_watch_time_tsec = 0;
static volatile bool debounce_flag = true;