// 
// cristinel ababei
//
// this is a collection of various "graphics" functions used for
// drawing stuff on the LCD screen of the LandTiger 2.0 board;
// the vast majority of these are put together with a lot
// of open source code;
//
// Note 1: in lab#5 I had these functions at the end of GLCD.c
// however, I moved them into this separate file because now 
// here GLCD_LCP1700.c is different and I did not want to change it 
// like I did with GLCD.c
//
// Note 2: if in lab#5 I had this functions as LCD_* here I have them
// back to CRIS_* to see quickly what is what I added;
//


#include <stdio.h>
#include "LPC17xx.H"   
#include "GLCD.h"
#include "CRIS_UTILS.h"
#define LEFT 0
#define RIGHT 1
#define UP 2

// this is simply the midpoint circle algorithm for drawing a circle;
// it is a variant of Bresenham's line algorithm, and is thus sometimes 
// known as Bresenham's circle algorithm;
// see additional info:
// http://en.wikipedia.org/wiki/Midpoint_circle_algorithm  
// http://www.mathsisfun.com/algebra/circle-equations.html
// http://members.chello.at/~easyfilter/bresenham.html
void CRIS_draw_circle( int x0, int y0, int radius)
{
    int x = radius, y = 0;
    int xChange = 1 - (radius << 1);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y) 
    {
        GLCD_PutPixel(x + x0, y + y0); // call of GCLD function;
        GLCD_PutPixel(y + x0, x + y0);
        GLCD_PutPixel(-x + x0, y + y0);
        GLCD_PutPixel(-y + x0, x + y0);
        GLCD_PutPixel(-x + x0, -y + y0);
        GLCD_PutPixel(-y + x0, -x + y0);
        GLCD_PutPixel(x + x0, -y + y0);
        GLCD_PutPixel(y + x0, -x + y0);

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
}

