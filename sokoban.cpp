#include "brick.h"
#include "cleananimation.h"
#include "winanimation.h"

#include <windows.h>
#include <time.h>

/* Interface static function prototypes */
static void sokoban_init(void);
static brick_state_t* sokoban_get_state(void);
static void sokoban_right_key_press(void);
static void sokoban_left_key_press(void);
static void sokoban_right_key_release(void);
static void sokoban_left_key_release(void);
static void sokoban_up_key_press(void);
static void sokoban_up_key_release(void);
static void sokoban_down_key_press(void);
static void sokoban_down_key_release(void);
static void sokoban_next_figure_accepted(void);
static void sokoban_game_loop(void);

/* Other static function prototypes */
static void sokoban_init_after_cleananimation();
static void sokoban_move_right(void);
static void sokoban_move_left(void);
static void sokoban_move_up(void);
static void sokoban_move_down(void);

/* keyboard states and times */
static char left_key_is_pressed = 0;
static unsigned int left_key_press_start_time = 0;
static unsigned int left_key_repeat = 0;
static char right_key_is_pressed = 0;
static unsigned int right_key_press_start_time = 0;
static char right_key_repeat = 0;
static char up_key_is_pressed = 0;
static unsigned int up_key_press_start_time = 0;
static char up_key_repeat = 0;
static char down_key_is_pressed = 0;
static unsigned int down_key_press_start_time = 0;
static char down_key_repeat = 0;

typedef struct {
    unsigned char right;
    unsigned char left;
    unsigned char up;
    unsigned char down;
} keypress_store_t;

static keypress_store_t keypress_store = { 0, 0, 0, 0 };

/* Game states */
#define SOKOBAN_STATE_NORMAL         0
#define SOKOBAN_STATE_CLEANANIMATION 1

typedef struct {
    char x;
    char y;
    char state;
} sokoban_t;

static sokoban_t sokoban = { SOKOBAN_STATE_NORMAL };

/******************************************************************************
 * Exported functions.
 ******************************************************************************/
void sokoban_init_interface(game_interface_t* iface) {
    iface->game_init = sokoban_init;
    iface->game_right_key_press = sokoban_right_key_press;
    iface->game_left_key_press = sokoban_left_key_press;
    iface->game_right_key_release = sokoban_right_key_release;
    iface->game_left_key_release = sokoban_left_key_release;
    iface->game_up_key_press = sokoban_up_key_press;
    iface->game_up_key_release = sokoban_up_key_release;
    iface->game_down_key_press = sokoban_down_key_press;
    iface->game_down_key_release = sokoban_down_key_release;
    iface->game_next_figure_accepted = sokoban_next_figure_accepted;
    iface->game_loop = sokoban_game_loop;
}

/******************************************************************************
 * Static functions.
 ******************************************************************************/
static brick_state_t* sokoban_get_state(void) {
    return &brick_s;
}

static void sokoban_init(void) {
    sokoban.state = SOKOBAN_STATE_CLEANANIMATION;
    brick_s.level = 1;
    brick_s.score = 0;
    brick_s.lives = 9;
    cleananimation_init();
}

