#include "tetris.h"
#include "tetris_figures.h"

static void tetris_get_next_figure(void);
static void draw_figure(void);
static void clear_figure(void);
static bool is_collide(void);
static void rotate_right(void);
static void rotate_left(void);

static tetris_state_t tetris_state;
static int current_brick_pos_x = 5;
static int current_brick_pos_y = -1;
static unsigned int current_figure = 0;
static unsigned int current_state = 0;
static char next_figure = 1;

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
    if (current_brick_pos_y == TETRIS_PLAYFIELD_HEIGHT - 1) {
        tetris_get_next_figure();
        current_brick_pos_y = 0;
        current_brick_pos_x = 5;
    } else {
        clear_figure();
        current_brick_pos_y++;
        if (is_collide()) {
            current_brick_pos_y--;
            draw_figure();
            tetris_get_next_figure();
            current_brick_pos_y = 0;
            current_brick_pos_x = 5;
        }
    }

    draw_figure();
}

void tetris_right_key(void) {
    clear_figure();
    current_brick_pos_x++;
    if (is_collide())
        current_brick_pos_x--;
    draw_figure();
}

void tetris_left_key(void) {
    clear_figure();
    current_brick_pos_x--;
    if (is_collide())
        current_brick_pos_x++;
    draw_figure();
}

void tetris_down_key(void) {
    tetris_tick();
}

void tetris_up_key(void) {
    clear_figure();
    rotate_right();
    if (is_collide()) {
        rotate_left();
    }
    draw_figure();
}

static void tetris_get_next_figure(void) {
    current_figure = next_figure;
    current_state = 0;

    next_figure = (next_figure + 1) % 5;
}

static void draw_figure(void) {
    int i, j;

    if (current_brick_pos_y < 0)
        return;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (tetris_figures[current_figure].states[current_state][i][j] == 1)
                tetris_state.playfield[current_brick_pos_x + i][current_brick_pos_y + j] = TETRIS_FIELD_OCCUPIED;
        }
    }
}

static void clear_figure(void) {
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (tetris_figures[current_figure].states[current_state][i][j] == 1)
                tetris_state.playfield[current_brick_pos_x + i][current_brick_pos_y + j] = TETRIS_FIELD_EMPTY;
        }
    }
}

static bool is_collide(void) {
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (tetris_figures[current_figure].states[current_state][i][j] == 1) {
                if (current_brick_pos_x + i < 0)
                    return true;
                if (current_brick_pos_x + i >= TETRIS_PLAYFIELD_WIDTH)
                    return true;
                if (current_brick_pos_y + j >= TETRIS_PLAYFIELD_HEIGHT)
                    return true;
                if (tetris_state.playfield[current_brick_pos_x + i][current_brick_pos_y + j] == TETRIS_FIELD_OCCUPIED)
                    return true;
            }
        }
    }

    return false;
}

static void rotate_right(void) {
    current_state = (current_state + 1) % tetris_figures[current_figure].states_num;
}

static void rotate_left(void) {
    if (current_state == 0)
        current_state = tetris_figures[current_figure].states_num - 1;
    else
        --current_state;
}