// this is similar to CRIS_draw_circle() but the circle is also filled;
void CRIS_draw_circle_full( int x0, int y0, int radius) 
{
    int x = radius, y = 0;
    int xChange = 1 - (radius << 1);
    int yChange = 0;
    int radiusError = 0;
    int x_sweep = 0, x_limit = 0, y_const = 0;

    while (x >= y) 
    {
        // (1) draw the lower part of the circle;
        GLCD_PutPixel(x + x0, y + y0); // call of GCLD function;
        // scan line for "this y": y + y0
        x_sweep = -x+1+x0; // left start point;
        x_limit = x+1+x0; // right stop point;
        y_const = y+y0;
        while ( x_sweep < x_limit) {
            GLCD_PutPixel(x_sweep, y_const);
            x_sweep++;
        }       
        GLCD_PutPixel(y + x0, x + y0);
        // scan line for "this y": x + y0
        x_sweep = -y+1+x0; // left start point;
        x_limit = y+1+x0; // right stop point;
        y_const = x+y0;
        while ( x_sweep < x_limit) {
            GLCD_PutPixel(x_sweep, y_const);
            x_sweep++;
        }        
        GLCD_PutPixel(-x + x0, y + y0);
        GLCD_PutPixel(-y + x0, x + y0);
        
        // (2) draw the upper part of the circle;        
        GLCD_PutPixel(-x + x0, -y + y0);
        // scan line for "this y": -y + y0
        x_sweep = -x+1+x0; // left start point;
        x_limit = x+1+x0; // right stop point;
        y_const = -y+y0;
        while ( x_sweep < x_limit) {
            GLCD_PutPixel(x_sweep, y_const);
            x_sweep++;
        }
        GLCD_PutPixel(-y + x0, -x + y0);
        // scan line for "this y": -x + y0
        x_sweep = -y+1+x0; // left start point;
        x_limit = y+1+x0; // right stop point;
        y_const = -x+y0;
        while ( x_sweep < x_limit) {
            GLCD_PutPixel(x_sweep, y_const);
            x_sweep++;
        }
        GLCD_PutPixel(x + x0, -y + y0);
        GLCD_PutPixel(y + x0, -x + y0);

        // (3) prepare's for next iteration;
        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
}

// this is the so called Bresenham's line algorithm;
// draws a line between two points specified thru their
// x,y coordinates;
// see additional info:
// http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
// TODO: http://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm
void CRIS_draw_line( int x0, int y0, int x1, int y1) 
{ 
    int dx = x0<x1 ? (x1-x0) : (x0-x1); // basically dx=abs(x1-x0) 
    int sx = x0<x1 ? 1 : -1;
    int dy = y0<y1 ? (y1-y0) : (y0-y1);
    int sy = y0<y1 ? 1 : -1; 
    int err = (dx>dy ? dx : -dy)/2, e2;

    for (;;) {
        GLCD_PutPixel(x0,y0); // call of GCLD function;
        if (x0==x1 && y0==y1) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

// this draws a simplified smiley face;
// what would you change to improve it?
void CRIS_draw_smiley_face( int x0, int y0, int radius)
{
    int dx = radius >> 1;
    int eye_radius = radius >> 3;
    
    // nickel size should be radius = 38;
    // draw the large filled circle
		// the main color will be yellow
		GLCD_SetTextColor(Yellow);
    CRIS_draw_circle_full(x0, y0, radius);
    // draw eyes
    GLCD_SetTextColor(White);
    CRIS_draw_circle_full(x0-dx, y0-dx, eye_radius);
    CRIS_draw_circle_full(x0+dx, y0-dx, eye_radius); 
    // draw mouth; use simply 4 lines;
    CRIS_draw_line( x0-dx,y0+dx, x0+dx,y0+dx);
    CRIS_draw_line( x0-dx+2,y0+dx+1, x0+dx-2,y0+dx+1);
    CRIS_draw_line( x0-dx+4,y0+dx+2, x0+dx-4,y0+dx+2);
}

// method to draw a single 8x8 block at position (x,y) on the grid and of the color (c)
void drawBlock(int x, int y, unsigned short c){
	int i, j;
	x = x*23;
	y = y*23;
	GLCD_SetTextColor(c);
	for(i = 0; i < 23; i++){
			for(j = 0; j < 23; j++){
				GLCD_PutPixel(x-i, y-j);
				
			}
		}
}

// method to "clear" the UFO by drawing black blocks over the old x and y positions
void clear_UFO(int x0, int y0)
{
		int i = 0;
		for(i = 0; i < 13;i++){
			drawBlock(x0-6+i, y0, Black);
			drawBlock(x0+6-i, y0, Black);
		}
		for(i = 0; i < 7; i++){
			drawBlock(x0-3+i, y0-2, Black);
			drawBlock(x0+3-i, y0+2, Black);
		}
		for(i = 0; i < 9; i++){
			drawBlock(x0-4+i, y0-1, Black);
			drawBlock(x0+4-i, y0+1, Black);
		}
}

// method to move the UFO left and right across the screen
// when moving left, deletes blocks on right side and draws and left
// opposite for when moving right
void move_UFO(int x0, int y0, int dir, int amnt){
	int i;
	if(dir == LEFT){
		for(i = 0; i < amnt; i++){
			drawBlock(x0+6-i, y0, Black);
			drawBlock(x0+4-i, y0-1, Black);
			drawBlock(x0+4-i, y0+1, Black);
			drawBlock(x0+3-i, y0+2, Black);
			drawBlock(x0+3-i, y0-2, Black);
		}
	}
	else if(dir == RIGHT)
	{
		for(i = 0; i < amnt; i++){
			drawBlock(x0-6+i, y0, Black);
			drawBlock(x0-4+i, y0-1, Black);
			drawBlock(x0-4+i, y0+1, Black);
			drawBlock(x0-3+i, y0+2, Black);
			drawBlock(x0-3+i, y0-2, Black);
		}
	}
}

// method to draw the UFO
// three for loops - for each section of the UFO
void draw_snake_mouse(int x0, int y0, int col)
{
		//int i = 0;
		//unsigned short color;
		drawBlock(x0, y0, col);
}
