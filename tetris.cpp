#include "tetris.h"
#include "tetris_figures.h"

#include <time.h>
#include <stdlib.h>
#include <windows.h>

static int  tetris_get_next_figure(void);
static void draw_figure(char need_redraw);
static void clear_figure(void);
static bool is_collide(void);
static void rotate_right(void);
static void rotate_left(void);
static void tetris_move_right(void);
static void tetris_move_left(void);
static void tetris_up_key(void);
static int search_full_line(void);
static void clean_line(int line);

static tetris_state_t tetris_state;
static int current_brick_pos_x = 5;
static int current_brick_pos_y = -1;
static unsigned int current_figure = 0;
static unsigned int current_state = 0;
static unsigned int next_figure = 1;
static unsigned int next_state = 1;

/* keyboard states and times */
static char left_key_is_pressed = 0;
static unsigned int left_key_press_start_time = 0;
static unsigned int left_key_repeat = 0;
static char right_key_is_pressed = 0;
static unsigned int right_key_press_start_time = 0;
static char right_key_repeat = 0;
static char up_key_is_pressed = 0;
static char down_key_is_pressed = 0;
static unsigned int down_key_press_start_time = 0;
static char ignore_down_key = 0;

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
    srand(time(NULL));

    next_figure = rand() % TETRIS_FIGURES_NUM;
    next_state = rand() % tetris_figures[next_figure]->states_num;
    current_brick_pos_y = tetris_get_next_figure() - 1;
}

tetris_state_t *tetris_get_state(void) {
    return &tetris_state;
}

void tetris_tick(void) {
    char it_was_clean = tetris_state.rr.clean = 1;
    int line = -1;

    clear_figure();
    current_brick_pos_y++;
    if (is_collide()) {
        current_brick_pos_y--;
        if (it_was_clean)
            tetris_reset_redraw_rectangle();
        draw_figure(0);

        do {
            line = search_full_line();
            if (line > 0)
                clean_line(line);
        } while (line > 0);

        current_brick_pos_y = tetris_get_next_figure();
        current_brick_pos_x = 5;
        if (down_key_is_pressed)
            ignore_down_key = 1;
    }

    draw_figure(1);
}

void tetris_right_key_press(void) {
    if (right_key_is_pressed == 0)
        right_key_press_start_time = GetTickCount();
    right_key_is_pressed = 1;
}

void tetris_left_key_press(void) {
    if (left_key_is_pressed == 0)
        left_key_press_start_time = GetTickCount();
    left_key_is_pressed = 1;
}

void tetris_right_key_release(void) {
    right_key_is_pressed = 0;
    right_key_repeat = 0;
}

void tetris_left_key_release(void) {
    left_key_is_pressed = 0;
    left_key_repeat = 0;
}

void tetris_up_key_press(void) {
    if (up_key_is_pressed == 0) {
        tetris_up_key();
    }
    up_key_is_pressed = 1;
}

void tetris_up_key_release(void) {
    up_key_is_pressed = 0;
}

void tetris_down_key_press(void) {
    if (down_key_is_pressed == 0)
        down_key_press_start_time = GetTickCount();
    down_key_is_pressed = 1;
}

void tetris_down_key_release(void) {
    down_key_is_pressed = 0;
    ignore_down_key = 0;
}

void tetris_reset_redraw_rectangle(void) {
    tetris_state.rr.left = TETRIS_PLAYFIELD_WIDTH;
    tetris_state.rr.top = TETRIS_PLAYFIELD_HEIGHT;
    tetris_state.rr.right = 0;
    tetris_state.rr.bottom = 0;
    tetris_state.rr.clean = 1;
}

void tetris_game_loop(void) {
    unsigned int t = GetTickCount();

    if (left_key_is_pressed) {
        if (t > left_key_press_start_time + 50 || left_key_repeat == 0) {
            if (left_key_repeat != 1)
                tetris_move_left();
            left_key_press_start_time = t;
            left_key_repeat++;
        }
    } else if (right_key_is_pressed) {
        if (t > right_key_press_start_time + 50 || right_key_repeat == 0) {
            if (right_key_repeat != 1)
                tetris_move_right();
            right_key_press_start_time = t;
            right_key_repeat++;
        }
    }
    if (down_key_is_pressed && !ignore_down_key) {
        if (t > down_key_press_start_time + 25) {
            tetris_tick();
            down_key_press_start_time = t;
        }
    }
}

/******************************************************************************
 * Static functions.
 ******************************************************************************/
