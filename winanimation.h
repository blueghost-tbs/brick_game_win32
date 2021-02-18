#ifndef _WINANIMATION_H_
#define _WINANIMATION_H_

#include "brick.h"

#define WINANIMATION_PROGRESS 0
#define WINANIMATION_DONE     1

void winanimation_init(void);
int  winanimation(brick_state_t *state);

#endif /* _WINANIMATION_H_ */
