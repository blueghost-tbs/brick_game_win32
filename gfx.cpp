#include "gfx.h"
#include "brick.h"

static int mode = GFX_MODE_MONOCHROME;

static void fill_monochrome(int size,
                            unsigned char *bitmap,
                            unsigned char outer_color,
                            unsigned char inner_color);

void gfx_set_mode(int set_mode) {
    switch (set_mode) {
        case GFX_MODE_MONOCHROME:
        case GFX_MODE_COLOR:
            mode = set_mode;
            break;
        default:
            mode = GFX_MODE_MONOCHROME;
            break;
    }
}

void gfx_get_brick(int size, int color, unsigned char *bitmap) {
    switch (color) {
        case BRICK_FIELD_EMPTY:
            fill_monochrome(size, bitmap, 170, 170);
            break;
        case BRICK_FIELD_OCCUPIED:
        case BRICK_FIELD_OCCUPIED_LIGHTBLUE:
        case BRICK_FIELD_OCCUPIED_DARKBLUE:
        case BRICK_FIELD_OCCUPIED_ORANGE:
        case BRICK_FIELD_OCCUPIED_YELLOW:
        case BRICK_FIELD_OCCUPIED_GREEN:
        case BRICK_FIELD_OCCUPIED_RED:
        case BRICK_FIELD_OCCUPIED_MAGENTA:
            fill_monochrome(size, bitmap, 0, 0);
            break;
        case BRICK_FIELD_OCCUPIED_INNER:
            fill_monochrome(size, bitmap, 170, 0);
            break;
        case BRICK_FIELD_OCCUPIED_OUTER:
            fill_monochrome(size, bitmap, 0, 170);
            break;
    }
}

/******************************************************************************
 * Static functions.
 ******************************************************************************/
static void fill_monochrome(int size,
                            unsigned char *bitmap,
                            unsigned char outer_color,
                            unsigned char inner_color) {
    int i, j;

    // Borders
    int block_border = size / 10;
    if (block_border < 2)
        block_border = 2;
    int outer_border = block_border * 7 / 10;
    for (i = outer_border; i < size - outer_border; i++) {
        for (j = outer_border; j < outer_border + block_border; j++) {
            // Vertical border
            bitmap[(i * size + j) * 4] = outer_color;
            bitmap[(i * size + j) * 4 + 1] = outer_color;
            bitmap[(i * size + j) * 4 + 2] = outer_color;
            // Horizontal border (i and j are swapped)
            bitmap[(j * size + i) * 4] = outer_color;
            bitmap[(j * size + i) * 4 + 1] = outer_color;
            bitmap[(j * size + i) * 4 + 2] = outer_color;
        }
        for (j = size - block_border - outer_border; j < size - outer_border; j++) {
            // Vertical border
            bitmap[(i * size + j) * 4] = outer_color;
            bitmap[(i * size + j) * 4 + 1] = outer_color;
            bitmap[(i * size + j) * 4 + 2] = outer_color;
            // Horizontal border (i and j are swapped)
            bitmap[(j * size + i) * 4] = outer_color;
            bitmap[(j * size + i) * 4 + 1] = outer_color;
            bitmap[(j * size + i) * 4 + 2] = outer_color;
        }
    }

    // Inner rectangle
    for (i = block_border * 2; i < size - block_border * 2; i++) {
        for (j = block_border * 2; j < size - block_border * 2; j++) {
            bitmap[(i * size + j) * 4] = inner_color;
            bitmap[(i * size + j) * 4 + 1] = inner_color;
            bitmap[(i * size + j) * 4 + 2] = inner_color;
        }
    }
}
