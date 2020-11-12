#include "brick.h"
#include "cleananimation.h"

static char cleananimation_line = 0;

void cleananimation_init() {
    cleananimation_line = 0;
}

int cleananimation(brick_state_t *state) {
    char line_to_draw = cleananimation_line;
    char field_to_draw = BRICK_FIELD_OCCUPIED;
    int i;

    if (cleananimation_line < BRICK_PLAYFIELD_HEIGHT) {
        line_to_draw = BRICK_PLAYFIELD_HEIGHT - cleananimation_line - 1;
    } else if (cleananimation_line < BRICK_PLAYFIELD_HEIGHT * 2) {
        line_to_draw = cleananimation_line - BRICK_PLAYFIELD_HEIGHT;
        field_to_draw = BRICK_FIELD_EMPTY;
    } else {
        return CLEANANIMATION_DONE;
    }

    for (i = 0; i < BRICK_PLAYFIELD_WIDTH; i++) {
        state->playfield[i][line_to_draw] = field_to_draw;
    }

    state->rr.left = 0;
    state->rr.right = BRICK_PLAYFIELD_WIDTH - 1;
    if (state->rr.top > line_to_draw)
        state->rr.top = line_to_draw;
    if (state->rr.bottom < line_to_draw)
        state->rr.bottom = line_to_draw;
    state->rr.clean = 0;

    cleananimation_line++;

    return CLEANANIMATION_PROGRESS;
}
