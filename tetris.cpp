#include "tetris.h"

static tetris_state_t tetris_state;
static int test_tick = 0;

void tetris_init(void) {
    int i, j;

    for (i = 0; i < TETRIS_PLAYFIELD_WIDTH; i++) {
        for (j = 0; j < TETRIS_PLAYFIELD_HEIGHT; j++) {
            tetris_state.playfield[i][j] = TETRIS_FIELD_EMPTY;
        }
    }
}

tetris_state_t *tetris_get_state(void) {
    return &tetris_state;
}

void tetris_tick(void) {
    if (test_tick >= TETRIS_PLAYFIELD_WIDTH * TETRIS_PLAYFIELD_HEIGHT)
        return;
    tetris_state.playfield[test_tick % TETRIS_PLAYFIELD_WIDTH][test_tick / TETRIS_PLAYFIELD_WIDTH] = TETRIS_FIELD_OCCUPIED;
    ++test_tick;
}
