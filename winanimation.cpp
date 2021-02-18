#include "brick.h"
#include "winanimation.h"

static int winanimation_state = 0;

void winanimation_init() {
    winanimation_state = 0;
}

int winanimation(brick_state_t *state) {
    int i, j;
    
    if (winanimation_state < 150) {
        // Draw pattern
        for (i = 0; i < BRICK_PLAYFIELD_WIDTH; i++) {
            for (j = 0; j < BRICK_PLAYFIELD_HEIGHT; j++) {
                if ((j - 10) * (j - 10) + (i - 5) * (i - 5) < winanimation_state * 2)
                    state->playfield[i][j] = (((j - 10) * (j - 10) + (i - 5) * (i - 5) + 200 - winanimation_state) / 10) % 3 + 1;
            }
        }
        // Redraw the whole playfield
        state->rr.bottom = BRICK_PLAYFIELD_HEIGHT;
        state->rr.top = 0;
        state->rr.right = BRICK_PLAYFIELD_WIDTH;
        state->rr.left = 0;
        state->rr.clean = 0;
        winanimation_state++;
        return WINANIMATION_PROGRESS;
    }

    return WINANIMATION_DONE;
}
