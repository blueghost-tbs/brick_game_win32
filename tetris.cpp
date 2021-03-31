#include "brick.h"
#include "tetrominos.h"
#include "cleananimation.h"

#include <time.h>
#include <stdlib.h>
#include <windows.h>

/* Interface static function prototypes */
static void tetris_init(void);
static brick_state_t *tetris_get_state(void);
static void tetris_right_key_press(void);
static void tetris_left_key_press(void);
static void tetris_right_key_release(void);
static void tetris_left_key_release(void);
static void tetris_up_key_press(void);
static void tetris_up_key_release(void);
static void tetris_down_key_press(void);
static void tetris_down_key_release(void);
static void tetris_next_figure_accepted(void);
static void tetris_game_loop(void);

/* Other static function prototypes */
static void tetris_init_after_cleananimation(void);
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
static void move_down_lines(int line);
static void tetris_tick(void);
static void notify_next_figure();

static brick_state_t brick_t;
static int current_brick_pos_x = 5;
static int current_brick_pos_y = -1;
static unsigned int current_figure = 0;
static unsigned int current_state = 0;
static unsigned int next_figure = TETROMINOS_NUM;
static unsigned int next_state = 1;
static unsigned int gravity_timer = 0;

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

/* Game states */
#define TS_NORMAL                0
#define TS_LINEDELETED_ANIMATION 1
#define TS_BLOCKSMOVED_ANIMATION 2
#define TS_NEXTFGAFTER_ANIMATION 3
#define TS_GAMEOVER              4
#define TS_CLEANANIMATION        5

static struct {
    char state;
    char line;
    char cleared_lines;
    char lines_since_level_increase;
} ts = {TS_NORMAL, 0, 0};

/*
 * 1 line  - 100 points
 * 2 lines - 300 points
 * 3 lines - 500 points
 * 4 lines - 800 points
 */

#define LEVEL_SPEEDS_NUM 11
const static int level_speed[LEVEL_SPEEDS_NUM] = {
    1000,
    800,
    640,
    512,
    410,
    325,
    250,
    200,
    166,
    133,
    110
};

/******************************************************************************
 * Exported functions.
 ******************************************************************************/
void tetris_init_interface(game_interface_t *iface) {
    iface->game_get_state = tetris_get_state;
    iface->game_init = tetris_init;
    iface->game_right_key_press = tetris_right_key_press;
    iface->game_left_key_press = tetris_left_key_press;
    iface->game_right_key_release = tetris_right_key_release;
    iface->game_left_key_release = tetris_left_key_release;
    iface->game_up_key_press = tetris_up_key_press;
    iface->game_up_key_release = tetris_up_key_release;
    iface->game_down_key_press = tetris_down_key_press;
    iface->game_down_key_release = tetris_down_key_release;
    iface->game_next_figure_accepted = tetris_next_figure_accepted;
    iface->game_loop = tetris_game_loop;
}

/******************************************************************************
 * Static functions.
 ******************************************************************************/
static void tetris_init(void) {
    ts.state = TS_CLEANANIMATION;
    cleananimation_init();
}

static void tetris_init_after_cleananimation(void) {
    int i, j;

    for (i = 0; i < BRICK_PLAYFIELD_WIDTH; i++) {
        for (j = 0; j < BRICK_PLAYFIELD_HEIGHT; j++) {
            brick_t.playfield[i][j] = BRICK_FIELD_EMPTY;
        }
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            brick_t.next[i][j] = BRICK_FIELD_EMPTY;
        }
    }

    brick_t.score = 0;
    brick_t.score_changed = 1;
    brick_t.level = 1;
    brick_t.level_changed = 1;
    brick_t.lives = 0;
    brick_t.lives_changed = 1;
    brick_t.game_over_notification_flag = false;
    brick_t.winning_notification_flag = false;

    brick_t.rr.left = 0;
    brick_t.rr.top = 0;
    brick_t.rr.right = BRICK_PLAYFIELD_WIDTH;
    brick_t.rr.bottom = BRICK_PLAYFIELD_HEIGHT;
    brick_t.rr.clean = 0;

    brick_t.next_changed = true;
    srand(time(NULL));

    left_key_is_pressed = 0;
    right_key_is_pressed = 0;
    up_key_is_pressed = 0;
    down_key_is_pressed = 0;

    current_brick_pos_x = 5;
    current_brick_pos_y = tetris_get_next_figure() - 1;
    ts.state = TS_NORMAL;
    ts.lines_since_level_increase = 0;
}

static brick_state_t *tetris_get_state(void) {
    return &brick_t;
}

