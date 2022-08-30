/* INCLUDES ******************************************************************/

#include "ses_common.h"
#include "ses_led.h"

/* DEFINES & MACROS **********************************************************/

// LED wiring on SES board
#define LED_RED_PORT       	PORTG
#define LED_RED_PIN         	1

#define LED_YELLOW_PORT 	PORTF
#define LED_YELLOW_PIN      	7

#define LED_GREEN_PORT 		PORTF
#define LED_GREEN_PIN       	6

/* FUNCTION DEFINITION *******************************************************/

void led_redInit(void) {
    DDR_REGISTER(LED_RED_PORT) |= (1<<LED_RED_PIN);     // set bit LED_RED_PIN in DDRG register
    led_redOff();
}

void led_redToggle(void) {
    PIN_REGISTER(LED_RED_PORT) |= (1<<LED_RED_PIN);      // set bit LED_RED_PIN in PIN register
}

void led_redOn(void) {
    LED_RED_PORT &=~(1<<LED_RED_PIN);                    // clear bit LED_RED_PIN in PORTG register
}

void led_redOff(void) {
    LED_RED_PORT |=(1<<LED_RED_PIN);                     //set bit LED_RED_PIN in PORTG register
}

void led_yellowInit(void) {
    DDR_REGISTER(LED_YELLOW_PORT) |= (1<<LED_YELLOW_PIN);   // set bit LED_YELLOW_PIN in DDRF register
    led_yellowOff();
}

void led_yellowToggle(void) {
    PIN_REGISTER(LED_YELLOW_PORT) |= (1<<LED_YELLOW_PIN);   // set bit LED_YELLOW_PIN in PIN register
}

void led_yellowOn(void) {
    LED_YELLOW_PORT &= ~(1<<LED_YELLOW_PIN);                 // clear bit LED_YELLOW_PIN in PORTF register
}

void led_yellowOff(void) {
     LED_YELLOW_PORT |= (1<<LED_YELLOW_PIN);                  // set bit LED_YELLOW_PIN in PORTF register
}

void led_greenInit(void) {
    DDR_REGISTER(LED_GREEN_PORT) |= (1<<LED_GREEN_PIN);        // set bit LED_GREEN_PIN in DDRF register
    led_greenOff();
}

void led_greenToggle(void) { 
    LED_GREEN_PORT ^= (1<< LED_GREEN_PIN);         // set bit LED_GREEN_PIN in PIN register
}

void led_greenOn(void) {
    LED_GREEN_PORT &= ~(1<< LED_GREEN_PIN);                      // clear bit LED_GREEN_PIN in PORTF register
}

void led_greenOff(void) {
    LED_GREEN_PORT |= (1<< LED_GREEN_PIN);                       // set bit LED_GREEN_PIN in PORTF register
}

bool isYellowON(void)
{
    return (((LED_YELLOW_PORT >>LED_YELLOW_PIN)& 1) != 1);       // check if bit LED_YELLOW_PIN is cleared
}
