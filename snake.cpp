#include "brick.h"
#include "cleananimation.h"
#include "winanimation.h"
#include "snake_levels.h"

#include <windows.h>
#include <time.h>

/* Interface static function prototypes */
static void snake_init(void);
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
static void snake_restart_level(void);

/* Other static function prototypes */
static void snake_init_after_cleananimation();
static void snake_tick(void);
static void add_block_to_redraw_rectangle(char x, char y);
static void place_food(void);

#define SNAKE_MAX_SEGMENTS        30  
#define SNAKE_SPEED              150

#define SNAKE_STATE_NORMAL         0
#define SNAKE_STATE_GAMEOVER       1
#define SNAKE_STATE_CLEANANIMATION 2
#define SNAKE_STATE_WINANIMATION   3

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

static snake_t snake = {{{5, 0}, {5, 1}, {5, 2}}, 1, 0, SNAKE_MOVE_DOWN, -1, 0, SNAKE_STATE_NORMAL, 0, 2, 0};
static segment_t food = {0, 0};
static unsigned int snake_timer = 0;

/******************************************************************************
 * Exported functions.
 ******************************************************************************/
void snake_init_interface(game_interface_t *iface) {
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
    iface->game_restart_level = snake_restart_level;
}

/******************************************************************************
 * Static functions.
 ******************************************************************************/
static void snake_init(void) {
    snake.state = SNAKE_STATE_CLEANANIMATION;
    brick_s.level = 1;
    brick_s.score = 0;
    brick_s.lives = 9;
    cleananimation_init();
}

static void snake_init_after_cleananimation() {
    int i, j;

    for (i = 0; i < BRICK_PLAYFIELD_WIDTH; i++) {
        for (j = 0; j < BRICK_PLAYFIELD_HEIGHT; j++) {
            if (snake_levels[brick_s.level - 1].level[j][i] == 0)
                brick_s.playfield[i][j] = BRICK_FIELD_EMPTY;
            else
                brick_s.playfield[i][j] = BRICK_FIELD_OCCUPIED_OUTER;
        }
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            brick_s.next[i][j] = BRICK_FIELD_OCCUPIED;
        }
    }

    brick_s.score_changed = 1;
    brick_s.level_changed = 1;
    brick_s.lives_changed = 1;
    brick_s.game_over_notification_flag = false;
    brick_s.winning_notification_flag = false;

    brick_s.rr.left = 0;
    brick_s.rr.top = 0;
    brick_s.rr.right = BRICK_PLAYFIELD_WIDTH;
    brick_s.rr.bottom = BRICK_PLAYFIELD_HEIGHT;
    brick_s.rr.clean = 0;

    brick_s.next_changed = true;

    snake.head = 2;
    snake.tail = 0;

    snake.segments[0] = snake_levels[brick_s.level - 1].start_segments[0];
    snake.segments[1] = snake_levels[brick_s.level - 1].start_segments[1];
    snake.segments[2] = snake_levels[brick_s.level - 1].start_segments[2];
    snake.move = snake_levels[brick_s.level - 1].start_move;
    snake.turn_in_progress = 0;
    snake.delayed_move = -1;
    snake.state = SNAKE_STATE_NORMAL;
    snake.increase_length = 0;
    snake.wait_before_hit = 2;
    snake.collected_food = 0;

    for (i = snake.tail; i <= snake.head; i++) {
        brick_s.playfield[snake.segments[i].x][snake.segments[i].y] = BRICK_FIELD_OCCUPIED_ORANGE;
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
    brick_s.next_changed = false;
}

static void snake_game_loop(void) {
    if (snake.state == SNAKE_STATE_CLEANANIMATION) {
        cleananimation();
        if (cleananimation() == CLEANANIMATION_DONE)
            snake_init_after_cleananimation();
        return;
    }

    if (snake.state == SNAKE_STATE_WINANIMATION) {
        winanimation();
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

static void snake_restart_level(void) {
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

    // Check if we bumped in ourselves
    if ((brick_s.playfield[snake.segments[next_head].x][snake.segments[next_head].y] != BRICK_FIELD_EMPTY) &&
        !(snake.segments[next_head].x == food.x && snake.segments[next_head].y == food.y))
        goto game_over;

    brick_s.playfield[snake.segments[next_head].x][snake.segments[next_head].y] = BRICK_FIELD_OCCUPIED_ORANGE;
    add_block_to_redraw_rectangle(snake.segments[next_head].x, snake.segments[next_head].y);
    snake.head = next_head;

    // Check if we reached the maximum length, in that case do not increase the length
    if (((next_head + 1) % SNAKE_MAX_SEGMENTS == snake.tail) ||
        (snake.increase_length == 0)) {
        brick_s.playfield[snake.segments[snake.tail].x][snake.segments[snake.tail].y] = BRICK_FIELD_EMPTY;
        add_block_to_redraw_rectangle(snake.segments[snake.tail].x, snake.segments[snake.tail].y);
        snake.tail = next_tail;
    }

    if (snake.increase_length > 0)
        snake.increase_length--;

    brick_s.rr.clean = 0;

    if (snake.delayed_move >= 0) {
        snake.move = snake.delayed_move;
        snake.delayed_move = -1;
    } else {
        snake.turn_in_progress = false;
    }

    // Check if we hit the food
    if (snake.segments[next_head].x == food.x &&
        snake.segments[next_head].y == food.y) {
        brick_s.score += 100;
        brick_s.score_changed = 1;
        snake.increase_length++;

        brick_s.next[3 - snake.collected_food % 4][3 - snake.collected_food / 4] = BRICK_FIELD_EMPTY;
        brick_s.next_changed = true;

        snake.collected_food++;
        if (snake.collected_food >= 16) {
            if (brick_s.level > SNAKE_LEVELS - 1) {
                winanimation_init();
                snake.state = SNAKE_STATE_WINANIMATION;
                brick_s.winning_notification_flag = true;
                return;
            }
            brick_s.level++;
            snake.state = SNAKE_STATE_CLEANANIMATION;
            cleananimation_init();
            return;
        }
        place_food();
    }

    snake.wait_before_hit = 2;
    return;

game_over:
    snake.turn_in_progress = false;
    if (snake.wait_before_hit > 0) {
        snake.wait_before_hit--;
        return;
    }
    if (brick_s.lives > 0) {
        brick_s.lives--;
        brick_s.lives_changed = true;
        brick_s.score -= snake.collected_food * 100;
        brick_s.score_changed = true;
        snake.state = SNAKE_STATE_CLEANANIMATION;
        cleananimation_init();
    } else {
        brick_s.game_over_notification_flag = true;
        snake.state = SNAKE_STATE_GAMEOVER;
    }
    return;
}

static void add_block_to_redraw_rectangle(char x, char y) {
    if (y < brick_s.rr.top)
        brick_s.rr.top = y;
    if (y > brick_s.rr.bottom)
        brick_s.rr.bottom = y;
    if (x < brick_s.rr.left)
        brick_s.rr.left = x;
    if (x > brick_s.rr.right)
        brick_s.rr.right = x;
}

static void place_food(void) {
    // Find an empty spot and place the food there.
    do {
        food.x = rand() % BRICK_PLAYFIELD_WIDTH;
        food.y = rand() % BRICK_PLAYFIELD_HEIGHT;
    } while (brick_s.playfield[food.x][food.y] != BRICK_FIELD_EMPTY);

    brick_s.playfield[food.x][food.y] = BRICK_FIELD_OCCUPIED_INNER;
    add_block_to_redraw_rectangle(food.x, food.y);
}
