/* INCLUDES ******************************************************************/
#include "ses_button.h"
#include <ses_timer.h>
#include <ses_uart.h>

/* DEFINES & MACROS **********************************************************/

#define ENCODER_BUTTON_PORT PORTB
#define ENCODER_BUTTON_PIN 6

#define JOYSTICK_BUTTON_PORT PORTB
#define JOYSTICK_BUTTON_PIN 7

#define BUTTON_NUM_DEBOUNCE_CHECKS 5
#define BUTTON_DEBOUNCE_PERIOD 30
#define BUTTON_DEBOUNCE_POS_JOYSTICK 0x01
#define BUTTON_DEBOUNCE_POS_ROTARY 0x02

/* GLOBAL VARIALBLES **********************************************************/
static volatile pButtonCallback RotaryButtonCallback;
static volatile pButtonCallback JoystickButtonCallback;

/* FUNCTION DEFINITION *******************************************************/

void button_init(bool debouncing)
{
    uart_init(57600);

    DDR_REGISTER(JOYSTICK_BUTTON_PORT) &= ~(1 << JOYSTICK_BUTTON_PIN);
    DDR_REGISTER(ENCODER_BUTTON_PORT) &= ~(1 << ENCODER_BUTTON_PIN);
    ENCODER_BUTTON_PORT |= (1 << ENCODER_BUTTON_PIN);
    JOYSTICK_BUTTON_PORT |= (1 << JOYSTICK_BUTTON_PIN);

    if (debouncing)
    {
        timer1_start(); // is asked to be removed in task 4.2
        timer1_setCallback(&button_checkState);
    }
    else
    { // if debounce is false configure the button change interrupt
        PCICR |= (1 << PCIE0);
        PCMSK0 |= (1 << ENCODER_BUTTON_PIN);
        PCMSK0 |= (1 << JOYSTICK_BUTTON_PIN);
        PCIFR |= (1 << PCIF0);
    }
}

bool button_isJoystickPressed(void)
{
    if (((PIN_REGISTER(JOYSTICK_BUTTON_PORT) >> JOYSTICK_BUTTON_PIN) & 1) == 0) // check if the JOYSTICK_BUTTON_PIN is cleared
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool button_isRotaryPressed(void)
{
    if (((PIN_REGISTER(ENCODER_BUTTON_PORT) >> ENCODER_BUTTON_PIN) & 1) == 0) // check if the ENCODER_BUTTON_PIN is cleared
    {
        return true;
    }
    else
    {
        return false;
    }
}

void button_setRotaryButtonCallback(pButtonCallback callback)
{
    RotaryButtonCallback = callback; // assign call back function to the global variable
}

void button_setJoystickButtonCallback(pButtonCallback callback)
{
    JoystickButtonCallback = callback; // assign call back function to the global variable
}

void button_checkState()
{
    static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = {}; // create an array for the button checks
    static uint8_t index = 0;
    static uint8_t debouncedState = 0;
    uint8_t lastDebouncedState = debouncedState; // save the last state for the button
    // each bit in every state byte represents one button
    state[index] = 0;
    if (button_isJoystickPressed())
    {
        state[index] |= BUTTON_DEBOUNCE_POS_JOYSTICK; // if button pressed set the corresponding bit
    }
    if (button_isRotaryPressed())
    {
        state[index] |= BUTTON_DEBOUNCE_POS_ROTARY; // if button pressed set the corresponding bit
    }
    index++; // increment the index for next check
    if (index == BUTTON_NUM_DEBOUNCE_CHECKS)
    {
        index = 0; // if num of checks are fullfilled reset index
    }
    // init compare value and compare with ALL reads, only if
    // we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "1's" in the state
    // array, the button at this position is considered pressed
    uint8_t j = 0xFF; // mask for obtaining the button state
    for (uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++)
    {
        j = j & state[i];
    }
    debouncedState = j;

    if(debouncedState != lastDebouncedState)
    {
        if (debouncedState == 0x01)
        {
            fprintf(uartout, "Joystick pressed\n");
            JoystickButtonCallback(); // if button is pressed for all checks call its function
        }

        if (debouncedState == 0x02)
        {
            fprintf(uartout, "Rotary pressed\n");
            RotaryButtonCallback(); // if button is pressed for all checks call its function
        }

        if (debouncedState == 0x03)
        {
            JoystickButtonCallback(); // if both buttons are pressed for all checks call their function
            RotaryButtonCallback();
            fprintf(uartout, "Joystick pressed");
            fprintf(uartout, "Rotary pressed");
        }
    }
}

ISR(PCINT0_vect) // interrupt service routine for button change
{
    if (((PCMSK0 >> ENCODER_BUTTON_PIN) & 1) == 1) // check if bit ENCODER_BUTTON_PIN is set
    {
        if (RotaryButtonCallback != NULL && button_isRotaryPressed() == true) // if call back is valid and button is pressed call the corresponding function
        {
            RotaryButtonCallback();
        }
    }

    if (((PCMSK0 >> JOYSTICK_BUTTON_PIN) & 1) == 1) // check if bit JOYSTICK_BUTTON_PIN is set
    {
        if (JoystickButtonCallback != NULL && button_isJoystickPressed() == true) // if call back is valid and button is pressed call the corresponding function
        {
            JoystickButtonCallback();
        }
    }
}