static void sokoban_init_after_cleananimation() {
    int i, j;

    for (i = 0; i < BRICK_PLAYFIELD_WIDTH; i++) {
        for (j = 0; j < BRICK_PLAYFIELD_HEIGHT; j++) {
            brick_s.playfield[i][j] = BRICK_FIELD_EMPTY;
        }
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            brick_s.next[i][j] = BRICK_FIELD_EMPTY;
        }
    }

    brick_s.score = 0;
    brick_s.score_changed = 1;
    brick_s.level = 1;
    brick_s.level_changed = 1;
    brick_s.lives = 0;
    brick_s.lives_changed = 1;
    brick_s.game_over_notification_flag = false;
    brick_s.winning_notification_flag = false;

    brick_s.rr.left = 0;
    brick_s.rr.top = 0;
    brick_s.rr.right = BRICK_PLAYFIELD_WIDTH;
    brick_s.rr.bottom = BRICK_PLAYFIELD_HEIGHT;
    brick_s.rr.clean = 0;

    brick_s.next_changed = true;

    left_key_is_pressed = 0;
    right_key_is_pressed = 0;
    up_key_is_pressed = 0;
    down_key_is_pressed = 0;

    // Place the player in the upper left corner
    sokoban.x = 0;
    sokoban.y = 0;
    brick_s.playfield[sokoban.x][sokoban.y] = BRICK_FIELD_OCCUPIED_ORANGE;
    
    // Place 2 movable boxes somewhere in the middle
    brick_s.playfield[4][4] = BRICK_FIELD_OCCUPIED_INNER;
    brick_s.playfield[4][6] = BRICK_FIELD_OCCUPIED_INNER;

    // Place a peace of wall also somewhere in the middle
    brick_s.playfield[4][10] = BRICK_FIELD_OCCUPIED_OUTER;
    brick_s.playfield[4][11] = BRICK_FIELD_OCCUPIED_OUTER;

    // Place a target block in the bottom right corner
    brick_s.playfield[BRICK_PLAYFIELD_WIDTH - 1][BRICK_PLAYFIELD_HEIGHT - 1] = BRICK_FIELD_OCCUPIED_INNER_SMALL;

    sokoban.state = SOKOBAN_STATE_NORMAL;  
}

static void sokoban_right_key_press(void) {
    if (right_key_is_pressed == 0)
        right_key_press_start_time = GetTickCount();
    right_key_is_pressed = 1;
    keypress_store.right = 1;
}

static void sokoban_left_key_press(void) {
    if (left_key_is_pressed == 0)
        left_key_press_start_time = GetTickCount();
    left_key_is_pressed = 1;
    keypress_store.left = 1;
}

static void sokoban_right_key_release(void) {
    right_key_is_pressed = 0;
    right_key_repeat = 0;
}

static void sokoban_left_key_release(void) {
    left_key_is_pressed = 0;
    left_key_repeat = 0;
}

static void sokoban_up_key_press(void) {
    if (up_key_is_pressed == 0)
        up_key_press_start_time = GetTickCount();
    up_key_is_pressed = 1;
    keypress_store.up = 1;
}

static void sokoban_up_key_release(void) {
    up_key_is_pressed = 0;
    up_key_repeat = 0;
}

static void sokoban_down_key_press(void) {
    if (down_key_is_pressed == 0)
        down_key_press_start_time = GetTickCount();
    down_key_is_pressed = 1;
    keypress_store.down = 1;
}

static void sokoban_down_key_release(void) {
    down_key_is_pressed = 0;
    down_key_repeat = 0;
}

static void sokoban_next_figure_accepted(void) {
    brick_s.next_changed = false;
}

static void sokoban_game_loop(void) {
    if (sokoban.state == SOKOBAN_STATE_CLEANANIMATION) {
        cleananimation();
        if (cleananimation() == CLEANANIMATION_DONE)
            sokoban_init_after_cleananimation();
        return;
    }

    unsigned int t = GetTickCount();

    if (left_key_is_pressed) {
        if (t > left_key_press_start_time + 50 || left_key_repeat == 0) {
            if (left_key_repeat != 1)
                sokoban_move_left();
            left_key_press_start_time = t;
            left_key_repeat++;
            keypress_store.left = 0;
        }
    }
    else if (keypress_store.left) {
        sokoban_move_left();
        keypress_store.left = 0;
    }
    else if (right_key_is_pressed) {
        if (t > right_key_press_start_time + 50 || right_key_repeat == 0) {
            if (right_key_repeat != 1)
                sokoban_move_right();
            right_key_press_start_time = t;
            right_key_repeat++;
            keypress_store.right = 0;
        }
    }
    else if (keypress_store.right) {
        sokoban_move_right();
        keypress_store.right = 0;
    }
    else if (up_key_is_pressed) {
        if (t > up_key_press_start_time + 50 || up_key_repeat == 0) {
            if (up_key_repeat != 1)
                sokoban_move_up();
            up_key_press_start_time = t;
            up_key_repeat++;
            keypress_store.up = 0;
        }
    }
    else if (keypress_store.up) {
        sokoban_move_up();
        keypress_store.up = 0;
    }
    else if (down_key_is_pressed) {
        if (t > down_key_press_start_time + 50 || down_key_repeat == 0) {
            if (down_key_repeat != 1)
                sokoban_move_down();
            down_key_press_start_time = t;
            down_key_repeat++;
            keypress_store.down = 0;
        }
    }
    else if (keypress_store.down) {
        sokoban_move_down();
        keypress_store.down = 0;
    }
}

