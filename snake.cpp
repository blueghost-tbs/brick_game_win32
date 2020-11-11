#include "brick.h"

#include <windows.h>

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

/* Other static function prototypes */
static void snake_tick(void);
static void add_block_to_redraw_rectangle(char x, char y);

static brick_state_t snake_state;

#define SNAKE_MOVE_DOWN      0
#define SNAKE_MOVE_UP        1
#define SNAKE_MOVE_RIGHT     2
#define SNAKE_MOVE_LEFT      3

#define SNAKE_MAX_SEGMENTS 100  
#define SNAKE_SPEED        200

#define SNAKE_STATE_NORMAL   0
#define SNAKE_STATE_GAMEOVER 1

typedef struct {
    char x;
    char y;
} segment_t;

typedef struct {
    segment_t segments[SNAKE_MAX_SEGMENTS];
    short length;
    short head;
    short tail;
    char move;
    char turn_in_progress;
    char state;
} snake_t;

static snake_t snake = {{{5, 5}, {5, 6}, {5, 7}}, 3, 1, 0, SNAKE_MOVE_DOWN, SNAKE_STATE_NORMAL};
static unsigned int snake_timer = 0;

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

    snake.length = 3;
    snake.head = 2;
    snake.tail = 0;
    snake.segments[0].x = 5; snake.segments[0].y = 5;
    snake.segments[1].x = 5; snake.segments[1].y = 6;
    snake.segments[2].x = 5; snake.segments[2].y = 7;
    snake.move = SNAKE_MOVE_DOWN;
    snake.state = SNAKE_STATE_NORMAL;

    for (i = 0; i < snake.length; i++) {
        snake_state.playfield[snake.segments[i].x][snake.segments[i].y] = BRICK_FIELD_OCCUPIED;
    }
}

static void snake_right_key_press(void) {
    if (snake.move != SNAKE_MOVE_LEFT && !snake.turn_in_progress) {
        snake.move = SNAKE_MOVE_RIGHT;
        snake.turn_in_progress = true;
    }
}

static void snake_left_key_press(void) {
    if (snake.move != SNAKE_MOVE_RIGHT && !snake.turn_in_progress) {
        snake.move = SNAKE_MOVE_LEFT;
        snake.turn_in_progress = true;
    }
}

static void snake_right_key_release(void) {
}

static void snake_left_key_release(void) {
}

static void snake_up_key_press(void) {
}

static void snake_up_key_release(void) {
    if (snake.move != SNAKE_MOVE_DOWN && !snake.turn_in_progress) {
        snake.move = SNAKE_MOVE_UP;
        snake.turn_in_progress = true;
    }
}

static void snake_down_key_press(void) {
    if (snake.move != SNAKE_MOVE_UP && !snake.turn_in_progress) {
        snake.move = SNAKE_MOVE_DOWN;
        snake.turn_in_progress = true;
    }
}

static void snake_down_key_release(void) {
}

static void snake_next_figure_accepted(void) {
    snake_state.next_changed = false;
}

static void snake_game_loop(void) {
    if (snake.state == SNAKE_STATE_GAMEOVER)
        return;

    unsigned int t = GetTickCount();
    int speed = SNAKE_SPEED;

    if (t > snake_timer + speed) {
        snake_tick();
    }

    while (t > snake_timer + speed) {
        snake_timer += speed;
    }
}

static void snake_tick(void) {
    short next_head = (snake.head + 1) % SNAKE_MAX_SEGMENTS;
    short next_tail = (snake.tail + 1) % SNAKE_MAX_SEGMENTS;

    // Move head forward
    switch (snake.move) {
        case SNAKE_MOVE_UP:
            if (snake.segments[snake.head].y == 0)
                goto game_over;
            snake.segments[next_head].x = snake.segments[snake.head].x;
            snake.segments[next_head].y = snake.segments[snake.head].y - 1;
            break;
        case SNAKE_MOVE_DOWN:
            if (snake.segments[snake.head].y == BRICK_PLAYFIELD_HEIGHT - 1)
                goto game_over;
            snake.segments[next_head].x = snake.segments[snake.head].x;
            snake.segments[next_head].y = snake.segments[snake.head].y + 1;
            break;
        case SNAKE_MOVE_RIGHT:
            if (snake.segments[snake.head].x == BRICK_PLAYFIELD_WIDTH - 1)
                goto game_over;
            snake.segments[next_head].x = snake.segments[snake.head].x + 1;
            snake.segments[next_head].y = snake.segments[snake.head].y;
            break;
        case SNAKE_MOVE_LEFT:
            if (snake.segments[snake.head].x == 0)
                goto game_over;
            snake.segments[next_head].x = snake.segments[snake.head].x - 1;
            snake.segments[next_head].y = snake.segments[snake.head].y;
            break;
    }

    snake_state.playfield[snake.segments[next_head].x][snake.segments[next_head].y] = BRICK_FIELD_OCCUPIED;
    snake_state.playfield[snake.segments[snake.tail].x][snake.segments[snake.tail].y] = BRICK_FIELD_EMPTY;
    add_block_to_redraw_rectangle(snake.segments[next_head].x, snake.segments[next_head].y);
    add_block_to_redraw_rectangle(snake.segments[snake.tail].x, snake.segments[snake.tail].y);
    snake_state.rr.clean = 0;

    snake.head = next_head;
    snake.tail = next_tail;
    snake.turn_in_progress = false;
    return;

game_over:
    snake_state.game_over_notification_flag = true;
    snake.state = SNAKE_STATE_GAMEOVER;
    return;
}

static void add_block_to_redraw_rectangle(char x, char y) {
    if (y < snake_state.rr.top)
        snake_state.rr.top = y;
    if (y > snake_state.rr.bottom)
        snake_state.rr.bottom = y;
    if (x < snake_state.rr.left)
        snake_state.rr.left = x;
    if (x > snake_state.rr.right)
        snake_state.rr.right = x;
}
