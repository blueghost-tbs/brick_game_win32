#ifndef _GFX_H_
#define _GFX_H_

#define GFX_MODE_FIRST           0
#define GFX_MODE_MONOCHROME      0
#define GFX_MODE_COLORED_TILE    1
#define GFX_MODE_COLORED_PYRAMID 2
#define GFX_MODE_LAST            2

void gfx_set_mode(int mode);
int  gfx_get_mode();
void gfx_get_brick(int size, int color, unsigned char *bitmap);

#endif