static int tetris_get_next_figure(void) {
    int i, j;
    current_figure = next_figure;
    current_state = next_state;

    do {
        next_figure = rand() % TETRIS_FIGURES_NUM;
    } while (next_figure == current_figure);

    next_state = rand() % tetris_figures[next_figure]->states_num;

    for (i = tetris_figures[current_figure]->size; i > 0; i--) {
        for (j = 0; j < tetris_figures[current_figure]->size; j++) {
            if (tetris_figures[current_figure]->states[current_state][j][i] == 1)
                goto exit;
        }
    }

exit:
    return -i;
}

static void draw_figure(char need_redraw) {
    int i, j;
    int size = tetris_figures[current_figure]->size;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (current_brick_pos_x + i < 0 || current_brick_pos_y + j < 0)
                continue;
            if (tetris_figures[current_figure]->states[current_state][i][j] == 1) {
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
    int size = tetris_figures[current_figure]->size;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (current_brick_pos_x + i < 0 || current_brick_pos_y + j < 0)
                continue;
            if (tetris_figures[current_figure]->states[current_state][i][j] == 1) {
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
    int size = tetris_figures[current_figure]->size;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (tetris_figures[current_figure]->states[current_state][i][j] == 1) {
                if (current_brick_pos_x + i < 0)
                    return true;
                if (current_brick_pos_x + i >= TETRIS_PLAYFIELD_WIDTH)
                    return true;
                if (current_brick_pos_y + j < 0)
                    continue;
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
    current_state = (current_state + 1) % tetris_figures[current_figure]->states_num;
}

static void rotate_left(void) {
    if (current_state == 0)
        current_state = tetris_figures[current_figure]->states_num - 1;
    else
        --current_state;
}

static void tetris_move_right(void) {
    char it_was_clean = tetris_state.rr.clean = 1;

    clear_figure();
    current_brick_pos_x++;
    if (is_collide()) {
        current_brick_pos_x--;
        if (it_was_clean)
            tetris_reset_redraw_rectangle();
        draw_figure(0);
    } else {
        draw_figure(1);
    }
}

static void tetris_move_left(void) {
    char it_was_clean = tetris_state.rr.clean = 1;

    clear_figure();
    current_brick_pos_x--;
    if (is_collide()) {
        current_brick_pos_x++;
        if (it_was_clean)
            tetris_reset_redraw_rectangle();
        draw_figure(0);
    } else {
        draw_figure(1);
    }
}

static void tetris_up_key(void) {
    if (tetris_figures[current_figure]->states_num == 1)
        return;

    char it_was_clean = tetris_state.rr.clean = 1;

    clear_figure();
    rotate_right();
    if (is_collide()) {
        rotate_left();
        if (it_was_clean)
            tetris_reset_redraw_rectangle();
        draw_figure(0);
    } else {
        draw_figure(1);
    }
}

static int search_full_line(void) {
    int i, j;

    for (i = TETRIS_PLAYFIELD_HEIGHT - 1; i >= 0; i--) {
        for (j = 0; j < TETRIS_PLAYFIELD_WIDTH; j++) {
            if (tetris_state.playfield[j][i] == TETRIS_FIELD_EMPTY)
                break;
        }
        if (j == TETRIS_PLAYFIELD_WIDTH) {
            // Full line found!
            return i;
        }
    }

    return -1;
}

static void clean_line(int line) {
    int i, j, empty;

    // Move every not empty line down by 1.
    for (i = line - 1; i >= 0; i--) {
        empty = 1;
        for (j = 0; j < TETRIS_PLAYFIELD_WIDTH; j++) {
            if (tetris_state.playfield[j][i] != TETRIS_FIELD_EMPTY)
                empty = 0;
            tetris_state.playfield[j][i + 1] = tetris_state.playfield[j][i];
        }
        if (empty)
            break;
    }

    if (i == -1) {
        // Clean the top line.
        for (j = 0; j < TETRIS_PLAYFIELD_WIDTH; j++)
            tetris_state.playfield[j][0] = TETRIS_FIELD_EMPTY;
    }

    tetris_state.rr.right = TETRIS_PLAYFIELD_WIDTH - 1;
    tetris_state.rr.left = 0;

    if (i + 1 < tetris_state.rr.top) {
        tetris_state.rr.top = i + 1;
        tetris_state.rr.clean = 0;
    }
    if (line > tetris_state.rr.bottom) {
        tetris_state.rr.bottom = line;
        tetris_state.rr.clean = 0;
    }
}
