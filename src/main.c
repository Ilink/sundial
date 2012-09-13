#include <ncurses.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "types.h"
#include "sun.h"
#include "drawing.h"
#include "util.h"
#include "main.h"

int main(){
	double lat = 37.9232; double lng = -122.2937; double tz = -8.0; // todo: get from console input
	remove("out.txt");
	remove("shadow.txt");
	remove("shadow2.txt");
	remove("scaling.txt");
	remove("ticks.txt");

	double JD2 = get_jd(2012, 8, 31);
	printf("JD 8/31/12: %f\n", JD2);
	double J2k = get_jd(2000, 1, 1);
	double n = 0;
	s_coord sun_pos = celestial(JD2, 37.9232, -122.2937, n, 25, -8.0);

	printf("Radial dist: %f\n", sun_pos.r);
	printf("Azimuth: %f\n", sun_pos.azimuth);
	printf("Elevation: %f\n", sun_pos.elevation);

	printf("Scaled Azimuth: %f\n", sun_pos.azimuth);
	printf("Scaled Elevation: %f\n", sun_pos.elevation);

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
	n = 0;
	double increment = 1;
	int j = 0;
	double x = 1.0;
	double y = 1.0;

	FILE* file;
	file = fopen("out.txt","a+");

	struct Arg* args = malloc(sizeof args);
	args->x_offset = 0;
	args->y_offset = 0;
	args->lat = lat;
	args->a = 1;

	// int offset_x = draw_ticks(args);
	pthread_t threads[5];
	int rc;
	
	rc = pthread_create(&threads[0], NULL, draw_ticks, args);
	if (rc){
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	}
	
	refresh();
	sun_pos = celestial(JD2, lat, lng, n, 25, tz);
	point_f sun_pos_point = s_coord_to_point(&sun_pos);
	graph_info g = get_graph_info(JD2, lat, lng, 1.0, tz);

	while(1) {
		// clear();
		struct winsize w;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

		double half_width = w.ws_col / 2.0;
		double y_midpoint = floor(w.ws_col/16.0);

		sun_pos = celestial(JD2, lat, lng, n, 25, tz);
		n+=increment;
		if(n > 24*60) n = 0.01;

		double midpoint = g.midpoint;
		scale_info s;
		point_f sun_pos_coord = s_coord_to_point(&sun_pos);
		s = console_scale(&sun_pos_coord, g.midpoint);

		double shadow_length = w.ws_row/3.3;
		point_f spoint = shadow_point(&sun_pos_coord, shadow_length, s.midpoint, y_midpoint);

		int x = ceil(spoint.y+ w.ws_row/1.5);
		int y = ceil(spoint.x+s.midpoint-2);
		
		// fprintf(file, "sc shadow x: %i\t", y);
		// fprintf(file, "sc shadow y: %i\n", x);
		
		draw_line(s.midpoint, 25, y, x, 'x'); // this one is right, but needs y-scaling

		// mvaddch(x,y, 'o');
		mvaddch(floor(sun_pos_coord.y), floor(sun_pos_coord.x), '~');

		j++;

		usleep(500);
		// usleep(5000);
		refresh();
	}
	fclose(file);
	endwin(); // clear ncurses's junk
	pthread_exit(NULL);
}