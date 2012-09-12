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
#include "scaling.h"
#include "util.h"
#include "main.h"

struct Arg {
	int x_offset;
	int y_offset;
	int lat;
	int a;
};

scale_factor get_scale(){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	double x_ratio = (double) w.ws_col / 360.0;
	double y_ratio = (double) w.ws_row / 360.0;

	scale_factor f;
	f.x = x_ratio / 1.4;
	f.y = y_ratio / 1.4;
	return f;
}

point_f tick_point(double hla, int shadow_length){
	point_f pos;
	pos.y = 20+sin(hla)*shadow_length;
	pos.x = cos(hla)*shadow_length;

	return pos;
}

double fit_bound(double min, double max, double x){
	return 1.0;
}

double get_hla(double lat, double ha){
	return atan(cos(lat)*tan(ha*(PI/180)));
}

double get_ha(int hour){
	(hour - 12) * 15;
	return (hour - 12) * 15;
}

// expects: int x_offset, int y_offset, int lat, int a
void* draw_ticks(void* o_args){
	struct Arg* args = (struct Arg*) o_args;
	FILE *file;
	file = fopen("ticks.txt","a+");
	
	while(1){
		int hours[] = {6,7,8,9,10,11,12,13,14,15,16,17,18};

		int* iter;
		int size = sizeof(hours) / sizeof(int);

		scale_factor f = get_scale();
		int first=0; int mid=0;

		struct winsize w;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
		double half_width = w.ws_col / 2.0;

		for(iter = &hours; iter < hours+size; ++iter){
			double ha = get_ha(*iter);
			double hla = get_hla(args->lat, ha);
			double xform = hla * 180 / PI + 90;
			point_f tick = tick_point(hla, 10);
			fprintf(file,"ha: %f\t", ha);
			fprintf(file,"hla: %f\t", hla);
			fprintf(file,"xform: %f\t", xform);
			fprintf(file, "hour: %i\t\t", *iter);
			fprintf(file, "x: %f\t\t", tick.x);
			fprintf(file, "y: %f\t\t", tick.y);
			fprintf(file, "x: %f\t\t", f.x);
			fprintf(file, "y: %f\t\t", f.y);
			fprintf(file, "offset: %i\t\t", args->x_offset);

			int x = args->x_offset+12+ceil(tick.x*f.x*3);
			int y = half_width-(args->y_offset*2.25)+ceil(tick.y*f.y*10);
			fprintf(file, "scaled x: %i\t", x);
			fprintf(file, "scaled y: %i\n", y);

			if(args->a){
				if(*iter == 6){
					first = y;
				}
				if(*iter == 12){
					mid = y;
				}
				mid = abs(mid - first);
			}

			if(!args->a){
				mvaddch(x,y, '*');
			}
		}

		mvaddch(0,0, '*');

		
	}
	pthread_exit(NULL);
	fclose(file);
	// return mid;
}

point_f shadow_point (s_coord2* sun_pos, int shadow_length, double midpoint){

	double angle = atan2(sun_pos->elevation, sun_pos->azimuth) * 1.7;
	FILE *file; 
	file = fopen("shadow.txt","a+");
	shadow_length = 3;
  
	point_f p;
	if(sun_pos->azimuth >= midpoint){
		p.x = sin(-1*angle) * shadow_length;
	} else {
		p.x = sin(angle) * shadow_length;
	}

	p.y = cos(angle) * shadow_length;

	if(sun_pos->azimuth >= midpoint){
		p.x = 10 * (p.x-0.01)+10;
	} else {
		p.x = 10 * (p.x-0.01)+8;
	}
	p.y = 10 * (p.y-2.8) / 0.15;

	fprintf(file, "s.angle rad: %f\t", angle);
	fprintf(file, "s.angle deg: %f\t", (180.0 * angle) / PI);
	fprintf(file, "x: %f\t", sun_pos->azimuth);
	fprintf(file, "s.y: %f\t", p.y);
	fprintf(file, "s.x: %f\n", p.x);

	fclose(file);
	return p;
}

