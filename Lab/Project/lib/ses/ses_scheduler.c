/*INCLUDES ************************************************************/
#include "ses_timer.h"
#include "ses_scheduler.h"
#include "util/atomic.h"

/* PRIVATE VARIABLES **************************************************/
/** list of scheduled tasks */
static taskDescriptor *taskList = NULL;
static volatile systemTime_t system_time = 0;           // private variable to store time

/*FUNCTION DEFINITION *************************************************/
static void scheduler_update(void)
{
	system_time++;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)                    // use atomic block to prevent interrupts 
	{
	taskDescriptor *link = taskList;                     // create a pointer to traverse the the list
	while (link != NULL)
	{
		if (link->expire != 0)
		{
			if(--(link->expire) == 0)                     // if the expire is 0 set flag to execute the task
			{
				link -> execute =1;
			}
		}
		link = link -> next;                              // move to the next task in the list
	}
    }
}

void scheduler_init()
{
	timer2_start();                                            // configure the timer bits
	timer2_setCallback(&scheduler_update);                     // assign call back function to the timer 
}

void scheduler_run()
{
	
	while (true)
	{

		taskDescriptor *link = taskList; // create a pointer to traverse the the list

		while (link != NULL)
		{
			if (link->execute == 1)
			{
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
				{
					(link->task)(link->param); // call the task function to perform it
				}

				if ((link->period) == 0)
				{
					scheduler_remove(link); // if the task period = 0 remove the task from the list
					//break;
				}
				else
				{
					link->expire = link->period; // reset the task expire and the execution flag
					link->execute = 0;
				}
			}
			link = link->next; // move to the next task in the list
		}
	}
}

bool scheduler_add(taskDescriptor *toAdd)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
	if (toAdd == NULL)
	{
		return false;                               // if pointer is not valid reture false
	}
	if (taskList == NULL)
	{
		taskList = toAdd;                            // if list is empty assign the given task as header to the list
		return true;
	}
	taskDescriptor *link = taskList;                  // create pointer to traverse the list
	while (link->next != NULL)
	{
		if (link->next == toAdd)
		{
			return false;                              // if task already exists reture false and do not add it again
		}
		link = link->next;
	}
	link->next = toAdd;                                 // adding task to the list and reture true
	return true;
	}
}

void scheduler_remove(taskDescriptor *toRemove)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
	if (taskList == NULL)
	{
		return;                                           // if list is empty return
	}
	if (taskList == toRemove)
	{
		taskList = taskList->next;                        // if the task to remove is first task change list header
		return;
	}
	taskDescriptor *link = taskList;                      // create pointer to traverse the list
	while (link->next != toRemove)
	{
		if (link->next == NULL)
		{
			return;                                        // if the task is not found return 
		}
		link = link->next;
	}
	link->next = toRemove->next;                            // if task found remove it
	toRemove-> next = NULL;
	}
}

void scheduler_setTime(systemTime_t time)
{
	system_time = time;											// set given time as system time
} 

systemTime_t scheduler_getTime()
{
	return system_time;											// return system time
}
