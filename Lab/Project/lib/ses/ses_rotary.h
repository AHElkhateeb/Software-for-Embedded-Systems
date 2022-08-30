#ifndef SES_ROTARY_H_
#define SES_ROTARY_H_

/* INCLUDES ******************************************************************/

#include "ses_common.h"

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes rotary encoder
 */
void rotary_init();

typedef void (*pTypeRotaryCallback)();

/** functions to assign call back functions to the corresponding rotation direction 
 */
void rotary_setClockwiseCallback(pTypeRotaryCallback callback);
void rotary_setCounterClockwiseCallback(pTypeRotaryCallback callback);

/** fuction to perform the debounce functionality by checking the sequence completion
 */
void check_rotary(void);


#endif /* SES_ROTARY_H_ */