point_f shadow_point2(point_f* sun_pos, int shadow_length, double midpoint, double y_midpoint){
	FILE *file; 
	file = fopen("shadow2.txt","a+");

	// convert to relative coordinates
	double rel_x = sun_pos->x - midpoint;
	double rel_y = sun_pos->y - y_midpoint;

	double angle = atan2(rel_y,rel_x);
	double x = cos(angle - (PI/2)) * shadow_length;
	double y = sin(-1*angle) * shadow_length;

	fprintf(file, "y: %f\t", y);
	fprintf(file, "x: %f\t", x);
	fprintf(file, "angle (deg): %f\t", angle* (180/PI));
	fprintf(file, "angle (rad): %f\n", angle);

	point_f p;
	p.x = x;
	p.y = y;
	fclose(file);
	return p;
}

int main(){
	double lat = 37.9232; double lng = -122.2937; double tz = -8.0;
	remove("out.txt");
	remove("shadow.txt");
	remove("shadow2.txt");
	remove("scaling.txt");
	remove("ticks.txt");

	double JD2 = get_jd(2012, 8, 31);
	printf("JD 8/31/12: %f\n", JD2);
	double J2k = get_jd(2000, 1, 1);
	double n = 0;
	s_coord2 sun_pos = celestial(JD2, 37.9232, -122.2937, n, 25, -8.0);

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
	



	// draw_ticks(offset_x,offset_x, lat, 0);
	// fprintf(file, "offset from first: %i\t", offset_x);

	refresh();
	sun_pos = celestial(JD2, lat, lng, n, 25, tz);
	point_f sun_pos_point = s_coord_to_point(&sun_pos);
	graph_info g = get_graph_info(JD2, lat, lng, 1.0, tz);

	while(1) {
		clear();
		struct winsize w;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

		// int offset_x = draw_ticks(args);

		double x_ratio = (double) w.ws_col / 700.0; // these are named wrong..let's keep it that way
		double y_ratio = (double) w.ws_row / 360.0;
		double half_width = w.ws_col / 2.0;
		double y_midpoint = floor(w.ws_col/16.0);

		sun_pos = celestial(JD2, lat, lng, n, 25, tz);
		n+=increment;
		if(n > 24*60) n = 0.01;
		// graph_info g = get_graph_info(JD2, lat, lng, 1.0, tz);
		// fprintf(file, 'highest: %f\t', );
		// console_scale(&sun_pos, g.midpoint);
		double midpoint = g.midpoint;
		scale_info s;
		point_f sun_pos_coord = s_coord_to_point(&sun_pos);
		s = console_scale(&sun_pos_coord, g.midpoint);


		fprintf(file, "midpoint (weird) x: %f\t", s.midpoint);
		fprintf(file, "midpoint x: %f\t", 86.5);
		double test = 86.500000;
		// point_f spoint = shadow_point(&sun_pos, 1, s.midpoint);

		double shadow_length = w.ws_row/3.3;
		point_f spoint = shadow_point2(&sun_pos, shadow_length, s.midpoint, y_midpoint);
		

		// int x = ceil(spoint.y+4);
		int x = ceil(spoint.y+ w.ws_row/1.5);
		int y = ceil(spoint.x+s.midpoint-2);

		// int x = ceil(spoint.x + 10);
		// int y = ceil(spoint.y + half_width/2.0);
		
		fprintf(file, "sc shadow x: %i\t", y);
		fprintf(file, "sc shadow y: %i\n", x);
		

		draw_line(s.midpoint, 25, y, x, 'x'); // this one is right!

		// mvaddch(x,y, 'o');
		mvaddch(floor(sun_pos.elevation), floor(sun_pos.azimuth), main_char);

		j++;

		usleep(5000);
		refresh();
	}
	fclose(file);
	endwin(); // clear ncurses's junk
	pthread_exit(NULL);
}