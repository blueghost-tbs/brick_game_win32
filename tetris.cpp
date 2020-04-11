#include "tetris.h"

static tetris_state_t tetris_state;
static int current_brick_pos_x = 0;
static int current_brick_pos_y = 0;

void tetris_init(void) {
    int i, j;

    for (i = 0; i < TETRIS_PLAYFIELD_WIDTH; i++) {
        for (j = 0; j < TETRIS_PLAYFIELD_HEIGHT; j++) {
            tetris_state.playfield[i][j] = TETRIS_FIELD_EMPTY;
        }
    }

    tetris_state.playfield[current_brick_pos_x][current_brick_pos_y] = TETRIS_FIELD_OCCUPIED;
}

tetris_state_t *tetris_get_state(void) {
    return &tetris_state;
}

void tetris_tick(void) {
    tetris_state.playfield[current_brick_pos_x][current_brick_pos_y] = TETRIS_FIELD_EMPTY;
    if (current_brick_pos_y < TETRIS_PLAYFIELD_HEIGHT - 1)
        current_brick_pos_y++;
    tetris_state.playfield[current_brick_pos_x][current_brick_pos_y] = TETRIS_FIELD_OCCUPIED;
}

void tetris_right_key(void) {
    tetris_state.playfield[current_brick_pos_x][current_brick_pos_y] = TETRIS_FIELD_EMPTY;
    if (current_brick_pos_x < TETRIS_PLAYFIELD_WIDTH - 1)
        current_brick_pos_x++;
    tetris_state.playfield[current_brick_pos_x][current_brick_pos_y] = TETRIS_FIELD_OCCUPIED;
}

void tetris_left_key(void) {
    tetris_state.playfield[current_brick_pos_x][current_brick_pos_y] = TETRIS_FIELD_EMPTY;
    if (current_brick_pos_x > 0)
        current_brick_pos_x--;
    tetris_state.playfield[current_brick_pos_x][current_brick_pos_y] = TETRIS_FIELD_OCCUPIED;
}
