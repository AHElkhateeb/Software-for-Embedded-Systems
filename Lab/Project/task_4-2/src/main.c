#include <main.h>

int main()
{
    sei();
    scheduler_init();                       // initialize the scheduler
    button_init(debounce_flag);             // initialize the button and configure the operation mode
    led_greenInit();                        // initialize leds
    led_yellowInit();

    lcd_init();                             // lcd initialization
    uart_init(57600);                       // uart initalization

    button_setJoystickButtonCallback(&joystick_pressed);             // assinging callback fuctions to buttons presses
    button_setRotaryButtonCallback(&rotary_pressed);

    scheduler_add(&led_toggle_task);                                 // add tasks to scheduler
    scheduler_add(&stop_watch_task);

    if (debounce_flag)
    {
        scheduler_add(&debounce_task);                                // if the debounce flag is true use scheduler for calling ...
    }                                                                 // the button_checkstate function 

    while (true)
    {
        scheduler_run();                                                // super loop run the scheduler operation
    }
}

void led_toggle(void *para)
{
    uint8_t color = (uint8_t *)para;
    switch (color)
    {
    case RED:
        led_redToggle();
        break;
    case GREEN:
        led_greenToggle();
        break;
    case YELLOW:
        led_yellowToggle();
        break;
    }
}

void joystick_pressed()
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        led_yellowToggle();
        if (isYellowON())
        {
            scheduler_add(&yellow_led_off_task);
        }
        else
        {
            scheduler_remove(&yellow_led_off_task);
        }
    }
}

void yellow_led_off(void *para)
{
    led_yellowOff();
    scheduler_remove(&yellow_led_off_task);
}

void rotary_pressed()
{
    if (stop_watch_state == true)
    {
        stop_watch_state = false;
    }
    else
    {
        stop_watch_state = true;
    }
}

void stop_watch(void *para)
{
    if (stop_watch_state)
    {
        stop_watch_time_tsec += 1;
        if (stop_watch_time_tsec > 9)
        {
            stop_watch_time_sec += 1;
            stop_watch_time_tsec = 0;
        }

        lcd_clear();
        // first row
        lcd_setCursor(0, 0);
        fprintf(lcdout, "Time Elapsed : ");
        // second row
        lcd_setCursor(0, 1);
        fprintf(lcdout, "%u : %u", stop_watch_time_sec, stop_watch_time_tsec);
    }
}
