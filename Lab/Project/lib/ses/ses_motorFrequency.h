#ifndef SES_MOTORFREQUENCY_H
#define SES_MOTORFREQUENCY_H

/*INCLUDES-------------------------------------------------------------------*/

#include "ses_common.h"

/* DEFINES & MACROS **********************************************************/

#define FREQUENCY_MEASUREMENT_PORT               PORTD
#define FREQUENCY_MEASUREMENT_PIN                0

/* FUNCTION PROTOTYPES *******************************************************/


/**
 * This function initialize timer used for generating the PWM and configure the port used for measuring the rising edges
 */
void motorFrequency_init();

/**
 * This function return the frequency of the motor based on the time taken to complete the complete rotation
 */
uint16_t motorFrequency_getRecent();

/**
 * This function return the frequency of the motor based on applying the median filter for the last N rotation times
 */
uint16_t motorFrequency_getMedian();

/**
 * This function is used to clear the time elapsed variable to donate that the motor has stopped 
 */
void clearTimeElapsed();

/**
 * This function is a helper function to sort a given array
 */
void Array_sort(uint16_t *array , uint8_t n);

/**
 * This function is  a helper function to return the middle value of a given array
 */
uint16_t Find_median(uint16_t array[] , uint8_t n);

#endif