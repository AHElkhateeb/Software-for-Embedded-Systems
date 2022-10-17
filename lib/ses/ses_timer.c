/* INCLUDES ******************************************************************/
#include "ses_timer.h"

/* DEFINES & MACROS **********************************************************/
#define TIMER1_CYC_FOR_5MILLISEC    1250                      // value to be loaded in OCR1A for 5 ms timer interrupt
#define TIMER2_CYC_FOR_1MILLISEC	250                       // value to be loaded in OCR2A for 1 ms timer interrupt
#define TIMER5_CYC_FOR_200MILLISEC  50000                     // value to be loaded in OCR5A for 200 ms timer interrupt

/* GLOBAL VARIABLES **********************************************************/
static volatile pTimerCallback Timer0Callback;
static volatile pTimerCallback Timer1Callback;
static volatile pTimerCallback Timer2Callback;
static volatile pTimerCallback Timer5Callback;


/*FUNCTION DEFINITION ********************************************************/
void timer2_setCallback(pTimerCallback cb) {
	Timer2Callback = cb ; 
}

void timer2_start() {
	TCCR2A =  0x00 ;              //reset the register
    TCCR2B =  0x00 ;              //reset the register
	TIMSK2 |= (1<<OCIE2A);        // set interrupt mask 
	TIFR2 |= (1<<OCF2A);         // Clear Flag
	TCCR2A |= (1<<WGM21);         // choose ctc mode
	TCCR2A &= ~(1<<WGM20);
	TCCR2B |= (1<<CS22);          // set prescaler to be 64 
	TCNT2 = (uint8_t)0x00;        // start counter by value 0
	OCR2A = TIMER2_CYC_FOR_1MILLISEC;

}


void timer2_stop() {
    TIMSK2 &= ~(1<<OCIE2A);        // Clear interrupt mask 
}

void timer1_setCallback(pTimerCallback cb) {
	Timer1Callback = cb;
}

void timer1_start() {
	TCCR1A =  0x00 ;              //reset the register
    TCCR1B =  0x00 ;              //reset the register
	TIMSK1 |= (1<<OCIE1A);        // set interrupt mask 
	TIFR1 |= (1<<OCF1A);         // Clear Flag
	TCCR1A &= ~(1<<WGM11);         // choose ctc mode
	TCCR1A &= ~(1<<WGM10);
	TCCR1B |= (1<<WGM12);
	TCCR1B |= (1<<CS10);          // set prescaler to be 64
	TCCR1B |= (1<<CS11); 
	TCCR1B &= ~(1<<CS12); 
	TCNT1 = (uint16_t)0x00;        // start counter by value 0
	OCR1A = TIMER1_CYC_FOR_5MILLISEC;

}


void timer1_stop() {
	TIMSK1 &= ~(1<<OCIE1A);        // Clear interrupt mask 
}

void timer0_start() 
{
}
timer0_stop()
{

}
void timer0_setCallback(pTimerCallback cb) {
	Timer0Callback = cb;
}

void timer5_setCallback(pTimerCallback cb) {
	Timer5Callback = cb ; 
}

void timer5_start() {
	TCCR5A =  0x00 ;              //reset the register
    TCCR5B =  0x00 ;              //reset the register
	TIMSK5 |= (1<<OCIE5A);        // set interrupt mask 
	TIFR5 |= (1<<OCF5A);         // Clear Flag
	TCCR5A &= ~(1<<WGM51);         // choose ctc mode
	TCCR5A &= ~(1<<WGM50);
	TCCR5B |= (1<<WGM52);
	TCCR5B |= (1<<CS50);          // set prescaler to be 64
	TCCR5B |= (1<<CS51); 
	TCCR5B &= ~(1<<CS52); 
	TCNT5 = (uint16_t)0x00;        // start counter by value 0
	OCR5A = TIMER5_CYC_FOR_200MILLISEC;

}


void timer5_stop() {
	TIMSK5 &= ~(1<<OCIE5A);        // Clear interrupt mask 
}



ISR(TIMER1_COMPA_vect) {

	if(Timer1Callback != NULL)
        {
			Timer1Callback();
		}
	TCNT1 = (uint16_t)0x00;        // start counter by value 0
}

ISR(TIMER2_COMPA_vect) {

	if(Timer2Callback != NULL)
        {
			Timer2Callback();
		}
	TCNT2 = (uint8_t)0x00;        // start counter by value 0
}

ISR(TIMER5_COMPA_vect) {

	if(Timer5Callback != NULL)
        {
			Timer5Callback();
		}
	TCNT5 = (uint16_t)0x00;        // start counter by value 0
}