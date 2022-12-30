#ifndef _BRICK_H_
#define _BRICK_H_

#define BRICK_PLAYFIELD_WIDTH         10
#define BRICK_PLAYFIELD_HEIGHT        20

/* Field states */
#define BRICK_FIELD_EMPTY              0
#define BRICK_FIELD_OCCUPIED           1
#define BRICK_FIELD_OCCUPIED_LIGHTBLUE 2
#define BRICK_FIELD_OCCUPIED_DARKBLUE  3
#define BRICK_FIELD_OCCUPIED_ORANGE    4
#define BRICK_FIELD_OCCUPIED_YELLOW    5
#define BRICK_FIELD_OCCUPIED_GREEN     6
#define BRICK_FIELD_OCCUPIED_RED       7
#define BRICK_FIELD_OCCUPIED_MAGENTA   8
#define BRICK_FIELD_OCCUPIED_INNER     9
#define BRICK_FIELD_OCCUPIED_OUTER    10

typedef struct {
    char left;
    char right;
    char top;
    char bottom;
    char clean;
} redraw_rectangle_t;

typedef struct {
    char playfield[BRICK_PLAYFIELD_WIDTH][BRICK_PLAYFIELD_HEIGHT];
    redraw_rectangle_t rr;
    char next[4][4];
    bool next_changed;
    bool game_over_notification_flag;
    bool winning_notification_flag;
    unsigned long score;
    char score_changed;
    int level;
    char level_changed;
    char lives;
    char lives_changed;
} brick_state_t;

typedef struct {
    brick_state_t* (*game_get_state)(void);
    void (*game_init)(void);
    void (*game_right_key_press)(void);
    void (*game_left_key_press)(void);
    void (*game_right_key_release)(void);
    void (*game_left_key_release)(void);
    void (*game_up_key_press)(void);
    void (*game_up_key_release)(void);
    void (*game_down_key_press)(void);
    void (*game_down_key_release)(void);
    void (*game_next_figure_accepted)(void);
    void (*game_loop)(void);
} game_interface_t;

void reset_redraw_rectangle(redraw_rectangle_t *rr);

#endif /* _BRICK_H_ */
