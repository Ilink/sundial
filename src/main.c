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
#include "main.h"

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

void draw_ticks(int x_offset, int y_offset, int lat, int shadow_length){
	FILE *file;
	file = fopen("ticks.txt","a+");

	int hours[] = {6,7,8,9,10,11,12,13,14,15,16,17,18};

	int* iter;
	int size = sizeof(hours) / sizeof(int);

	scale_factor f = get_scale();

	for(iter = &hours; iter < hours+size; ++iter){
		// fprintf(file,"%i\t", iter); 
		// fprintf(file,"%i\n", *iter);
		double ha = get_ha(*iter);
		double hla = get_hla(lat, ha);
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

		int x = x_offset+ceil(tick.x*f.x*3)+10;
		int y = y_offset+ceil(tick.y*f.y*10);
		fprintf(file, "scaled x: %i\t", x);
		fprintf(file, "scaled y: %i\n", y);

		mvaddch(y,x, '*');
	}
	fclose(file);
}

point_f shadow_point (s_coord2* sun_pos, int shadow_length, double midpoint){
	double angle = atan2(sun_pos->elevation, sun_pos->azimuth) *1.5;
	FILE *file; 
	file = fopen("shadow.txt","a+");
	shadow_length = 3;
  
	point_f p;
	if(sun_pos->azimuth >= midpoint){
		// angle += PI/2.0;
		// fprintf(file, "over midpoint: %f\t", sun_pos->azimuth);
		// p.x = sin(angle)*(shadow_length+PI/2.0);
		p.x = sin(-1*angle) * shadow_length;
		// p.x = sin(angle + PI/2) * shadow_length;
	} else {
		p.x = sin(angle) * shadow_length;
	}

	
	// p.x = sin(-1*angle) * shadow_length;
	p.y = cos(angle) * shadow_length;

	if(sun_pos->azimuth >= midpoint){
		p.x = 10 * (p.x-0.01)+10;
	} else {
		p.x = 10 * (p.x-0.01)+9;
	}
	p.y = 10 * (p.y-2.8) / 0.15;


	// p.x = sun_pos->azimuth;
	// p.y = sun_pos->elevation *-1.0 +49;

	fprintf(file, "s.angle rad: %f\t", angle);
	fprintf(file, "s.angle deg: %f\t", (180.0 * angle) / PI);
	fprintf(file, "x: %f\t", sun_pos->azimuth);
	fprintf(file, "s.y: %f\t", p.y);
	fprintf(file, "s.x: %f\n", p.x);

	fclose(file);
	return p;
}

int main(){
	double lat = 37.9232; double lng = -122.2937; double tz = -8.0;
	remove("out.txt");
	remove("shadow.txt");
	remove("scaling.txt");
	remove("ticks.txt");
	point a;
	a.x = 5;
	a.y = 5;

	point b;
	b.x = 0;
	b.y = 0;

	double JD2 = get_jd(2012, 8, 31);
	printf("JD 8/31/12: %f\n", JD2);
	double J2k = get_jd(2000, 1, 1);
	double n = 0;
	s_coord2 sun_pos = celestial(JD2, 37.9232, -122.2937, n, 25, -8.0);

	printf("Radial dist: %f\n", sun_pos.r);
	printf("Azimuth: %f\n", sun_pos.azimuth);
	printf("Elevation: %f\n", sun_pos.elevation);

	// console_scale(&sun_pos);

	printf("Scaled Azimuth: %f\n", sun_pos.azimuth);
	printf("Scaled Elevation: %f\n", sun_pos.elevation);

	// test area
	// draw_ticks(0,0, lat, 1);
	// end

	// sun_pos.r = au_to_km(sun_pos.r);

	// point_3d sun_pos_cart = spherical_to_cart(&sun_pos);

	// printf("X: %f\n", sun_pos_cart.x);
	// printf("Y: %f\n", sun_pos_cart.y);
	// printf("Z: %f\n", sun_pos_cart.z);

	n = 1/60;
	while(1){
		break;
		s_coord2 sun_pos = celestial(JD2, lat, lng, n, 25, -8.0);
		// console_scale(&sun_pos);
		double hour = get_local()+n;
		printf("azimuth: %f", sun_pos.azimuth);
		printf("\televation: %f", sun_pos.elevation);
		printf("\thour: %f\n", hour);
		
		n += n;

		if(n > 24*60) n = 1/60;
		sleep(1);
	}


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
	draw_ticks(0,0, lat, 1);

	FILE* file;
	file = fopen("out.txt","a+");
	mvaddch(0, 0, 'o');
	refresh();
	sun_pos = celestial(JD2, lat, lng, n, 25, tz);
	graph_info g = get_graph_info(JD2, lat, lng, 1.0, tz);

	while(1) {
		// break;
		sun_pos = celestial(JD2, lat, lng, n, 25, tz);
		n+=increment;
		if(n > 24*60) n = 0.01;
		// graph_info g = get_graph_info(JD2, lat, lng, 1.0, tz);
		// fprintf(file, 'highest: %f\t', );
		// console_scale(&sun_pos, g.midpoint);
		double midpoint = g.midpoint;
		scale_stuff s;
		s = console_scale(&sun_pos, g.midpoint);

		fprintf(file, "midpoint (weird) x: %f\t", s.midpoint);
		fprintf(file, "midpoint x: %f\t", 86.5);
		double test = 86.500000;
		point_f spoint = shadow_point(&sun_pos, 1, s.midpoint);

		// int x = ceil(spoint.x);
		// int y = ceil(spoint.y);

		int x = ceil(spoint.x);
		int y = ceil(spoint.y);
		
		fprintf(file, "sc shadow x: %i\t", y);
		fprintf(file, "sc shadow y: %i\n", x);
		

		draw_line(70, 25, ceil(spoint.y), ceil(spoint.x), 'x');
		// draw_line(10, 10, 20, 20, 'X');


		mvaddch(y,x, 'o');
		mvaddch(floor(sun_pos.elevation), floor(sun_pos.azimuth), main_char);

		j++;

		usleep(8000);
		refresh();
	}
	fclose(file);
	endwin(); // clear ncurses's junk
}