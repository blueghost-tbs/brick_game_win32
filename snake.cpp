#include "brick.h"
#include "cleananimation.h"
#include "snake_levels.h"

#include <windows.h>
#include <time.h>

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
static void snake_init_after_cleananimation();
static void snake_tick(void);
static void add_block_to_redraw_rectangle(char x, char y);
static void place_food(void);

static brick_state_t snake_state;

#define SNAKE_MAX_SEGMENTS        30  
#define SNAKE_SPEED              150

#define SNAKE_STATE_NORMAL         0
#define SNAKE_STATE_GAMEOVER       1
#define SNAKE_STATE_CLEANANIMATION 2

typedef struct {
    segment_t segments[SNAKE_MAX_SEGMENTS];
    short head;
    short tail;
    char move;
    char delayed_move;
    char turn_in_progress;
    char state;
    char increase_length;
    char wait_before_hit;
    char collected_food;
} snake_t;

static snake_t snake = {{{5, 0}, {5, 1}, {5, 2}}, 1, 0, SNAKE_MOVE_DOWN, -1, SNAKE_STATE_NORMAL, 0, 2, 0};
static segment_t food = {0, 0};
static unsigned int snake_timer = 0;
static unsigned char cleananimation_line = 0;

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
    snake.state = SNAKE_STATE_CLEANANIMATION;
    snake_state.level = 1;
    snake_state.score = 0;
    cleananimation_init();
}

static void snake_init_after_cleananimation() {
    int i, j;

    for (i = 0; i < BRICK_PLAYFIELD_WIDTH; i++) {
        for (j = 0; j < BRICK_PLAYFIELD_HEIGHT; j++) {
            if (snake_levels[snake_state.level - 1].level[j][i] == 0)
                snake_state.playfield[i][j] = BRICK_FIELD_EMPTY;
            else
                snake_state.playfield[i][j] = BRICK_FIELD_OCCUPIED_OUTER;
        }
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            snake_state.next[i][j] = BRICK_FIELD_OCCUPIED;
        }
    }

    snake_state.score_changed = 1;
    snake_state.level_changed = 1;
    snake_state.lines_since_level_increase = 0;
    snake_state.game_over_notification_flag = false;

    snake_state.rr.left = 0;
    snake_state.rr.top = 0;
    snake_state.rr.right = BRICK_PLAYFIELD_WIDTH;
    snake_state.rr.bottom = BRICK_PLAYFIELD_HEIGHT;
    snake_state.rr.clean = 0;

    snake_state.next_changed = true;

    snake.head = 2;
    snake.tail = 0;

    snake.segments[0] = snake_levels[snake_state.level - 1].start_segments[0];
    snake.segments[1] = snake_levels[snake_state.level - 1].start_segments[1];
    snake.segments[2] = snake_levels[snake_state.level - 1].start_segments[2];
    snake.move = snake_levels[snake_state.level - 1].start_move;
    snake.delayed_move = -1;
    snake.state = SNAKE_STATE_NORMAL;
    snake.increase_length = 0;
    snake.wait_before_hit = 2;
    snake.collected_food = 0;

    for (i = snake.tail; i <= snake.head; i++) {
        snake_state.playfield[snake.segments[i].x][snake.segments[i].y] = BRICK_FIELD_OCCUPIED;
    }

    srand(time(NULL));
    place_food();
}

static void snake_right_key_press(void) {
    if (snake.move != SNAKE_MOVE_LEFT) {
        if (!snake.turn_in_progress) {
            snake.move = SNAKE_MOVE_RIGHT;
            snake.turn_in_progress = true;
        } else if (snake.delayed_move < 0) {
            snake.delayed_move = SNAKE_MOVE_RIGHT;
        }
    }
}

static void snake_left_key_press(void) {
    if (snake.move != SNAKE_MOVE_RIGHT) {
        if (!snake.turn_in_progress) {
            snake.move = SNAKE_MOVE_LEFT;
            snake.turn_in_progress = true;
        } else if (snake.delayed_move < 0) {
            snake.delayed_move = SNAKE_MOVE_LEFT;
        }
    }
}

