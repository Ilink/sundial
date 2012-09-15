#include "types.h"
#include "util.h"

point_f s_coord_to_point(s_coord* s){
	point_f p;
	p.x = s->azimuth;
	p.y = s->elevation;
	return p;
}

// thank you wikipedia
// http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
void draw_line(int x0, int y0, int x1, int y1, char render_char){
	int dx = abs(x1-x0);
	int dy = abs(y1-y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;

	// this examines how steep the slope is
	int err = dx - dy;

	for(;;){
		mvaddch(y0, x0, render_char);
		if(x0 == x1 && y0 == y1) break;
		int e2 = 2*err; 

		// i think this examines whether or not the slope is steep enough to have Y be the driving force. A steep line should iterate over Y instead of X.
		if(e2 > -dy){
			err -= dy; // reset the accumulation to 0
			x0 += sx;
		} else if(e2 < dx){
			err += dx; // accumulate more fractions
			y0 += sy;
		}
	}
}

// minor modifications to: http://free.pages.at/easyfilter/bresenham.html
void draw_circle(int x_offset, int y_offset, int r, char c){
	int x = -r, y = 0, err = 2-2*r;
	do {
		mvaddch(y_offset+y,x_offset-x, c);
		mvaddch(y_offset-y,x_offset-x, c);
		mvaddch(y_offset-y,x_offset+x, c);
		mvaddch(y_offset+y,x_offset+x, c);
		r = err;
		if (r <= y) err += ++y*2+1;
		if (r > x || err > y) err += ++x*2+1;
	} while (x < 0);
}

// minor modifications to: https://banu.com/blog/7/drawing-circles/
void draw_filled_circle(int y_offset, int x_offset, int radius, char c){
	int x, y;
	for (y = -radius; y <= radius; y++)
		for (x = -radius; x <= radius; x++)
			if ((x * x) + (y * y) <= (radius * radius))
				mvaddch (y+y_offset, x+x_offset, c);
}