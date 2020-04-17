#include "tetris.h"
#include "tetris_figures.h"

static void tetris_get_next_figure(void);
static void draw_figure(char need_redraw);
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

/******************************************************************************
 * Exported functions.
 ******************************************************************************/
void tetris_init(void) {
    int i, j;

    for (i = 0; i < TETRIS_PLAYFIELD_WIDTH; i++) {
        for (j = 0; j < TETRIS_PLAYFIELD_HEIGHT; j++) {
            tetris_state.playfield[i][j] = TETRIS_FIELD_EMPTY;
        }
    }

    tetris_reset_redraw_rectangle();
}

tetris_state_t *tetris_get_state(void) {
    return &tetris_state;
}

void tetris_tick(void) {
    if (current_brick_pos_y == TETRIS_PLAYFIELD_HEIGHT - 1) {
        tetris_get_next_figure();
        current_brick_pos_y = 0;
        current_brick_pos_x = 5;
        draw_figure(1);
    } else {
        clear_figure();
        current_brick_pos_y++;
        if (is_collide()) {
            current_brick_pos_y--;
            tetris_reset_redraw_rectangle();
            draw_figure(0);
            tetris_get_next_figure();
            current_brick_pos_y = 0;
            current_brick_pos_x = 5;
            draw_figure(1);
        } else
            draw_figure(1);
    }
}

void tetris_right_key(void) {
    clear_figure();
    current_brick_pos_x++;
    if (is_collide()) {
        current_brick_pos_x--;
        tetris_reset_redraw_rectangle();
        draw_figure(0);
    } else {
        draw_figure(1);
    }
}

void tetris_left_key(void) {
    clear_figure();
    current_brick_pos_x--;
    if (is_collide()) {
        current_brick_pos_x++;
        tetris_reset_redraw_rectangle();
        draw_figure(0);
    } else {
        draw_figure(1);
    }
}

void tetris_down_key(void) {
    tetris_tick();
}

void tetris_up_key(void) {
    clear_figure();
    rotate_right();
    if (is_collide()) {
        rotate_left();
        tetris_reset_redraw_rectangle();
        draw_figure(0);
    } else {
        draw_figure(1);
    }
}

void tetris_reset_redraw_rectangle(void) {
    tetris_state.rr.left = TETRIS_PLAYFIELD_WIDTH;
    tetris_state.rr.top = TETRIS_PLAYFIELD_HEIGHT;
    tetris_state.rr.right = 0;
    tetris_state.rr.bottom = 0;
    tetris_state.rr.clean = 1;
}

/******************************************************************************
 * Static functions.
 ******************************************************************************/
static void tetris_get_next_figure(void) {
    current_figure = next_figure;
    current_state = 0;

    next_figure = (next_figure + 1) % 5;
}

static void draw_figure(char need_redraw) {
    int i, j;
    int size = tetris_figures[current_figure].size;

    if (current_brick_pos_y < 0)
        return;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (current_brick_pos_x + i < 0 || current_brick_pos_y + j < 0)
                continue;
            if (tetris_figures[current_figure].states[current_state][i][j] == 1) {
                tetris_state.playfield[current_brick_pos_x + i][current_brick_pos_y + j] = TETRIS_FIELD_OCCUPIED;
                if (!need_redraw)
                    continue;
                if (current_brick_pos_x + i < tetris_state.rr.left) {
                    tetris_state.rr.left = current_brick_pos_x + i;
                    tetris_state.rr.clean = 0;
                }
                if (current_brick_pos_x + i > tetris_state.rr.right) {
                    tetris_state.rr.right = current_brick_pos_x + i;
                    tetris_state.rr.clean = 0;
                }
                if (current_brick_pos_y + j < tetris_state.rr.top) {
                    tetris_state.rr.top = current_brick_pos_y + j;
                    tetris_state.rr.clean = 0;
                }
                if (current_brick_pos_y + j > tetris_state.rr.bottom) {
                    tetris_state.rr.bottom = current_brick_pos_y + j;
                    tetris_state.rr.clean = 0;
                }
            }
        }
    }
}

static void clear_figure(void) {
    int i, j;
    int size = tetris_figures[current_figure].size;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (current_brick_pos_x + i < 0 || current_brick_pos_y + j < 0)
                continue;
            if (tetris_figures[current_figure].states[current_state][i][j] == 1) {
                tetris_state.playfield[current_brick_pos_x + i][current_brick_pos_y + j] = TETRIS_FIELD_EMPTY;
                if (current_brick_pos_x + i < tetris_state.rr.left) {
                    tetris_state.rr.left = current_brick_pos_x + i;
                    tetris_state.rr.clean = 0;
                }
                if (current_brick_pos_x + i > tetris_state.rr.right) {
                    tetris_state.rr.right = current_brick_pos_x + i;
                    tetris_state.rr.clean = 0;
                }
                if (current_brick_pos_y + j < tetris_state.rr.top) {
                    tetris_state.rr.top = current_brick_pos_y + j;
                    tetris_state.rr.clean = 0;
                }
                if (current_brick_pos_y + j > tetris_state.rr.bottom) {
                    tetris_state.rr.bottom = current_brick_pos_y + j;
                    tetris_state.rr.clean = 0;
                }
            }
        }
    }
}

static bool is_collide(void) {
    int i, j;
    int size = tetris_figures[current_figure].size;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
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
