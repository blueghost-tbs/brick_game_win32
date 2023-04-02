#include "brick.h"
#include "winanimation.h"

static int winanimation_state = 0;

void winanimation_init() {
    winanimation_state = 0;
}

int winanimation() {
    int i, j;

    // Draw pattern
    for (i = 0; i < BRICK_PLAYFIELD_WIDTH; i++) {
        for (j = 0; j < BRICK_PLAYFIELD_HEIGHT; j++) {
            if ((j - 10) * (j - 10) + (i - 5) * (i - 5) < winanimation_state * 2)
                brick_s.playfield[i][j] = (((j - 10) * (j - 10) + (i - 5) * (i - 5) + 5000 - winanimation_state) / 10) % 3 + 1;
        }
    }
    // Redraw the whole playfield
    brick_s.rr.bottom = BRICK_PLAYFIELD_HEIGHT;
    brick_s.rr.top = 0;
    brick_s.rr.right = BRICK_PLAYFIELD_WIDTH;
    brick_s.rr.left = 0;
    brick_s.rr.clean = 0;
    winanimation_state++;
    return WINANIMATION_PROGRESS;
}
