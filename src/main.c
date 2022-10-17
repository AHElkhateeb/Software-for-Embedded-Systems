/*INCLUDES-------------------------------------------------------------------*/
#include <ses_scheduler.h>
#include <ses_led.h>
#include <ses_button.h>
#include <ses_rotary.h>
#include <ses_lcd.h>
#include "util/atomic.h"
#include <ses_timer.h>
#include <Fsm.h>


// ** Finite State Machine States ** -------------------------------------------

fsmReturnStatus setH_state(Fsm * fsm, const Event * e);
fsmReturnStatus setM_state(Fsm * fsm, const Event * e);
fsmReturnStatus clock_state(Fsm * fsm, const Event * e);
fsmReturnStatus setH_Alarm_state(Fsm * fsm, const Event * e);
fsmReturnStatus setM_Alarm_state(Fsm * fsm, const Event * e);
fsmReturnStatus alarm_active_state(Fsm * fsm, const Event * e);

// ** Functions Prototypes ---------------------------------------

static void joystickPressedDispatch(void * param);
static void RotaryPressedDispatch(void * param);
static void ClockwiseDispatch(void * param);
static void CounterClockwiseDispatch(void * param);
static void AlarmDisableDispatch();
static void AlarmFiredDispatch();
void check_alarm(void* para);
void update_display(void* para);
void toggle_green_led(void* para);
void toggle_red_led(void* para);
void alarm_disable(void* para);

// ** Global variables -----------------------------

static Fsm Clock_fsm = {NULL,false,{0,0,0,0}};
static volatile bool debounce_flag = true;
struct time_t alarmTime = {0,0,0,0};

// ** Tasks Descrebtion Initialization ------------------------------------------------------

struct taskDescriptor_s update_display_task = {&update_display, NULL, 0, 1000, 1, 0, NULL};
struct taskDescriptor_s toggle_green_led_task = {&toggle_green_led, NULL, 0, 1000, 1, 0, NULL};
struct taskDescriptor_s toggle_red_led_task = {&toggle_red_led, NULL, 0, 125, 1, 0, NULL};
struct taskDescriptor_s check_alarm_task =   {&check_alarm, NULL, 0, 1000, 1, 0, NULL};
struct taskDescriptor_s disable_alarm_task = {&alarm_disable, NULL, 5000, 5000, 0, 0, NULL};
struct taskDescriptor_s rotary_check_task = {&check_rotary, NULL, 0, 5, 1, 0, NULL};

int main()
{

    // ** assinging callback functions  ----------------------------
    button_setJoystickButtonCallback(&joystickPressedDispatch);             
    button_setRotaryButtonCallback(&RotaryPressedDispatch);
    rotary_setCounterClockwiseCallback(&CounterClockwiseDispatch);
    rotary_setClockwiseCallback(&ClockwiseDispatch);

    button_init(debounce_flag);                                      // initialize the button and configure the operation mode
    rotary_init();
    lcd_init();                                                     // lcd initialized 
    fsm_init((Fsm*)&Clock_fsm,&setH_state);                         // initialize the FSM 
    scheduler_init();
    led_yellowInit();
    led_greenInit();
    led_redInit();
    scheduler_add(&rotary_check_task);
    sei();                                                         // global interrupts enabled

    scheduler_run();                                              // run the scheduler in a infinite loop
}

// ** Schedular Tasks ** ------------------------------------------
void toggle_green_led(void* para)
{
    led_greenToggle();
}

void toggle_red_led(void* para)
{
    led_redToggle();
}

void alarm_disable(void* para)
{
    AlarmDisableDispatch();
}

void update_display(void* para)                             // calculate the current time and update the display
{
    systemTime_t time = scheduler_getTime();               // get time from scheduler
    Clock_fsm.timeSet.hour = (time/3600000)%24;            // extract hours
    time = time%3600000;
    Clock_fsm.timeSet.minute = time/60000;                // extract minutes
    time = time%60000;
    Clock_fsm.timeSet.second = time/1000;                 // extract seconds
    Clock_fsm.timeSet.milli = time%1000;                  // extract milliseconds

    lcd_clear();
    lcd_setCursor(0, 0);
    fprintf(lcdout, "%02d : %02d : %02d", Clock_fsm.timeSet.hour, Clock_fsm.timeSet.minute, Clock_fsm.timeSet.second);
}

void check_alarm(void* para)                          // function checks if the current time is equal to the alaram time every second
{
    systemTime_t time = scheduler_getTime();
    Clock_fsm.timeSet.hour = (time/3600000)%24;
    time = time%3600000;
    Clock_fsm.timeSet.minute = time/60000;

    if((Clock_fsm.timeSet.hour==alarmTime.hour)&&(Clock_fsm.timeSet.minute==alarmTime.minute)&&(Clock_fsm.isAlarmEnabled))
    {
        AlarmFiredDispatch();                      // Alarm is fired event
    }

}

