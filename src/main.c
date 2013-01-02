#include <ncurses/ncurses.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <getopt.h>

// #include "get_loc.h"
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
	double lat; double lng; double tz;
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

	remove("../log/out.txt");
	FILE* file;
	file = fopen("../log/out.txt","a+");

	geo_coord loc;
	if(required_arguments.lat && required_arguments.lng){
		printf("ALL SYSTEMS ARE GO");
	} else {
		const char ip[50] = "98.210.146.154";

		// fprintf(file, "IP: %s", ip);
		// loc = get_loc(ip);
		loc.lat = 37.871592;
		loc.lng = -122.272747;
		lat = 37.871592; lng = -122.2937;
	}

	remove("../log/shadow.txt");
	remove("../log/shadow2.txt");
	remove("../log/scaling.txt");
	remove("../log/ticks.txt");
	remove("../log/time.txt");
	remove("../log/JD.txt");

	time_t rawtime;
	struct tm *ptm;

	time(&rawtime);
	ptm = localtime(&rawtime);

	tz = -8.0;
	int year = 1900 + ptm->tm_year;
	int day = ptm->tm_mday;
	int month = ptm->tm_mon+1;

	// fprintf(file, "year: %i\t day: %i\t month: %i\n", year, day, month);

	double JD = get_jd(year, month, day);
	// fprintf(file, "JD: %f\n\n", JD);

	// Initialize ncurses
	initscr();
	clear();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0);

	// Kinda useless thread setup!
	struct Arg* args = malloc(sizeof args);
	args->x_offset = 0;
	args->y_offset = 0;
	args->lat = lat;
	args->a = 1;

	// pthread_t threads[5];
	// int rc;
	// rc = pthread_create(&threads[0], NULL, draw_ticks, args);
	// if (rc){
	// 	printf("ERROR; return code from pthread_create() is %d\n", rc);
	// 	exit(-1);
	// }

	graph_info g = get_graph_info(JD, lat, lng, 1, tz);
	double n = 0;
	double increment = 0.1;
	while(1) {
		clear();

		double hour = get_local();
		hour = 18.0 * 60.0;
		fprintf(file, "hour (local*60) %f\t", hour);

		screen_info screen = get_screen_info(); // allows resizing of the window by keeping this up to date
		double half_width = screen.width / 2.0;
		double y_midpoint = floor(screen.height/3.0);
		s_coord sun_pos = celestial(JD, lat, lng , hour, tz);
		
		n+=increment;
		if(n > 24*60) n = 0;
		fprintf(file, "azi: %f\t ele: %f\t n: %f\n", sun_pos.azimuth, sun_pos.elevation, n);

		double midpoint = g.midpoint;
		point_f sun_pos_coord = s_coord_to_point(&sun_pos);
		scale_info s = scale_sun_pos(&sun_pos_coord, g.midpoint);

		double shadow_length = screen.height/3.0;
		point_f spoint = shadow_point(&sun_pos_coord, shadow_length, s.midpoint, y_midpoint);

		int x = ceil(spoint.y);
		int y = ceil(spoint.x);
		
		// fprintf(file, "sc shadow x: %i sc shadow y: %i\n", y, x);
		// fprintf(file, "x: %f\t y: %f\n", floor(sun_pos_coord.y), floor(sun_pos_coord.x));
		
		draw_line(s.midpoint, screen.height/3.0, y, x, 'x');

		mvaddch(ceil(x),ceil(y), 'o');
		// mvaddch(floor(sun_pos_coord.y), floor(sun_pos_coord.x), '5');
		draw_filled_circle(floor(sun_pos_coord.y),floor(sun_pos_coord.x), floor(screen.height/18), '&');

		usleep(5000);
		refresh();
	}
	fclose(file);
	endwin(); // clear ncurses's junk
	// pthread_exit(NULL);
}