#ifndef _SOKOBAN_LEVELS_H_
#define _SOKOBAN_LEVELS_H_

#include "brick.h"

#define SOKOBAN_MAX_NUMBER_OF_STORAGE_LOCATIONS 10
#define SOKOBAN_LEVELS                           1

typedef struct {
    char x;
    char y;
} place_t;

typedef struct {
    char storage_locations_num;
    place_t storage_locations[SOKOBAN_MAX_NUMBER_OF_STORAGE_LOCATIONS];
    place_t player_start_location;
    char level[BRICK_PLAYFIELD_HEIGHT][BRICK_PLAYFIELD_WIDTH];
} sokoban_level_t;

sokoban_level_t sokoban_levels[SOKOBAN_LEVELS] = { {
    2,
    {{9, 19}, {8, 19}},
    {0, 0},
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 2, 2, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
} };

#endif /* _SOKOBAN_LEVELS_H_ */
