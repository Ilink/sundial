#include <ncurses.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <getopt.h>
// #include <string.h>
#include "types.h"
#include "sun.h"
#include "drawing.h"
#include "util.h"
#include "main.h"

static const char *opt_string = "l:n:h";
static struct option long_options[] = {
	{"lat", required_argument, 0, 'l'},
	{"lng", required_argument, 0, 'n'},
	{"help", no_argument, NULL, 'h'},
	{NULL, no_argument, NULL, 0}
};

static struct Required_arguments {
	int lat;
	int lng;
} required_arguments;

int main(int argc, char *argv[]){
	int c;
	double lat, lng, tz;
	while(1){
		int option_index = 0;
		c = getopt_long (argc, argv, opt_string, long_options, &option_index);
		if(c == -1) break;

		switch (c){
			case 'l':
				lat = strtod(optarg, NULL);
				required_arguments.lat = 1;
				break;
			case 'n':
				lng = strtod(optarg, NULL);
				required_arguments.lng = 1;
				break;
			case 'h':
				break;
			default:
				break;
		}
	}
	
	if(required_arguments.lat && required_arguments.lng){
		printf("ALL SYSTEMS ARE GO");
		// return;
	} else {
		lat = 37.9232; lng = -122.2937;
		// return;
	}

	tz = -8.0;
	remove("out.txt");
	remove("shadow.txt");
	remove("shadow2.txt");
	remove("scaling.txt");
	remove("ticks.txt");
	remove("time.txt");

	time_t rawtime;
	struct tm *ptm;

	time(&rawtime);
	ptm = localtime(&rawtime);

	int year = 1900 + ptm->tm_year;
	int day = ptm->tm_mday;
	int month = ptm->tm_mon+1;

	double JD2 = get_jd(year, month, day);
	printf("JD 8/31/12: %f\n", JD2);
	double J2k = get_jd(2000, 1, 1);
	double n = 0;
	s_coord sun_pos = celestial(JD2, lat, lng, n, 25, -8.0);

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

	// Rendering loop
	n = 0;
	double increment = 1;
	int j = 0;
	double x = 1.0;
	double y = 1.0;

	FILE* file;
	file = fopen("out.txt","a+");

	// Kinda useless thread setup!

	struct Arg* args = malloc(sizeof args);
	args->x_offset = 0;
	args->y_offset = 0;
	args->lat = lat;
	args->a = 1;

	pthread_t threads[5];
	int rc;
	rc = pthread_create(&threads[0], NULL, draw_ticks, args);
	if (rc){
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	}
	
	refresh();
	sun_pos = celestial(JD2, lat, lng, n, 8.0, tz);
	point_f sun_pos_point = s_coord_to_point(&sun_pos);
	graph_info g = get_graph_info(JD2, lat, lng, 1.0, tz);

	while(1) {
		clear();

		screen_info screen = get_screen_info();

		double half_width = screen.width / 2.0;
		double y_midpoint = floor(screen.height/3.0);

		sun_pos = celestial(JD2, lat, lng, n, 12, tz);
		n+=increment;
		if(n > 24*60) n = 0.01;

		double midpoint = g.midpoint;
		scale_info s;
		point_f sun_pos_coord = s_coord_to_point(&sun_pos);
		s = scale_sun_pos(&sun_pos_coord, g.midpoint);

		double shadow_length = screen.height/3.0;
		point_f spoint = shadow_point(&sun_pos_coord, shadow_length, s.midpoint, y_midpoint);

		int x = ceil(spoint.y);
		int y = ceil(spoint.x);
		
		fprintf(file, "sc shadow x: %i\t", y);
		fprintf(file, "sc shadow y: %i\n", x);
		
		draw_line(s.midpoint, screen.height/3.0, y, x, 'x'); // this one is right, i think

		mvaddch(x,y, 'o');
		mvaddch(floor(sun_pos_coord.y), floor(sun_pos_coord.x), '5');
		// draw_filled_circle(floor(sun_pos_coord.y),floor(sun_pos_coord.x), floor(screen.height/18), '&');

		j++;

		usleep(500);
		// usleep(50000);

		refresh();
		// sleep(30);
	}
	fclose(file);
	endwin(); // clear ncurses's junk
	pthread_exit(NULL);
}