static void tetris_right_key_press(void) {
    if (ts.state == TS_GAMEOVER)
        return;
    if (right_key_is_pressed == 0)
        right_key_press_start_time = GetTickCount();
    right_key_is_pressed = 1;
}

static void tetris_left_key_press(void) {
    if (ts.state == TS_GAMEOVER)
        return;
    if (left_key_is_pressed == 0)
        left_key_press_start_time = GetTickCount();
    left_key_is_pressed = 1;
}

static void tetris_right_key_release(void) {
    if (ts.state == TS_GAMEOVER)
        return;
    right_key_is_pressed = 0;
    right_key_repeat = 0;
}

static void tetris_left_key_release(void) {
    if (ts.state == TS_GAMEOVER)
        return;
    left_key_is_pressed = 0;
    left_key_repeat = 0;
}

static void tetris_up_key_press(void) {
    if (ts.state == TS_GAMEOVER)
        return;
    if (up_key_is_pressed == 0) {
        tetris_up_key();
    }
    up_key_is_pressed = 1;
}

static void tetris_up_key_release(void) {
    if (ts.state == TS_GAMEOVER)
        return;
    up_key_is_pressed = 0;
}

static void tetris_down_key_press(void) {
    if (ts.state == TS_GAMEOVER)
        return;
    if (down_key_is_pressed == 0)
        down_key_press_start_time = GetTickCount();
    down_key_is_pressed = 1;
}

static void tetris_down_key_release(void) {
    if (ts.state == TS_GAMEOVER)
        return;
    down_key_is_pressed = 0;
    ignore_down_key = 0;
}

static void tetris_next_figure_accepted(void) {
    brick_t.next_changed = false;
}