static void snake_right_key_release(void) {
}

static void snake_left_key_release(void) {
}

static void snake_up_key_press(void) {
    if (snake.move != SNAKE_MOVE_DOWN) {
        if (!snake.turn_in_progress) { 
            snake.move = SNAKE_MOVE_UP;
            snake.turn_in_progress = true;
        } else if (snake.delayed_move < 0) {
            snake.delayed_move = SNAKE_MOVE_UP;
        }
    }
}

static void snake_up_key_release(void) {
}

static void snake_down_key_press(void) {
    if (snake.move != SNAKE_MOVE_UP) {
        if (!snake.turn_in_progress) {
            snake.move = SNAKE_MOVE_DOWN;
            snake.turn_in_progress = true;
        } else if (snake.delayed_move < 0) {
            snake.delayed_move = SNAKE_MOVE_DOWN;
        }
    }
}

static void snake_down_key_release(void) {
}

static void snake_next_figure_accepted(void) {
    snake_state.next_changed = false;
}

static void snake_game_loop(void) {
    if (snake.state == SNAKE_STATE_CLEANANIMATION) {
        cleananimation(&snake_state);
        if (cleananimation(&snake_state) == CLEANANIMATION_DONE)
            snake_init_after_cleananimation();
        return;
    }

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

    // Check if bumped in ourselves
    if ((snake_state.playfield[snake.segments[next_head].x][snake.segments[next_head].y] != BRICK_FIELD_EMPTY) &&
        !(snake.segments[next_head].x == food.x && snake.segments[next_head].y == food.y))
        goto game_over;

    snake_state.playfield[snake.segments[next_head].x][snake.segments[next_head].y] = BRICK_FIELD_OCCUPIED;
    add_block_to_redraw_rectangle(snake.segments[next_head].x, snake.segments[next_head].y);
    snake.head = next_head;

    // Check if we reached the maximum length, in that case do not increase the length
    if (((next_head + 1) % SNAKE_MAX_SEGMENTS == snake.tail) ||
        (snake.increase_length == 0)) {
        snake_state.playfield[snake.segments[snake.tail].x][snake.segments[snake.tail].y] = BRICK_FIELD_EMPTY;
        add_block_to_redraw_rectangle(snake.segments[snake.tail].x, snake.segments[snake.tail].y);
        snake.tail = next_tail;
    }
    
    if (snake.increase_length > 0)
        snake.increase_length--;

    snake_state.rr.clean = 0;

    if (snake.delayed_move >= 0) {
        snake.move = snake.delayed_move;
        snake.delayed_move = -1;
    } else {
        snake.turn_in_progress = false;
    }

    // Check if we hit the food
    if (snake.segments[next_head].x == food.x &&
        snake.segments[next_head].y == food.y) {
        snake_state.score += 100;
        snake_state.score_changed = 1;
        snake.increase_length++;

        snake_state.next[3 - snake.collected_food % 4][3 - snake.collected_food / 4] = BRICK_FIELD_EMPTY;
        snake_state.next_changed = true;

        snake.collected_food++;
        if (snake.collected_food >= 16) {
            snake_state.level++;
            if (snake_state.level > SNAKE_LEVELS)
                snake_state.level = 1;
            snake.state = SNAKE_STATE_CLEANANIMATION;
            cleananimation_init();
            return;
        }
        place_food();
    }

    snake.wait_before_hit = 2;
    return;

game_over:
    if (snake.wait_before_hit > 0) {
        snake.wait_before_hit--;
        return;
    }
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

static void place_food(void) {
    // Find an empty spot and place the food there.
    do {
        food.x = rand() % BRICK_PLAYFIELD_WIDTH;
        food.y = rand() % BRICK_PLAYFIELD_HEIGHT;
    } while (snake_state.playfield[food.x][food.y] != BRICK_FIELD_EMPTY);

    snake_state.playfield[food.x][food.y] = BRICK_FIELD_OCCUPIED_INNER;
    add_block_to_redraw_rectangle(food.x, food.y);
}
