#ifndef _TETRIS_FIGURES_H_
#define _TETRIS_FIGURES_H_

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
    4,
    {{{1, 0, 0, 0},
      {1, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 0, 0}},
     {{0, 0, 0, 0},
      {0, 1, 1, 0},
      {1, 1, 0, 0},
      {0, 0, 0, 0}},
     {{0, 1, 0, 0},
      {0, 1, 1, 0},
      {0, 0, 1, 0},
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

tetris_figure_t tetris_figures[5] = {
    tetris_figure_0,
    tetris_figure_1,
    tetris_figure_2,
    tetris_figure_3,
    tetris_figure_4
};

#endif /* _TETRIS_FIGURES_H_*/