static void sokoban_move_right(void) {
    if (sokoban.x == BRICK_PLAYFIELD_WIDTH - 1)
        return;

    if (brick_s.playfield[sokoban.x + 1][sokoban.y] == BRICK_FIELD_EMPTY) {
        // Move the player 1 field to the right
        sokoban.x++;
        brick_s.playfield[sokoban.x - 1][sokoban.y] = BRICK_FIELD_EMPTY;
        brick_s.playfield[sokoban.x][sokoban.y] = BRICK_FIELD_OCCUPIED_ORANGE;
        brick_s.rr.left = sokoban.x - 1;
        brick_s.rr.right = sokoban.x;
        brick_s.rr.top = brick_s.rr.bottom = sokoban.y;
        brick_s.rr.clean = 0;
        return;
    }

    if (sokoban.x == BRICK_PLAYFIELD_WIDTH - 2)
        return;

    if (brick_s.playfield[sokoban.x + 1][sokoban.y] == BRICK_FIELD_OCCUPIED_INNER &&
        brick_s.playfield[sokoban.x + 2][sokoban.y] == BRICK_FIELD_EMPTY) {
        // Push the movable box 1 field to the right
        brick_s.playfield[sokoban.x + 2][sokoban.y] = BRICK_FIELD_OCCUPIED_INNER;
        brick_s.playfield[sokoban.x + 1][sokoban.y] = BRICK_FIELD_OCCUPIED_ORANGE;
        brick_s.playfield[sokoban.x][sokoban.y] = BRICK_FIELD_EMPTY;
        sokoban.x++;
        brick_s.rr.left = sokoban.x - 1;
        brick_s.rr.right = sokoban.x + 1;
        brick_s.rr.top = brick_s.rr.bottom = sokoban.y;
        brick_s.rr.clean = 0;
        return;
    }
}

static void sokoban_move_left(void) {
    if (sokoban.x == 0)
        return;

    if (brick_s.playfield[sokoban.x - 1][sokoban.y] == BRICK_FIELD_EMPTY) {
        // Move the player 1 field to the left
        sokoban.x--;
        brick_s.playfield[sokoban.x + 1][sokoban.y] = BRICK_FIELD_EMPTY;
        brick_s.playfield[sokoban.x][sokoban.y] = BRICK_FIELD_OCCUPIED_ORANGE;
        brick_s.rr.left = sokoban.x;
        brick_s.rr.right = sokoban.x + 1;
        brick_s.rr.top = brick_s.rr.bottom = sokoban.y;
        brick_s.rr.clean = 0;
        return;
    }

    if (sokoban.x == 1)
        return;

    if (brick_s.playfield[sokoban.x - 1][sokoban.y] == BRICK_FIELD_OCCUPIED_INNER &&
        brick_s.playfield[sokoban.x - 2][sokoban.y] == BRICK_FIELD_EMPTY) {
        // Push the movable box 1 field to the left
        brick_s.playfield[sokoban.x - 2][sokoban.y] = BRICK_FIELD_OCCUPIED_INNER;
        brick_s.playfield[sokoban.x - 1][sokoban.y] = BRICK_FIELD_OCCUPIED_ORANGE;
        brick_s.playfield[sokoban.x][sokoban.y] = BRICK_FIELD_EMPTY;
        sokoban.x--;
        brick_s.rr.left = sokoban.x - 1;
        brick_s.rr.right = sokoban.x + 1;
        brick_s.rr.top = brick_s.rr.bottom = sokoban.y;
        brick_s.rr.clean = 0;
        return;
    }
}

