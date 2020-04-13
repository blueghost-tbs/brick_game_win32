#include "tetris.h"

static void tetris_get_next_figure(void);
static void draw_figure(void);
static void clear_figure(void);
static bool is_collide(void);

static tetris_state_t tetris_state;
static int current_brick_pos_x = 5;
static int current_brick_pos_y = -1;
static char next_figure = 1;

static const char tetris_figure0[4][4] = {
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0}
};

static const char tetris_figure1[4][4] = {
    {1, 1, 0, 0},
    {1, 1, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};

static const char tetris_figure2[4][4] = {
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 0, 0, 0}
};

static const char tetris_figure3[4][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 0}
};

static const char tetris_figure4[4][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {1, 0, 0, 0},
    {0, 0, 0, 0}
};

static char current_figure[4][4] = {
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0}
};

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
    if (current_brick_pos_x >= TETRIS_PLAYFIELD_WIDTH - 1)
        return;

    clear_figure();
    current_brick_pos_x++;
    if (is_collide())
        current_brick_pos_x--;
    draw_figure();
}

void tetris_left_key(void) {
    if (current_brick_pos_x <= 0)
        return;

    clear_figure();
    current_brick_pos_x--;
    if (is_collide())
        current_brick_pos_x++;
    draw_figure();
}

void tetris_down_key(void) {
    tetris_tick();
}

static void tetris_get_next_figure(void) {
    int i, j;

    const char (*f)[4][4] = &tetris_figure0;

    switch (next_figure) {
        case 0:
            f = &tetris_figure0;
            break;
        case 1:
            f = &tetris_figure1;
            break;
        case 2:
            f = &tetris_figure2;
            break;
        case 3:
            f = &tetris_figure3;
            break;
        case 4:
        default:
            f = &tetris_figure4;
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            current_figure[i][j] = (*f)[i][j];
        }
    }

    next_figure = (next_figure + 1) % 5;
}

static void draw_figure(void) {
    int i, j;

    if (current_brick_pos_y < 0)
        return;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (current_figure[i][j] == 1)
                tetris_state.playfield[current_brick_pos_x + i][current_brick_pos_y + j] = TETRIS_FIELD_OCCUPIED;
        }
    }
}

static void clear_figure(void) {
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (current_figure[i][j] == 1)
                tetris_state.playfield[current_brick_pos_x + i][current_brick_pos_y + j] = TETRIS_FIELD_EMPTY;
        }
    }
}

static bool is_collide(void) {
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (current_figure[i][j] == 1) {
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
