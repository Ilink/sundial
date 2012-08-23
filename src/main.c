#include <ncurses.h>
#include <stdio.h>
#include <math.h>

// the typedef should go in the header
typedef struct {
	int x; 
	int y;
} point;

int linear_eq(double m, int x, double b){
	int y = m * x + b;
	return y;
}

point* make_line(point a, point b){
	/*
	find the equation for line
	allocate array big enough to fit the distance between them
	for(int i = 0; i < |b.x - a.x|; i++){ 
		
	}
		

	abs(b.y - a.y) = a.y/b.x * abs(a.x - b.x)
	*/
}

point* draw_line(point* line){

}

int main(){
	
	point a;
	a.x = 1;
	a.y = 1;

	// Initialize ncurses
	initscr();
	clear();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0);	

	// test symbol
	int row = 10, col = 10;
	char main_char = '@';
	char ch;


	// Rendering loop
	while(1) {
		ch = getch();
		if(ch == 'q' || ch == 'Q') {
			break;
			clear();
		} else {
			mvaddch(row, col, main_char);
			refresh();
		}
	}
}