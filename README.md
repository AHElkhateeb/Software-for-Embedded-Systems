# Alarm Clock based on Event-Driven Finite-State Machine Model

Implementation of an alarm clock using an event-based FSM and scheduler on the ATmega128RFA1 microcontroller

![alt text](https://github.com/AHElkhateeb/Software-for-Embedded-Systems/blob/main/fsm.png?raw=true)

### Functionality:
- When the microcontroller is powered up, the actual time is not known. The display shows an uninitialized clock using the format HH:MM. 
- The user has to set the time manually using the buttons. Firstly, the hour has to be set either by repeatedly pressing the Rotary Button or by rotating it. After pressing the Joystick Button, the minutes have to be set similarly via the Rotary Button. And finally pressing the Joystick Button again updates the system time and starts the clock.
- In this normal operation mode the time is shown in the format HH:MM:SS. In this state, the user can press the Rotary Button to enable the alarm or to disable the alarm.
- If the Joystick Button is pressed, the alarm time can be set. In this mode line 1 shows the alarm time instead of the current system time using the format HH:MM.
- If the alarm is enabled and the actual time matches the alarm time, the red LED shall toggle with 4 Hz.
- The alarm shall stop, if any button is pressed or 5 seconds have passed.
- The alarm must only be triggered if the clock is in its normal operating mode, i.e., it must not be triggered while the alarm time is being modified.

## Task Scheduler
Instead of using multiple hardware timers, a task scheduler is implemented using one hardware timer to manage mutliple tasks and allow the execution of them in parallel after a given time period in a synchronous context (not inside the interrupt service routine) since it avoids executing multiple time-consuming functions inside an interrupt service routine which is disadvantageous because it blocks other interrupts.

In order to provide a synchronous execution of tasks, the scheduler periodically polls for executable tasks inside the scheduler_run() function. The function scheduler_init() initializes the scheduler and Timer2.

All tasks are represented by a linked list data structure. Each task is described by a function pointer to the function to execute (taskDescriptor.task). A task is scheduled for execution after a fixed time period given by taskDescriptor.expire. The scheduler also uses this variable to count down the milliseconds until the task should be executed. Tasks can be scheduled for single execution (taskDescriptor.period==0) or periodic execution (taskDescriptor.period>0). “Single execution tasks” are removed after execution; periodic tasks remain in the task list and are repeated depending on their period.