static void tetris_game_loop(void) {
    if (ts.state == TS_CLEANANIMATION) {
        cleananimation(&brick_t);
        if (cleananimation(&brick_t) == CLEANANIMATION_DONE)
            tetris_init_after_cleananimation();
        return;
    }

    if (ts.state == TS_GAMEOVER) {
        return;
    } else if (ts.state == TS_LINEDELETED_ANIMATION) {
        move_down_lines(ts.line);
        ts.state = TS_BLOCKSMOVED_ANIMATION;
        return;
    } else if (ts.state == TS_BLOCKSMOVED_ANIMATION) {
        int line = search_full_line();

        if (line > 0) {
            clean_line(line);
            ts.state = TS_LINEDELETED_ANIMATION;
            ts.line = line;
            return;
        }

        switch (ts.cleared_lines) {
            case 1:
            default:
                brick_t.score += 100 * brick_t.level;
                break;
            case 2:
                brick_t.score += 300 * brick_t.level;
                break;
            case 3:
                brick_t.score += 500 * brick_t.level;
                break;
            case 4:
                brick_t.score += 800 * brick_t.level;
                break;
        }
        brick_t.score_changed = 1;
        ts.lines_since_level_increase += ts.cleared_lines;

        if (ts.lines_since_level_increase >= 10) {
            ts.lines_since_level_increase = 0;
            brick_t.level++;
            brick_t.level_changed = 1;
        }

        ts.cleared_lines = 0;
        ts.state = TS_NEXTFGAFTER_ANIMATION;
        return;
    }

    unsigned int t = GetTickCount();
    int speed = 1000;
    if (brick_t.level < 1)
        speed = level_speed[0];
    else if (brick_t.level > LEVEL_SPEEDS_NUM)
        speed = level_speed[LEVEL_SPEEDS_NUM - 1];
    else
        speed = level_speed[brick_t.level - 1];

    if (t > gravity_timer + speed) {
        tetris_tick();
    }

    while (t > gravity_timer + speed) {
        gravity_timer += speed;
    }

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

static void tetris_tick(void) {
    if (ts.state == TS_NEXTFGAFTER_ANIMATION) {
        current_brick_pos_y = tetris_get_next_figure();
        current_brick_pos_x = 5;
        if (down_key_is_pressed)
            ignore_down_key = 1;
        draw_figure(1);
        ts.state = TS_NORMAL;
        return;
    }

    if (ts.state != TS_NORMAL)
        return;

    char it_was_clean = brick_t.rr.clean;
    int line = -1;

    clear_figure();
    current_brick_pos_y++;
    if (is_collide()) {
        current_brick_pos_y--;
        if (it_was_clean)
            reset_redraw_rectangle(&brick_t.rr);
        draw_figure(0);

        if (current_brick_pos_y < 0) {
            draw_figure(1);
            ts.state = TS_GAMEOVER;
            brick_t.game_over_notification_flag = true;
            return;
        }

        line = search_full_line();
        if (line > 0) {
            ts.cleared_lines = 0;
            clean_line(line);
            ts.state = TS_LINEDELETED_ANIMATION;
            ts.line = line;
            return;
        }

        current_brick_pos_y = tetris_get_next_figure();
        current_brick_pos_x = 5;
        if (down_key_is_pressed)
            ignore_down_key = 1;
    }

    draw_figure(1);
}

static int tetris_get_next_figure(void) {
    static char bag[TETROMINOS_NUM] = {0,};
    static char bag_remaining = 0;
    int i, j;

    if (bag_remaining == 0) {
        // refill bag
        for (i = 0; i < TETROMINOS_NUM; i++) {
            bag[i] = i;
        }

        bag_remaining = 7;
    }

    if (next_figure == TETROMINOS_NUM) {
        // first call!
        // we have to pull from the bag twice
        do {
            i = rand() % TETROMINOS_NUM;
        } while (bag[i] == -1);

        next_figure = bag[i];
        bag[i] = -1;
        next_state = rand() % tetrominos[next_figure]->states_num;
        bag_remaining--;
    }

    current_figure = next_figure;
    current_state = next_state;

    // pull next tetromino from the bag
    do {
        i = rand() % TETROMINOS_NUM;
    } while (bag[i] == -1);

    next_figure = bag[i];
    bag[i] = -1;
    bag_remaining--;

    next_state = rand() % tetrominos[next_figure]->states_num;
    notify_next_figure();

    for (i = tetrominos[current_figure]->size; i > 0; i--) {
        for (j = 0; j < tetrominos[current_figure]->size; j++) {
            if (tetrominos[current_figure]->states[current_state][j][i] > 0)
                goto exit;
        }
    }

exit:
    return -i;
}

static void draw_figure(char need_redraw) {
    int i, j;
    int size = tetrominos[current_figure]->size;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (current_brick_pos_x + i < 0 || current_brick_pos_y + j < 0)
                continue;
            if (tetrominos[current_figure]->states[current_state][i][j] > 0) {
                brick_t.playfield[current_brick_pos_x + i][current_brick_pos_y + j] = tetrominos[current_figure]->states[current_state][i][j];
                if (!need_redraw)
                    continue;
                if (current_brick_pos_x + i < brick_t.rr.left) {
                    brick_t.rr.left = current_brick_pos_x + i;
                    brick_t.rr.clean = 0;
                }
                if (current_brick_pos_x + i > brick_t.rr.right) {
                    brick_t.rr.right = current_brick_pos_x + i;
                    brick_t.rr.clean = 0;
                }
                if (current_brick_pos_y + j < brick_t.rr.top) {
                    brick_t.rr.top = current_brick_pos_y + j;
                    brick_t.rr.clean = 0;
                }
                if (current_brick_pos_y + j > brick_t.rr.bottom) {
                    brick_t.rr.bottom = current_brick_pos_y + j;
                    brick_t.rr.clean = 0;
                }
            }
        }
    }
}

static void clear_figure(void) {
    int i, j;
    int size = tetrominos[current_figure]->size;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (current_brick_pos_x + i < 0 || current_brick_pos_y + j < 0)
                continue;
            if (tetrominos[current_figure]->states[current_state][i][j] > 0) {
                brick_t.playfield[current_brick_pos_x + i][current_brick_pos_y + j] = BRICK_FIELD_EMPTY;
                if (current_brick_pos_x + i < brick_t.rr.left) {
                    brick_t.rr.left = current_brick_pos_x + i;
                    brick_t.rr.clean = 0;
                }
                if (current_brick_pos_x + i > brick_t.rr.right) {
                    brick_t.rr.right = current_brick_pos_x + i;
                    brick_t.rr.clean = 0;
                }
                if (current_brick_pos_y + j < brick_t.rr.top) {
                    brick_t.rr.top = current_brick_pos_y + j;
                    brick_t.rr.clean = 0;
                }
                if (current_brick_pos_y + j > brick_t.rr.bottom) {
                    brick_t.rr.bottom = current_brick_pos_y + j;
                    brick_t.rr.clean = 0;
                }
            }
        }
    }
}

static bool is_collide(void) {
    int i, j;
    int size = tetrominos[current_figure]->size;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (tetrominos[current_figure]->states[current_state][i][j] > 0) {
                if (current_brick_pos_x + i < 0)
                    return true;
                if (current_brick_pos_x + i >= BRICK_PLAYFIELD_WIDTH)
                    return true;
                if (current_brick_pos_y + j < 0)
                    continue;
                if (current_brick_pos_y + j >= BRICK_PLAYFIELD_HEIGHT)
                    return true;
                if (brick_t.playfield[current_brick_pos_x + i][current_brick_pos_y + j] > 0)
                    return true;
            }
        }
    }

    return false;
}

