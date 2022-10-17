/* INCLUDES ******************************************************************/
#include "ses_motorFrequency.h"
#include <ses_led.h>
#include <ses_uart.h>
#include "util/atomic.h"

/* DEFINES & MACROS **********************************************************/

#define FREQUENCY_MEASUREMENT_PORT               PORTD
#define FREQUENCY_MEASUREMENT_PIN                0
#define N                                        150                           //No. of samples for median calculation

/* GLOBAL VARIABLES **********************************************************/

static volatile uint8_t rising_edges= 0;
static volatile uint16_t time_elapsed= 0;
static volatile uint16_t median_samples[N]= {};
static volatile uint8_t samples_counter= 0;


/*FUNCTION DEFINITION ********************************************************/

void motorFrequency_init()
{
    EICRA |= 0x03;                                                                   // configure the interrupt for every rising edge
    EIMSK |= (1 << INT0);
    EIFR  |= (1 << INTF0);
    DDR_REGISTER(FREQUENCY_MEASUREMENT_PORT) &= ~(1 << FREQUENCY_MEASUREMENT_PIN);   // configure the port as INPUT
    FREQUENCY_MEASUREMENT_PORT &= ~(1 << FREQUENCY_MEASUREMENT_PIN);                 // deactivate the pullup resistor
}

uint16_t motorFrequency_getRecent()
{

    if (time_elapsed == 0)
    {
        return 0;                               // return zero frequency if motor is not working
    }

    return 16000000 / (time_elapsed * 64);                     // return the frequency based on the prescaler and the speed of MC   
}

uint16_t motorFrequency_getMedian()
{
    if (time_elapsed == 0)
    {
        return 0;                               // return zero frequency if motor is not working
    }

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        Array_sort(&median_samples, N);
    }

    return 16000000 / (Find_median(median_samples, N) * 64);     // return the frequency based on the prescaler and the speed of MC with median filter applied
}

// function to sort the array in ascending order
void Array_sort(uint16_t *array , uint8_t n)
{ 
    // declare some local variables
    uint8_t i=0 , j=0;
    uint16_t temp=0;

    for(i=0 ; i<n ; i++)
    {
        for(j=0 ; j<n-1 ; j++)
        {
            if(array[j]>array[j+1])
            {
                temp        = array[j];
                array[j]    = array[j+1];
                array[j+1]  = temp;
            }
        }
    }
}

// function to calculate the median of the array
uint16_t Find_median(uint16_t array[] , uint8_t n)
{
    uint16_t median=0;
    
    // if number of elements are even
    if(n%2 == 0)                                      
        median = (array[(n-1)/2] + array[n/2])/2;              // if number of elements is even take average of middle 2
    // if number of elements are odd
    else
        median = array[n/2];                                   // if number of elements is odd take the middle value
    
    return median;
}

// function to clear the time elapsed variable to donate that the motor has stopped
void clearTimeElapsed()
{
    time_elapsed = 0;
}

ISR(INT0_vect) // interrupt service routine for rising edge
{
    if ((((EIMSK >> FREQUENCY_MEASUREMENT_PIN) & 1) == 1) && (((PIND >> FREQUENCY_MEASUREMENT_PIN) & 1) == 1)) // if mask is set and pin is high a rising edge is recorded
    {
        led_yellowToggle();                                // Toggle the yellow led
        rising_edges++;
        if (rising_edges == 1)
        {
            TCNT5 = (uint16_t)0x00;                        // we start the counting after the first edge is detected therefore we count from 1 to 7
        }
        led_greenOff();                                   // turn off the green led as motor starts moving 
        if (rising_edges == 7)                            // check if 6 edges is detected
        {
            if(TCNT5 > 200 )                             // if the number of clock cycles needed for 1 rotation < 200 cycle ingnore it " it is assumed to be noise"
            {
                time_elapsed = TCNT5;                              // store the time for 1 rotation
                median_samples[samples_counter] = time_elapsed;    // store a time sample for 1 rotation
                samples_counter++;                                 // increment counter to take a new reading next rotation
            }                           
            if (samples_counter == N)
            {
                samples_counter = 0;                        // if array is full, start overwriting the values from the start
            }
            rising_edges = 0;                               // reset the rising edges to start a new rotation
        }
    }
}