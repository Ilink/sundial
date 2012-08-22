#include <ncurses.h>
#include <stdio.h>

int main(){
	
	// Initialize ncurses
	initscr();
	clear();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0);	

	// Define the main character initial position and symbol:
	int row = 10, col = 10;
	char main_char = '@';
	char ch;


	// Start the game loop
	while(1) {
		ch = getch();
		if(ch == 'q' || ch == 'Q') {
			break;
		}
		// If the user choses to stay, show the main character at position (row,col)
		else {
			mvaddch(row, col, main_char);
			refresh();
		}
	}
}