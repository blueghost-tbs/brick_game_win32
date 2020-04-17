#ifndef _TETRIS_FIGURES_H_
#define _TETRIS_FIGURES_H_

#define TETRIS_FIGURES_NUM 10

typedef struct {
    char size;
    char states_num;
    char states[4][4][4];
} tetris_figure_t;

tetris_figure_t tetris_figure_0 = {
    2,
    1,
    {{{1, 1, 0, 0},
      {1, 1, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}}}
};

tetris_figure_t tetris_figure_1 = {
    4,
    2,
    {{{0, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 1, 0, 0}},
     {{0, 0, 0, 0},
      {1, 1, 1, 1},
      {0, 0, 0, 0},
      {0, 0, 0, 0}}}
};

tetris_figure_t tetris_figure_2 = {
    3,
    4,
    {{{0, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 1, 1, 0},
      {0, 0, 0, 0}},
     {{0, 0, 1, 0},
      {1, 1, 1, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},
     {{1, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 0, 0}},
     {{0, 0, 0, 0},
      {1, 1, 1, 0},
      {1, 0, 0, 0},
      {0, 0, 0, 0}}}
};

tetris_figure_t tetris_figure_3 = {
    3,
    2,
    {{{1, 0, 0, 0},
      {1, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 0, 0}},
     {{0, 1, 1, 0},
      {1, 1, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}}}
};

tetris_figure_t tetris_figure_4 = {
    3,
    4,
    {{{0, 1, 0, 0},
      {0, 1, 1, 0},
      {0, 1, 0, 0},
      {0, 0, 0, 0}},
     {{0, 1, 0, 0},
      {1, 1, 1, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},
     {{0, 1, 0, 0},
      {1, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 0, 0}},
     {{0, 0, 0, 0},
      {1, 1, 1, 0},
      {0, 1, 0, 0},
      {0, 0, 0, 0}}}
};

tetris_figure_t tetris_figure_2_mirror = {
    3,
    4,
    {{{0, 1, 0, 0},
      {0, 1, 0, 0},
      {1, 1, 0, 0},
      {0, 0, 0, 0}},
     {{0, 0, 0, 0},
      {1, 1, 1, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 0}},
     {{0, 1, 1, 0},
      {0, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 0, 0}},
     {{1, 0, 0, 0},
      {1, 1, 1, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}}}
};

tetris_figure_t tetris_figure_3_mirror = {
    3,
    2,
    {{{0, 1, 0, 0},
      {1, 1, 0, 0},
      {1, 0, 0, 0},
      {0, 0, 0, 0}},
     {{1, 1, 0, 0},
      {0, 1, 1, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}}}
};

tetris_figure_t tetris_figures[TETRIS_FIGURES_NUM] = {
    tetris_figure_0,
    tetris_figure_1,
    tetris_figure_2,
    tetris_figure_3,
    tetris_figure_4,
    tetris_figure_0, // the mirror is the same
    tetris_figure_1, // the mirror is the same
    tetris_figure_2_mirror,
    tetris_figure_3_mirror,
    tetris_figure_4  // the mirror is the same
};

#endif /* _TETRIS_FIGURES_H_*/