// ** Events Dispatch ** ------------------------------------------
static void joystickPressedDispatch(void * param) 
{
    Event e = {.signal = JOYSTICK_PRESSED};
    fsm_dispatch(&Clock_fsm, &e);
}

static void AlarmFiredDispatch() 
{
    Event e = {.signal = ALARM_FIRED};
    fsm_dispatch(&Clock_fsm, &e);
}

static void RotaryPressedDispatch(void * param) 
{
    Event e = {.signal = Rotary_PRESSED};
    fsm_dispatch(&Clock_fsm, &e);
}

static void AlarmDisableDispatch() 
{
    Event e = {.signal = ALARM_DISABLE};
    fsm_dispatch(&Clock_fsm, &e);
}

static void ClockwiseDispatch(void * param) 
{
    Event e = {.signal = Clockwise_ROTATION};
    fsm_dispatch(&Clock_fsm, &e);
}

static void CounterClockwiseDispatch(void * param) 
{
    Event e = {.signal = CounterClockwise_ROTATION};
    fsm_dispatch(&Clock_fsm, &e);
}

// ** States Implementations ** ------------------------------------------
fsmReturnStatus setH_state(Fsm * fsm, const Event * e)
{
    switch (e->signal)
    {
    case ENTRY:                                      // update the display when entering the state
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",fsm->timeSet.hour,fsm->timeSet.minute);

        lcd_setCursor(0, 1);
        fprintf(lcdout, "Set Initial Hours");
        return RET_HANDLED; 
        break;

    case Rotary_PRESSED:
        if(++(fsm->timeSet.hour)==24){fsm->timeSet.hour=0;}             // increment sytem hour variable
        lcd_clear();
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",fsm->timeSet.hour,fsm->timeSet.minute);  // update display
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Set Initial Hours");
        return RET_HANDLED;
        break;

    case Clockwise_ROTATION:
        if(++(fsm->timeSet.hour)==24){fsm->timeSet.hour=0;}              // increment sytem hour variable
        lcd_clear();
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",fsm->timeSet.hour,fsm->timeSet.minute);    // update display
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Set Initial Hours");
        return RET_HANDLED;
        break;
    
    case CounterClockwise_ROTATION:
        if((fsm->timeSet.hour)--==0){fsm->timeSet.hour=23;}               // decrement sytem hour variable
        lcd_clear();
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",fsm->timeSet.hour,fsm->timeSet.minute);   // update display
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Set Initial Hours");
        return RET_HANDLED;
        break;

    case JOYSTICK_PRESSED:
        return TRANSITION(&setM_state);                           // transition to setting initial minutes state
        break;
    default:
        return RET_IGNORED;
        break;
    }
}

fsmReturnStatus setM_state(Fsm * fsm, const Event * e)
{
    switch (e->signal)
    {

    case ENTRY:                            // update the display when entering the state
        lcd_clear();
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",alarmTime.hour,alarmTime.minute);
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Set Initial Minutes");
        return RET_HANDLED;
    case Rotary_PRESSED:
        if(++(fsm->timeSet.minute)==60){fsm->timeSet.minute=0;}  // increment sytem minutes variable
        lcd_clear();
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",fsm->timeSet.hour,fsm->timeSet.minute);
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Set Initial Minutes");
        return RET_HANDLED;
        break;

    case Clockwise_ROTATION:
        if(++(fsm->timeSet.minute)==60){fsm->timeSet.minute=0;}  // increment sytem minutes variable
        lcd_clear();
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",fsm->timeSet.hour,fsm->timeSet.minute);
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Set Initial Minutes");
        return RET_HANDLED;
        break;
    
    case CounterClockwise_ROTATION:
        if((fsm->timeSet.minute)--==0){fsm->timeSet.minute=59;}   // decrement sytem minutes variable
        lcd_clear();
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",fsm->timeSet.hour,fsm->timeSet.minute);
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Set Initial Minutes");
        return RET_HANDLED;
        break;

    case JOYSTICK_PRESSED:
        return TRANSITION(&clock_state);                  // transition to clock state
        break;

    case EXIT:                                           // update time display and start the timer                                      
        scheduler_setTime((fsm->timeSet.hour)*3600000 + (fsm->timeSet.minute)*60000); 
        scheduler_add(&toggle_green_led_task);
        scheduler_add(&check_alarm_task);
        break;
    default:
        return RET_IGNORED;
        break;
    }
}

fsmReturnStatus clock_state(Fsm * fsm, const Event * e)
{
    switch (e->signal)
    {
    case ENTRY:
        scheduler_add(&update_display_task);             // add the task to update the display every second
        break;

    case JOYSTICK_PRESSED:
        return TRANSITION(&setH_Alarm_state);            // transition to the set alarm hour state
        
        break;
    
    case Rotary_PRESSED:
        fsm->isAlarmEnabled = ~(fsm->isAlarmEnabled);        // toggle alarm enable variable
        led_yellowToggle();                                  // toggle yellow led
        return RET_HANDLED;
        break;

    case ALARM_FIRED:
        return TRANSITION(&alarm_active_state);                // transition to alarm active state
        break;

    case EXIT:
        scheduler_remove(&update_display_task);                // remove the update display task
        break;
    default:
        return RET_IGNORED;
        break;
    }
}

