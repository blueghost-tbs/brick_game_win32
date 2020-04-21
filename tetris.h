#ifndef _TETRIS_H_
#define _TETRIS_H_

#define TETRIS_PLAYFIELD_WIDTH  10
#define TETRIS_PLAYFIELD_HEIGHT 20
#define TETRIS_FIELD_EMPTY       0
#define TETRIS_FIELD_OCCUPIED    1

typedef struct {
    char left;
    char right;
    char top;
    char bottom;
    char clean;
} redraw_rectangle_t;

typedef struct {
    char playfield[TETRIS_PLAYFIELD_WIDTH][TETRIS_PLAYFIELD_HEIGHT];
    redraw_rectangle_t rr;
} tetris_state_t;

tetris_state_t *tetris_get_state(void);
void tetris_init(void);
void tetris_tick(void);
void tetris_right_key_press(void);
void tetris_left_key_press(void);
void tetris_right_key_release(void);
void tetris_left_key_release(void);
void tetris_up_key_press(void);
void tetris_up_key_release(void);
void tetris_down_key_press(void);
void tetris_down_key_release(void);
void tetris_reset_redraw_rectangle(void);
void tetris_game_loop(void);

#endif /* _TETRIS_H_ */
