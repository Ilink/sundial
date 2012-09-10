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

void draw_ticks(int x_offset, int y_offset, int lat, int shadow_length){
	FILE *file;
	file = fopen("out.txt","a+");
	

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
		// fprintf(file,"ha: %f\t", ha);
		// fprintf(file,"hla: %f\t", hla);
		// fprintf(file,"xform: %f\t", xform);
		// fprintf(file, "hour: %i\t", *iter);
		// fprintf(file, "x: %f\t", tick.x);
		// fprintf(file, "y: %f\t", tick.y);
		// fprintf(file, "x: %f\t", f.x);
		// fprintf(file, "y: %f\n", f.y);

		mvaddch(x_offset+ceil(tick.x*f.x*3), y_offset+ceil(tick.y*f.y*10), '*');
	}
	fclose(file);
}

point_f shadow_point (s_coord2* sun_pos, int shadow_length){
	double angle = atan2(sun_pos->elevation, sun_pos->azimuth);
	FILE *file; 
	file = fopen("out.txt","a+");
	fprintf(file, "ele: %f\t", sun_pos->elevation);
	fprintf(file, "azi: %f\t", sun_pos->azimuth);
	fprintf(file, "shadow angle: %f\n", rad_to_deg(angle));
  
	fclose(file);
	if(angle > PI/2.0){
		// angle -= PI/2.0;
	}
	
	point_f p;
	p.x = sin(angle) * shadow_length;
	p.y = cos(angle) * shadow_length;
	return p;
}

int main(){
	double lat = 37.9232; double lng = -122.2937; double tz = -8.0;
	remove("out.txt");
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

	console_scale(&sun_pos);

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
		console_scale(&sun_pos);
		double hour = get_local()+n;
		printf("azimuth: %f", sun_pos.azimuth);
		printf("\televation: %f", sun_pos.elevation);
		printf("\thour: %f\n", hour);
		
		n+=n;

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
	while(1) {
		// break;
		s_coord2 sun_pos = celestial(JD2, lat, lng, n, 25, tz);
		n+=increment;
		if(n > 24*60) n = 0.01;
		// graph_info g = get_graph_info(JD2, lat, lng, 1.0, tz);
		// fprintf(file, 'highest: %f\t', );
		console_scale(&sun_pos);


		point_f spoint = shadow_point(&sun_pos, 1);

		int x = ceil(spoint.x * 30+20);
		int y = ceil(spoint.y * 30);
		
		fprintf(file, "sc shadow x: %i\t", x);
		fprintf(file, "sc shadow y: %i\n", y);
		

		// draw_line(10, 10, ceil(spoint.y*20.0+30), ceil(spoint.x*20.0+30), 'o');

		mvaddch(y,x, 'o');
		mvaddch(floor(sun_pos.elevation), floor(sun_pos.azimuth), main_char);

		j++;

		usleep(2000);
		refresh();
	}
	fclose(file);
	endwin(); // clear ncurses's junk
}