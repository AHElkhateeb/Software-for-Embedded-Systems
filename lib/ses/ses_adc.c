/* INCLUDES ******************************************************************/
#include "ses_adc.h"
#include <util/delay.h>

/* DEFINES & MACROS **********************************************************/

// Sensors wiring on SES board
#define SENSOR_TEMPERATURE_PORT 	PORTF
#define SENSOR_TEMPERATURE_PIN      	2

#define SENSOR_LIGHT_PORT       	PORTF
#define SENSOR_LIGHT_PIN         	    4

#define SENSOR_JOYSTICK_PORT       	PORTF
#define SENSOR_JOYSTICK_PIN         	5

#define SENSOR_MICROPHONE_PORT       	PORTF
#define SENSOR_MICROPHONE_PIN1         	0
#define SENSOR_MICROPHONE_PIN2         	1

#define ADC_VREF_SRC         	      192                //Voltage reference equivelant to 1.6V.

#define ADC_PRESCALE         	      3                  //Prescale reference to divide clock speed by 8.

#define ADC_TEMP_MAX                  40
#define ADC_TEMP_MIN                  20
#define ADC_TEMP_RAW_MAX              51                  // adc value for temperature of 40 degrees 
#define ADC_TEMP_RAW_MIN              481                 // adc value for temperature of 20 degrees 
#define ADC_TEMP_FACTOR               100                 // factor to avoid rounding the slope to zero.

/* FUNCTION DEFINITION *******************************************************/

void adc_init(void) {
     // configuring sensors ports to be inputs
DDR_REGISTER(SENSOR_TEMPERATURE_PORT) &= ~(1<<SENSOR_TEMPERATURE_PIN);    
DDR_REGISTER(SENSOR_LIGHT_PORT) &= ~(1<<SENSOR_LIGHT_PIN);
DDR_REGISTER(SENSOR_JOYSTICK_PORT) &= ~(1<<SENSOR_JOYSTICK_PIN);
DDR_REGISTER(SENSOR_MICROPHONE_PORT) &= ~(1<<SENSOR_MICROPHONE_PIN1);
DDR_REGISTER(SENSOR_MICROPHONE_PORT) &= ~(1<<SENSOR_MICROPHONE_PIN2);

    // Disable pullup resistors for sensors ports
SENSOR_TEMPERATURE_PORT &= ~(1<<SENSOR_TEMPERATURE_PIN);
SENSOR_LIGHT_PORT &= ~(1<<SENSOR_LIGHT_PIN);
SENSOR_JOYSTICK_PORT &= ~(1<<SENSOR_JOYSTICK_PIN);
SENSOR_MICROPHONE_PORT &= ~(1<<SENSOR_MICROPHONE_PIN1);
SENSOR_MICROPHONE_PORT &= ~(1<<SENSOR_MICROPHONE_PIN2);

PRR0 &= ~(1<<PRADC);              //Disabling power reduction mode for the ADC module.
ADMUX |= ADC_VREF_SRC;            //Configuring the voltage reference in register ADMUX.
ADMUX &= ~(1<<ADLAR);            //Clearing the ADLAR bit to right adjust the result. 
ADCSRA |= ADC_PRESCALE;      //configure the prescaler in register ADCSRA.
ADCSRA &= ~(1<<ADATE);           //Disabling auto triggering (ADATE).
ADCSRA |= (1<<ADEN);            //Enabling the ADC (ADEN).
}

uint16_t adc_read(uint8_t adc_channel) {
    if(adc_channel>=(uint8_t)ADC_NUM){return ADC_INVALID_CHANNEL;}         // return if channel is not valid
    ADMUX &= 0xf0;                                                         // clear previously chosen channel 
    ADMUX |= (adc_channel&0x0f);                                           // choose new channel 
    ADCSRA |= (1<<ADSC);                                                   // start conversion 
    _delay_us(20);                                                         // wait 20 us for first conversion as explained in data sheet
    while(ADCSRA & (1<<ADSC));                                             // polling for the ADSC bit to be cleared by hardware
    return (ADC);                                                          // obtain conversion value from ADC register
}

int16_t adc_getLight() {
    // read light sensor data 
return adc_read(ADC_LIGHT_CH);            

}

int16_t adc_getTemperature() {
    // read temperature data in tenth of degrees
int16_t adc = adc_read(ADC_TEMP_CH);
int16_t slope = ((ADC_TEMP_MAX - ADC_TEMP_MIN)*ADC_TEMP_FACTOR) / (ADC_TEMP_RAW_MAX - ADC_TEMP_RAW_MIN);
int16_t offset = (ADC_TEMP_MAX - (ADC_TEMP_RAW_MAX * slope))*ADC_TEMP_FACTOR;
return (adc * slope + offset) / ADC_TEMP_FACTOR;           
}

