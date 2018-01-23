#ifndef _CRIS_UTILS_H
#define _CRIS_UTILS_H

extern void CRIS_draw_circle( int x0, int y0, int radius);
extern void CRIS_draw_circle_full( int x0, int y0, int radius);
extern void CRIS_draw_line( int x0, int y0, int x1, int y1);
extern void CRIS_draw_smiley_face( int x0, int y0, int radius);
extern void draw_snake_mouse(int x0, int y0, int flip_lights);
extern void clear_UFO(int x0, int y0);
extern void drawBlock(int x, int y, unsigned short c);
extern void move_UFO(int x0, int y0, int dir, int amnt);

#endif
