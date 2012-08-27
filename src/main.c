#include <ncurses.h>
#include <stdio.h>
#include <math.h>



// the typedef should go in the header
typedef struct {
	int x; 
	int y;
} point;

int linear_eq(int y2, int x1, int x2){
	int m = round(y2 / x1);
	int y = m * (round(x2 - x1)) + y2;
	return y;
}

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

double get_lstm(double hour_diff){
	return 
}

double get_hour_angle(double hour_diff, int days, double lng){
	double lstm = 15 * hour_diff;
	int b = 360/365 * (days-81);
	double time = 9.87 * sin(2*b) - 7.53 * cos(b) - 1.5*sin(b);
	double tc = 4*(lstm - lng) + time;
	double lst = hour_diff + tc / 60;
	
	return (double) 15 * (lst-12);
}

double get_time = function(){
	return 9.87
}

int main(){
	
	point a;
	a.x = 5;
	a.y = 5;

	point b;
	b.x = 0;
	b.y = 0;

	
	// printf("%d", y);

	// Initialize ncurses
	initscr();
	clear();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0);	

	// test symbol
	char main_char = '@';
	char ch;

	// Rendering loop
	draw_line(1,1,20,10, '@');
	refresh();
	while(1) {
		ch = getch();
		if(ch == 'q' || ch == 'Q') {
			break;
		}
	}
	endwin(); // clear ncurses's junk
}