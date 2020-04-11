#ifndef _TETRIS_H_
#define _TETRIS_H_

#define TETRIS_PLAYFIELD_WIDTH  10
#define TETRIS_PLAYFIELD_HEIGHT 20
#define TETRIS_FIELD_EMPTY       0
#define TETRIS_FIELD_OCCUPIED    1

typedef struct {
    char playfield[TETRIS_PLAYFIELD_WIDTH][TETRIS_PLAYFIELD_HEIGHT];
} tetris_state_t;

void            tetris_init(void);
tetris_state_t *tetris_get_state(void);
void            tetris_tick(void);

#endif /* _TETRIS_H_ */
