#include "brick.h"
#include "cleananimation.h"

static char cleananimation_line = 0;

void cleananimation_init() {
    cleananimation_line = 0;
}

int cleananimation() {
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
        brick_s.playfield[i][line_to_draw] = field_to_draw;
    }

    brick_s.rr.left = 0;
    brick_s.rr.right = BRICK_PLAYFIELD_WIDTH - 1;
    if (brick_s.rr.top > line_to_draw)
        brick_s.rr.top = line_to_draw;
    if (brick_s.rr.bottom < line_to_draw)
        brick_s.rr.bottom = line_to_draw;
    brick_s.rr.clean = 0;

    cleananimation_line++;

    return CLEANANIMATION_PROGRESS;
}
