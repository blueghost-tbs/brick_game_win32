#include "brick.h"

/* Interface static function prototypes */
static void snake_init(void);
static brick_state_t *snake_get_state(void);
static void snake_right_key_press(void);
static void snake_left_key_press(void);
static void snake_right_key_release(void);
static void snake_left_key_release(void);
static void snake_up_key_press(void);
static void snake_up_key_release(void);
static void snake_down_key_press(void);
static void snake_down_key_release(void);
static void snake_next_figure_accepted(void);
static void snake_game_loop(void);

static brick_state_t snake_state;

/******************************************************************************
 * Exported functions.
 ******************************************************************************/
void snake_init_interface(game_interface_t *iface) {
    iface->game_get_state = snake_get_state;
    iface->game_init = snake_init;
    iface->game_right_key_press = snake_right_key_press;
    iface->game_left_key_press = snake_left_key_press;
    iface->game_right_key_release = snake_right_key_release;
    iface->game_left_key_release = snake_left_key_release;
    iface->game_up_key_press = snake_up_key_press;
    iface->game_up_key_release = snake_up_key_release;
    iface->game_down_key_press = snake_down_key_press;
    iface->game_down_key_release = snake_down_key_release;
    iface->game_next_figure_accepted = snake_next_figure_accepted;
    iface->game_loop = snake_game_loop;
}

/******************************************************************************
 * Static functions.
 ******************************************************************************/
static brick_state_t *snake_get_state(void) {
    return &snake_state;
}

static void snake_init(void) {
    int i, j;

    for (i = 0; i < BRICK_PLAYFIELD_WIDTH; i++) {
        for (j = 0; j < BRICK_PLAYFIELD_HEIGHT; j++) {
            snake_state.playfield[i][j] = BRICK_FIELD_EMPTY;
        }
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            snake_state.next[i][j] = BRICK_FIELD_EMPTY;
        }
    }

    snake_state.score = 0;
    snake_state.score_changed = 1;
    snake_state.level = 1;
    snake_state.level_changed = 1;
    snake_state.lines_since_level_increase = 0;
    snake_state.game_over_notification_flag = false;

    snake_state.rr.left = 0;
    snake_state.rr.top = 0;
    snake_state.rr.right = BRICK_PLAYFIELD_WIDTH;
    snake_state.rr.bottom = BRICK_PLAYFIELD_HEIGHT;
    snake_state.rr.clean = 0;

    snake_state.next_changed = true;
}

static void snake_right_key_press(void) {
}

static void snake_left_key_press(void) {
}

static void snake_right_key_release(void) {
}

static void snake_left_key_release(void) {
}

static void snake_up_key_press(void) {
}

static void snake_up_key_release(void) {
}

static void snake_down_key_press(void) {
}

static void snake_down_key_release(void) {
}

static void snake_next_figure_accepted(void) {
}

static void snake_game_loop(void) {
}

static void snake_new_game(void) {
}
