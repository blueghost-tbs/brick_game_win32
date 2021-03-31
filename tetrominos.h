#ifndef _TETROMINOS_H_
#define _TETROMINOS_H_

#define TETROMINOS_NUM 7

typedef struct {
    char size;
    char states_num;
    char states[4][4][4];
} tetromino_t;

tetromino_t tetromino_0 = {
    2,
    1,
    {{{5, 5, 0, 0},
      {5, 5, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}}}
};

tetromino_t tetromino_1 = {
    4,
    2,
    {{{0, 2, 0, 0},
      {0, 2, 0, 0},
      {0, 2, 0, 0},
      {0, 2, 0, 0}},
     {{0, 0, 0, 0},
      {2, 2, 2, 2},
      {0, 0, 0, 0},
      {0, 0, 0, 0}}}
};

tetromino_t tetromino_2 = {
    3,
    4,
    {{{0, 4, 0, 0},
      {0, 4, 0, 0},
      {0, 4, 4, 0},
      {0, 0, 0, 0}},
     {{0, 0, 4, 0},
      {4, 4, 4, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},
     {{4, 4, 0, 0},
      {0, 4, 0, 0},
      {0, 4, 0, 0},
      {0, 0, 0, 0}},
     {{0, 0, 0, 0},
      {4, 4, 4, 0},
      {4, 0, 0, 0},
      {0, 0, 0, 0}}}
};

tetromino_t tetromino_3 = {
    3,
    2,
    {{{6, 0, 0, 0},
      {6, 6, 0, 0},
      {0, 6, 0, 0},
      {0, 0, 0, 0}},
     {{0, 6, 6, 0},
      {6, 6, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}}}
};

tetromino_t tetromino_4 = {
    3,
    4,
    {{{0, 8, 0, 0},
      {0, 8, 8, 0},
      {0, 8, 0, 0},
      {0, 0, 0, 0}},
     {{0, 8, 0, 0},
      {8, 8, 8, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},
     {{0, 8, 0, 0},
      {8, 8, 0, 0},
      {0, 8, 0, 0},
      {0, 0, 0, 0}},
     {{0, 0, 0, 0},
      {8, 8, 8, 0},
      {0, 8, 0, 0},
      {0, 0, 0, 0}}}
};

tetromino_t tetromino_5 = {
    3,
    4,
    {{{0, 3, 0, 0},
      {0, 3, 0, 0},
      {3, 3, 0, 0},
      {0, 0, 0, 0}},
     {{0, 0, 0, 0},
      {3, 3, 3, 0},
      {0, 0, 3, 0},
      {0, 0, 0, 0}},
     {{0, 3, 3, 0},
      {0, 3, 0, 0},
      {0, 3, 0, 0},
      {0, 0, 0, 0}},
     {{3, 0, 0, 0},
      {3, 3, 3, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}}}
};

tetromino_t tetromino_6 = {
    3,
    2,
    {{{0, 7, 0, 0},
      {7, 7, 0, 0},
      {7, 0, 0, 0},
      {0, 0, 0, 0}},
     {{7, 7, 0, 0},
      {0, 7, 7, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}}}
};

tetromino_t *tetrominos[TETROMINOS_NUM] = {
    &tetromino_0,
    &tetromino_1,
    &tetromino_2,
    &tetromino_3,
    &tetromino_4,
    &tetromino_5,
    &tetromino_6
};

#endif /* _TETROMINOS_H_*/