fsmReturnStatus setH_Alarm_state(Fsm * fsm, const Event * e)
{
    switch (e->signal)
    {
    case ENTRY:                                  // display the initial alarm time
        lcd_clear();
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",alarmTime.hour,alarmTime.minute);
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Set Alarm Hour");
        return RET_HANDLED; 
        break;

    case JOYSTICK_PRESSED:
        return TRANSITION(&setM_Alarm_state);       // transition to setting alarm minutes state
        break;
    
    case Rotary_PRESSED:
        if(++(alarmTime.hour)==24){alarmTime.hour=0;}    // increment alarm hour variable
        lcd_clear();
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",alarmTime.hour,alarmTime.minute);  // update display
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Set Alarm Hour");
        return RET_HANDLED; 
        break;
    
    case Clockwise_ROTATION:
        if(++(alarmTime.hour)==24){alarmTime.hour=0;}      // increment alarm hour variable
        lcd_clear();
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",alarmTime.hour,alarmTime.minute);   // update display
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Set Alarm Hour");
        return RET_HANDLED;
        break;
    
    case CounterClockwise_ROTATION:
        if((alarmTime.hour)--==0){alarmTime.hour=23;}    // decrement alarm hour variable
        lcd_clear();
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",alarmTime.hour,alarmTime.minute);   // update display
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Set Alarm Hour");
        return RET_HANDLED;
        break;
    default:
        return RET_IGNORED;
        break;
    }

}
fsmReturnStatus setM_Alarm_state(Fsm * fsm, const Event * e)
{
    switch (e->signal)
    {
        
    case ENTRY:                     // display current alarm time
        lcd_clear();
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",alarmTime.hour,alarmTime.minute);
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Set Alarm Minutes");
        return RET_HANDLED;
    break;

    case JOYSTICK_PRESSED:
        return TRANSITION(&clock_state);     // transition to clock state after setting the alarm time
        break;
    
    case Rotary_PRESSED:
        if(++(alarmTime.minute)==60){alarmTime.minute=0;}  // increment alarm minutes variable
        lcd_clear();
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",alarmTime.hour,alarmTime.minute);    // update display
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Set Alarm Minutes");
        return RET_HANDLED; 
        break;

    case Clockwise_ROTATION:
        if(++(alarmTime.minute)==60){alarmTime.minute=0;}   // increment alarm minutes variable
        lcd_clear();
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",alarmTime.hour,alarmTime.minute);    // update display
        lcd_setCursor(0, 1);
       fprintf(lcdout, "Set Alarm Minutes");
        return RET_HANDLED;
        break;
    
    case CounterClockwise_ROTATION:
        if((alarmTime.minute)--==0){alarmTime.minute=59;}   // decrement alarm minutes variable
        lcd_clear();
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d : %02d",alarmTime.hour,alarmTime.minute);    // update display
        lcd_setCursor(0, 1);
       fprintf(lcdout, "Set Alarm Minutes");
        return RET_HANDLED;
        break;
    default:
        return RET_IGNORED;
        break;
    }

}

fsmReturnStatus alarm_active_state(Fsm * fsm, const Event * e)
{
    switch (e->signal)
    {

    case ENTRY:                               // add toggle red led and alarm disable tasks when entering the state
        scheduler_add(&toggle_red_led_task);
        scheduler_add(&disable_alarm_task);
        lcd_clear();
        lcd_setCursor(1, 1);
        fprintf(lcdout, "Time is NOW : ");
        lcd_setCursor(3, 2);
        fprintf(lcdout, "%02d : %02d", alarmTime.hour,alarmTime.minute);
        break;

    case JOYSTICK_PRESSED:
        AlarmDisableDispatch();                // Alarm disabled by detecting button press
        return RET_HANDLED;
        break;
    
    case Rotary_PRESSED:
        AlarmDisableDispatch();              // Alarm disabled by detecting button press
        return RET_HANDLED;
        break;
    
    case Clockwise_ROTATION:
        AlarmDisableDispatch();             // Alarm disabled by detecting rotary encoder rotation
        return RET_HANDLED;
        break;
    
    case CounterClockwise_ROTATION:
        AlarmDisableDispatch();             // Alarm disabled by detecting rotary encoder rotation
        return RET_HANDLED;
        break;

    case ALARM_DISABLE:
        return TRANSITION(&clock_state);            // transition to clock state after timer is disabled 
        break;

    case EXIT:                            // clear alarm enable variable and turn the yellow led off   
        fsm->isAlarmEnabled = false;
        led_yellowOff();
        scheduler_remove(&disable_alarm_task);             // remove the alarm disable task 
        scheduler_remove(&toggle_red_led_task);            // remove the red toggling task
        led_redOff();
        break;
    default:
        return RET_IGNORED;
        break;
    }

}