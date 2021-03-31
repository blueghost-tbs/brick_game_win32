#ifndef _GFX_H_
#define _GFX_H_

#define GFX_MODE_MONOCHROME 0
#define GFX_MODE_COLOR      1

void gfx_set_mode(int mode);
void gfx_get_brick(int size, int color, unsigned char *bitmap);

#endif
