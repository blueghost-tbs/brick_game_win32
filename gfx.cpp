#include "gfx.h"
#include "brick.h"

#include <string.h>

static int mode = GFX_MODE_COLORED_TILE;

static void fill_monochrome(int size,
                            unsigned char *bitmap,
                            unsigned char outer_color,
                            unsigned char inner_color,
                            unsigned char small_inner);

static void fill_colored_tile(int size,
                              unsigned char *bitmap,
                              unsigned char red,
                              unsigned char green,
                              unsigned char blue,
                              unsigned char inner,
                              unsigned char outer,
                              unsigned char small_inner);

static void fill_colored_pyramid(int size,
                                 unsigned char* bitmap,
                                 unsigned char red,
                                 unsigned char green,
                                 unsigned char blue,
                                 unsigned char inner,
                                 unsigned char outer,
                                 unsigned char small_inner);

void gfx_set_mode(int set_mode) {
    switch (set_mode) {
        case GFX_MODE_MONOCHROME:
        case GFX_MODE_COLORED_TILE:
        case GFX_MODE_COLORED_PYRAMID:
            mode = set_mode;
            break;
        default:
            mode = GFX_MODE_MONOCHROME;
            break;
    }
}

int gfx_get_mode(void) {
    return mode;
}

void gfx_get_brick(int size, int color, unsigned char *bitmap) {
    if (mode == GFX_MODE_MONOCHROME) {
        switch (color) {
            case BRICK_FIELD_EMPTY:
                fill_monochrome(size, bitmap, 170, 170, 0);
                break;
            case BRICK_FIELD_OCCUPIED:
            case BRICK_FIELD_OCCUPIED_LIGHTBLUE:
            case BRICK_FIELD_OCCUPIED_DARKBLUE:
            case BRICK_FIELD_OCCUPIED_ORANGE:
            case BRICK_FIELD_OCCUPIED_YELLOW:
            case BRICK_FIELD_OCCUPIED_GREEN:
            case BRICK_FIELD_OCCUPIED_RED:
            case BRICK_FIELD_OCCUPIED_MAGENTA:
                fill_monochrome(size, bitmap, 0, 0, 0);
                break;
            case BRICK_FIELD_OCCUPIED_INNER:
                fill_monochrome(size, bitmap, 170, 0, 0);
                break;
            case BRICK_FIELD_OCCUPIED_OUTER:
                fill_monochrome(size, bitmap, 0, 170, 0);
                break;
            case BRICK_FIELD_OCCUPIED_INNER_SMALL:
                fill_monochrome(size, bitmap, 170, 0, 1);
                break;

        }
    } else if (mode == GFX_MODE_COLORED_TILE) {
        switch (color) {
            case BRICK_FIELD_EMPTY:
                fill_colored_tile(size, bitmap, 15, 15, 15, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED:
                fill_colored_tile(size, bitmap, 140, 140, 140, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_LIGHTBLUE:
                fill_colored_tile(size, bitmap, 0, 140, 140, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_DARKBLUE:
                fill_colored_tile(size, bitmap, 0, 0, 140, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_ORANGE:
                fill_colored_tile(size, bitmap, 140, 70, 0, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_YELLOW:
                fill_colored_tile(size, bitmap, 140, 140, 0, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_GREEN:
                fill_colored_tile(size, bitmap, 0, 140, 0, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_RED:
                fill_colored_tile(size, bitmap, 140, 0, 0, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_MAGENTA:
                fill_colored_tile(size, bitmap, 140, 0, 140, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_INNER:
                fill_colored_tile(size, bitmap, 0, 140, 140, 1, 0, 0);
                break;
            case BRICK_FIELD_OCCUPIED_INNER_SMALL:
                fill_colored_tile(size, bitmap, 0, 140, 0, 1, 0, 1);
                break;
            case BRICK_FIELD_OCCUPIED_OUTER:
                fill_colored_tile(size, bitmap, 140, 140, 140, 0, 1, 0);
                break;
        }
    } else if (mode == GFX_MODE_COLORED_PYRAMID) {
        switch (color) {
            case BRICK_FIELD_EMPTY:
                fill_colored_pyramid(size, bitmap, 15, 15, 15, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED:
                fill_colored_pyramid(size, bitmap, 145, 145, 145, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_LIGHTBLUE:
                fill_colored_pyramid(size, bitmap, 5, 145, 145, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_DARKBLUE:
                fill_colored_pyramid(size, bitmap, 5, 5, 145, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_ORANGE:
                fill_colored_pyramid(size, bitmap, 145, 75, 5, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_YELLOW:
                fill_colored_pyramid(size, bitmap, 145, 145, 5, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_GREEN:
                fill_colored_pyramid(size, bitmap, 5, 145, 5, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_RED:
                fill_colored_pyramid(size, bitmap, 145, 5, 5, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_MAGENTA:
                fill_colored_pyramid(size, bitmap, 145, 5, 145, 1, 1, 0);
                break;
            case BRICK_FIELD_OCCUPIED_INNER:
                fill_colored_pyramid(size, bitmap, 5, 145, 145, 1, 0, 0);
                break;
            case BRICK_FIELD_OCCUPIED_INNER_SMALL:
                fill_colored_pyramid(size, bitmap, 5, 145, 5, 1, 0, 1);
                break;
            case BRICK_FIELD_OCCUPIED_OUTER:
                fill_colored_pyramid(size, bitmap, 145, 145, 145, 0, 1, 0);
                break;
        }
    }
}

/******************************************************************************
 * Static functions.
 ******************************************************************************/
static void fill_monochrome(int size,
                            unsigned char *bitmap,
                            unsigned char outer_color,
                            unsigned char inner_color,
                            unsigned char small_inner) {
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

    int inner_border = block_border * 2;
    if (small_inner)
        inner_border = block_border * 4;

    // Inner rectangle
    for (i = inner_border; i < size - inner_border; i++) {
        for (j = inner_border; j < size - inner_border; j++) {
            bitmap[(i * size + j) * 4] = inner_color;
            bitmap[(i * size + j) * 4 + 1] = inner_color;
            bitmap[(i * size + j) * 4 + 2] = inner_color;
        }
    }
}

static void fill_colored_pyramid(int size,
                                 unsigned char *bitmap,
                                 unsigned char red,
                                 unsigned char green,
                                 unsigned char blue,
                                 unsigned char inner,
                                 unsigned char outer,
                                 unsigned char small_inner) {
    int i, j;
    float f;
    int color_gradient;

    unsigned char red_0 = (unsigned char)(red * 0.3);
    unsigned char green_0 = (unsigned char)(green * 0.3);
    unsigned char blue_0 = (unsigned char)(blue * 0.3);

    unsigned char red_1 = (unsigned char)(red * 0.5);
    unsigned char green_1 = (unsigned char)(green * 0.5);
    unsigned char blue_1 = (unsigned char)(blue * 0.5);

    f = red * 1.35f;
    unsigned char red_2 = (unsigned char)(f > 254 ? 255 : f);
    f = green * 1.35f;
    unsigned char green_2 = (unsigned char)(f > 254 ? 255 : f);
    f = blue * 1.35f;
    unsigned char blue_2 = (unsigned char)(f > 254 ? 255 : f);

    f = red * 1.8f;
    unsigned char red_3 = (unsigned char)(f > 254 ? 255 : f);
    f = green * 1.8f;
    unsigned char green_3 = (unsigned char)(f > 254 ? 255 : f);
    f = blue * 1.8f;
    unsigned char blue_3 = (unsigned char)(f > 254 ? 255 : f);

    // Borders
    int block_border = size / 10;
    if (block_border < 2)
        block_border = 2;

    memset(bitmap, 0, size * size * 4);

    if (outer) {
        // Outer color
        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
                color_gradient = (int)((float)((size - i) + j) / size * 10 - 5);
                // Exclude the square in the center
                if ((i > block_border * 2)        &&
                    (i < size - block_border * 2) &&
                    (j > block_border * 2)        &&
                    (j < size - block_border * 2))
                    continue;
                bitmap[(i * size + j) * 4] = blue + color_gradient;
                bitmap[(i * size + j) * 4 + 1] = green + color_gradient;
                bitmap[(i * size + j) * 4 + 2] = red + color_gradient;
            }
        }
    }

    int inner_border = block_border * 2;
    if (small_inner)
        inner_border = block_border * 4;

    if (inner) {
        // Draw innner pyramid
        // Bottom color
        for (i = inner_border; i < size / 2; i++) {
            for (j = inner_border; j < size - inner_border; j++) {
                if (i > j)
                    continue;
                if (i > size - j)
                    continue;
                bitmap[(i * size + j) * 4] = blue_0;
                bitmap[(i * size + j) * 4 + 1] = green_0;
                bitmap[(i * size + j) * 4 + 2] = red_0;
            }
        }
        // Right color
        for (i = inner_border; i < size - inner_border; i++) {
            for (j = size / 2; j < size - inner_border; j++) {
                if (i > j)
                    continue;
                if (i < size - j)
                    continue;
                bitmap[(i * size + j) * 4] = blue_1;
                bitmap[(i * size + j) * 4 + 1] = green_1;
                bitmap[(i * size + j) * 4 + 2] = red_1;
            }
        }
        // Left color
        for (i = inner_border; i < size - inner_border; i++) {
            for (j = inner_border; j <= size / 2 ; j++) {
                if (i < j)
                    continue;
                if (i > size - j)
                    continue;
                bitmap[(i * size + j) * 4] = blue_2;
                bitmap[(i * size + j) * 4 + 1] = green_2;
                bitmap[(i * size + j) * 4 + 2] = red_2;
            }
        }
        // Top color
        for (i = size / 2; i < size - inner_border; i++) {
            for (j = inner_border; j < size - inner_border; j++) {
                if (i < j)
                    continue;
                if (i < size - j)
                    continue;
                bitmap[(i * size + j) * 4] = blue_3;
                bitmap[(i * size + j) * 4 + 1] = green_3;
                bitmap[(i * size + j) * 4 + 2] = red_3;
            }
        }
    }
}

static void fill_colored_tile(int size,
                              unsigned char* bitmap,
                              unsigned char red,
                              unsigned char green,
                              unsigned char blue,
                              unsigned char inner,
                              unsigned char outer,
                              unsigned char small_inner) {
    int i, j;
    float f;

    unsigned char red_0 = (unsigned char)(red * 0.3);
    unsigned char green_0 = (unsigned char)(green * 0.3);
    unsigned char blue_0 = (unsigned char)(blue * 0.3);

    unsigned char red_1 = (unsigned char)(red * 0.5);
    unsigned char green_1 = (unsigned char)(green * 0.5);
    unsigned char blue_1 = (unsigned char)(blue * 0.5);

    f = red * 1.35f;
    unsigned char red_2 = (unsigned char)(f > 254 ? 255 : f);
    f = green * 1.35f;
    unsigned char green_2 = (unsigned char)(f > 254 ? 255 : f);
    f = blue * 1.35f;
    unsigned char blue_2 = (unsigned char)(f > 254 ? 255 : f);

    f = red * 1.8f;
    unsigned char red_3 = (unsigned char)(f > 254 ? 255 : f);
    f = green * 1.8f;
    unsigned char green_3 = (unsigned char)(f > 254 ? 255 : f);
    f = blue * 1.8f;
    unsigned char blue_3 = (unsigned char)(f > 254 ? 255 : f);

    // Borders
    int block_border = size / 10;
    if (block_border < 2)
        block_border = 2;

    memset(bitmap, 0, size * size * 4);

    int inner_border = block_border * 2;
    if (small_inner)
        inner_border = block_border * 4;

    if (inner) {
        // Middle color
        for (i = inner_border; i < size - inner_border; i++) {
            for (j = inner_border; j < size - inner_border; j++) {
                bitmap[(i * size + j) * 4] = blue;
                bitmap[(i * size + j) * 4 + 1] = green;
                bitmap[(i * size + j) * 4 + 2] = red;
            }
        }
    }

    if (outer) {
        // Bottom color
        for (i = 0; i < block_border * 2; i++) {
            for (j = 0; j < size; j++) {
                if (i > j)
                    continue;
                if (i > size - j)
                    continue;
                bitmap[(i * size + j) * 4] = blue_0;
                bitmap[(i * size + j) * 4 + 1] = green_0;
                bitmap[(i * size + j) * 4 + 2] = red_0;
            }
        }
        // Right color
        for (i = 0; i < size; i++) {
            for (j = size - block_border * 2; j < size; j++) {
                if (i > j)
                    continue;
                if (i < size - j)
                    continue;
                bitmap[(i * size + j) * 4] = blue_1;
                bitmap[(i * size + j) * 4 + 1] = green_1;
                bitmap[(i * size + j) * 4 + 2] = red_1;
            }
        }
        // Left color
        for (i = 0; i < size; i++) {
            for (j = 0; j < block_border * 2; j++) {
                if (i < j)
                    continue;
                if (i > size - j)
                    continue;
                bitmap[(i * size + j) * 4] = blue_2;
                bitmap[(i * size + j) * 4 + 1] = green_2;
                bitmap[(i * size + j) * 4 + 2] = red_2;
            }
        }
        // Top color
        for (i = size - block_border * 2; i < size; i++) {
            for (j = 0; j < size; j++) {
                if (i < j)
                    continue;
                if (i < size - j)
                    continue;
                bitmap[(i * size + j) * 4] = blue_3;
                bitmap[(i * size + j) * 4 + 1] = green_3;
                bitmap[(i * size + j) * 4 + 2] = red_3;
            }
        }
    }
}
