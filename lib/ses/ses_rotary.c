/* INCLUDES ******************************************************************/
#include "ses_rotary.h"

/* DEFINES & MACROS **********************************************************/

#define A_ROTARY_PORT PORTB
#define A_ROTARY_PIN 5

#define B_ROTARY_PORT PORTG
#define B_ROTARY_PIN 2

#define CLOCKWISE_FINAL_SEQUENCE 515
#define COUNTERCLOCKWISE_FINAL_SEQUENCE 259
#define INITIAL_ROTARY_VALUE 3
/* GLOBAL VARIALBLES **********************************************************/
static volatile pTypeRotaryCallback ClockwiseCallback;                               // functions pointers for call back functions
static volatile pTypeRotaryCallback CounterClockwiseCallback;

/* FUNCTION DEFINITION *******************************************************/

void rotary_init()
{
    DDR_REGISTER(A_ROTARY_PORT) &= ~(1 << A_ROTARY_PIN);                  // configure pins as input 
    DDR_REGISTER(B_ROTARY_PORT) &= ~(1 << B_ROTARY_PIN);
    A_ROTARY_PORT |= (1 << A_ROTARY_PIN);                                // enable pullup-resistor
    B_ROTARY_PORT |= (1 << B_ROTARY_PIN);
}


void rotary_setClockwiseCallback(pTypeRotaryCallback callback)
{
    ClockwiseCallback = callback; // assign call back function to the global variable
}

void rotary_setCounterClockwiseCallback(pTypeRotaryCallback callback)
{
    CounterClockwiseCallback = callback; // assign call back function to the global variable
}

void check_rotary()
{
    static uint8_t i=0;
    static uint16_t j=0;
    static uint8_t sample = 0;
    uint8_t lastsample = sample;                 // saves the last state for the rotary encoder

    uint8_t a = ( PIN_REGISTER(A_ROTARY_PORT) >> A_ROTARY_PIN) & 1;          //read pin value
    uint8_t b = ( PIN_REGISTER(B_ROTARY_PORT) >> B_ROTARY_PIN) & 1;          //read pin value
    
    sample= a<<1 | b;

    if (sample != lastsample)                                       // if read value is changed store new value
    {
        j = j<<i | sample;                                         // shift read values by 2 postions
        i += 2;                 
        i = i%10;                                                  // if 1 sequence is detected return to 0 
    }

    if(j == CLOCKWISE_FINAL_SEQUENCE)                             // if sequence is met excute call back fucntion 
    {
        ClockwiseCallback();
    }
    
    if(j == COUNTERCLOCKWISE_FINAL_SEQUENCE)                     // if sequence is met excute call back fucntion 
    {
        CounterClockwiseCallback();
    }
    
    if(sample == INITIAL_ROTARY_VALUE)                          // if sequence is breaked restart storing values
    {
        i=2;
        j=sample;
    }
}