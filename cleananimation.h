#ifndef _CLEANANIMATION_H_
#define _CLEANANIMATION_H_

#include "brick.h"

#define CLEANANIMATION_PROGRESS 0
#define CLEANANIMATION_DONE     1

void cleananimation_init(void);
int  cleananimation(brick_state_t *state);

#endif /* _CLEANANIMATION_H_ */