static void sokoban_move_up(void) {
    if (sokoban.y == 0)
        return;

    if (brick_s.playfield[sokoban.x][sokoban.y - 1] == BRICK_FIELD_EMPTY) {
        // Move the player 1 field up
        sokoban.y--;
        brick_s.playfield[sokoban.x][sokoban.y + 1] = BRICK_FIELD_EMPTY;
        brick_s.playfield[sokoban.x][sokoban.y] = BRICK_FIELD_OCCUPIED_ORANGE;
        brick_s.rr.top = sokoban.y;
        brick_s.rr.bottom = sokoban.y + 1;
        brick_s.rr.right = brick_s.rr.left = sokoban.x;
        brick_s.rr.clean = 0;
        return;
    }

    if (sokoban.y == 1)
        return;

    if (brick_s.playfield[sokoban.x][sokoban.y - 1] == BRICK_FIELD_OCCUPIED_INNER &&
        brick_s.playfield[sokoban.x][sokoban.y - 2] == BRICK_FIELD_EMPTY) {
        // Push the movable box 1 field up
        brick_s.playfield[sokoban.x][sokoban.y - 2] = BRICK_FIELD_OCCUPIED_INNER;
        brick_s.playfield[sokoban.x][sokoban.y - 1] = BRICK_FIELD_OCCUPIED_ORANGE;
        brick_s.playfield[sokoban.x][sokoban.y] = BRICK_FIELD_EMPTY;
        sokoban.y--;
        brick_s.rr.top = sokoban.y - 1;
        brick_s.rr.bottom = sokoban.y + 1;
        brick_s.rr.right = brick_s.rr.left = sokoban.x;
        brick_s.rr.clean = 0;
        return;
    }
}

static void sokoban_move_down(void) {
    if (sokoban.y == BRICK_PLAYFIELD_HEIGHT - 1)
        return;

    if (brick_s.playfield[sokoban.x][sokoban.y + 1] == BRICK_FIELD_EMPTY) {
        // Move the player 1 field down
        sokoban.y++;
        brick_s.playfield[sokoban.x][sokoban.y - 1] = BRICK_FIELD_EMPTY;
        brick_s.playfield[sokoban.x][sokoban.y] = BRICK_FIELD_OCCUPIED_ORANGE;
        brick_s.rr.top = sokoban.y - 1;
        brick_s.rr.bottom = sokoban.y;
        brick_s.rr.right = brick_s.rr.left = sokoban.x;
        brick_s.rr.clean = 0;
        return;
    }

    if (sokoban.y == BRICK_PLAYFIELD_HEIGHT - 2)
        return;

    if (brick_s.playfield[sokoban.x][sokoban.y + 1] == BRICK_FIELD_OCCUPIED_INNER &&
        brick_s.playfield[sokoban.x][sokoban.y + 2] == BRICK_FIELD_EMPTY) {
        // Push the movable box 1 field down
        brick_s.playfield[sokoban.x][sokoban.y + 2] = BRICK_FIELD_OCCUPIED_INNER;
        brick_s.playfield[sokoban.x][sokoban.y + 1] = BRICK_FIELD_OCCUPIED_ORANGE;
        brick_s.playfield[sokoban.x][sokoban.y] = BRICK_FIELD_EMPTY;
        sokoban.y++;
        brick_s.rr.top = sokoban.y - 1;
        brick_s.rr.bottom = sokoban.y + 1;
        brick_s.rr.right = brick_s.rr.left = sokoban.x;
        brick_s.rr.clean = 0;
        return;
    }
}