static void rotate_right(void) {
    current_state = (current_state + 1) % tetrominos[current_figure]->states_num;
}

static void rotate_left(void) {
    if (current_state == 0)
        current_state = tetrominos[current_figure]->states_num - 1;
    else
        --current_state;
}

static void tetris_move_right(void) {
    char it_was_clean = brick_t.rr.clean;

    clear_figure();
    current_brick_pos_x++;
    if (is_collide()) {
        current_brick_pos_x--;
        if (it_was_clean)
            reset_redraw_rectangle(&brick_t.rr);
        draw_figure(0);
    } else {
        draw_figure(1);
    }
}

static void tetris_move_left(void) {
    char it_was_clean = brick_t.rr.clean;

    clear_figure();
    current_brick_pos_x--;
    if (is_collide()) {
        current_brick_pos_x++;
        if (it_was_clean)
            reset_redraw_rectangle(&brick_t.rr);
        draw_figure(0);
    } else {
        draw_figure(1);
    }
}

static void tetris_up_key(void) {
    if (tetrominos[current_figure]->states_num == 1)
        return;

    char it_was_clean = brick_t.rr.clean;

    clear_figure();
    rotate_right();
    if (is_collide()) {
        rotate_left();
        if (it_was_clean)
            reset_redraw_rectangle(&brick_t.rr);
        draw_figure(0);
    } else {
        draw_figure(1);
    }
}

static int search_full_line(void) {
    int i, j;

    for (i = BRICK_PLAYFIELD_HEIGHT - 1; i >= 0; i--) {
        for (j = 0; j < BRICK_PLAYFIELD_WIDTH; j++) {
            if (brick_t.playfield[j][i] == BRICK_FIELD_EMPTY)
                break;
        }
        if (j == BRICK_PLAYFIELD_WIDTH) {
            // Full line found!
            return i;
        }
    }

    return -1;
}

static void clean_line(int line) {
    int i;

    for (i = 0; i < BRICK_PLAYFIELD_WIDTH; i++) {
        brick_t.playfield[i][line] = BRICK_FIELD_EMPTY;
    }

    if (brick_t.rr.right < BRICK_PLAYFIELD_WIDTH - 1 ||
        brick_t.rr.left > 0) {
        brick_t.rr.clean = 0;
    }

    brick_t.rr.right = BRICK_PLAYFIELD_WIDTH - 1;
    brick_t.rr.left = 0;

    if (line < brick_t.rr.top) {
        brick_t.rr.top = line;
        brick_t.rr.clean = 0;
    }

    if (line > brick_t.rr.bottom) {
        brick_t.rr.bottom = line;
        brick_t.rr.clean = 0;
    }

    ts.cleared_lines++;
}

static void move_down_lines(int line) {
    int i, j, empty;

    // Move every not empty line down by 1.
    for (i = line - 1; i >= 0; i--) {
        empty = 1;
        for (j = 0; j < BRICK_PLAYFIELD_WIDTH; j++) {
            if (brick_t.playfield[j][i] != BRICK_FIELD_EMPTY)
                empty = 0;
            brick_t.playfield[j][i + 1] = brick_t.playfield[j][i];
        }
        if (empty)
            break;
    }

    if (i == -1) {
        // Clean the top line.
        for (j = 0; j < BRICK_PLAYFIELD_WIDTH; j++)
            brick_t.playfield[j][0] = BRICK_FIELD_EMPTY;
    }

    if (brick_t.rr.right < BRICK_PLAYFIELD_WIDTH - 1 ||
        brick_t.rr.left > 0) {
        brick_t.rr.clean = 0;
    }

    brick_t.rr.right = BRICK_PLAYFIELD_WIDTH - 1;
    brick_t.rr.left = 0;

    if (i + 1 < brick_t.rr.top) {
        brick_t.rr.top = i + 1;
        brick_t.rr.clean = 0;
    }

    if (line > brick_t.rr.bottom) {
        brick_t.rr.bottom = line;
        brick_t.rr.clean = 0;
    }
}

static void notify_next_figure(void) {
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            brick_t.next[i][j] = 0;
        }
    }

    for (i = 0; i < tetrominos[next_figure]->size; i++) {
        for (j = 0; j < tetrominos[next_figure]->size; j++) {
            brick_t.next[i][j] = tetrominos[next_figure]->states[next_state][i][j];
        }
    }

    brick_t.next_changed = true;
}
