#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include "types.h"
#include "scaling.h"
#include "sun.h"

void console_scale(s_coord2* coord){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	// these dont really work too well, they should be better
	double x_ratio = (double) w.ws_col / 700.0;
	double y_ratio = (double) w.ws_row / 360.0;

	double half_width = w.ws_col / 2.0;
	// double x_offset = half_width - 

	// double new_azimuth = abs(w.ws_row - coord->azimuth * y_ratio);
	double new_azimuth = -1*(coord->azimuth * y_ratio) + floor(w.ws_col/1.4);
	coord->azimuth *= y_ratio;
	coord->azimuth = new_azimuth;

	// double new_ele = abs(w.ws_col - coord->elevation * x_ratio);
	double new_ele = -1*(coord->elevation * x_ratio) + floor(w.ws_col/7.5);
	coord->elevation *= x_ratio;
	coord->elevation = new_ele;

	FILE *file; 
	file = fopen("out.txt","a+");  
	// fprintf(file,"azi (x): %f\t", new_azimuth); 
	// fprintf(file,"ele (y): %f\n", new_ele); 
	
	// fprintf(file, "col: %f\n", floor(w.ws_col/5.0));

	// fprintf(file,"col console (x): %i\t", w.ws_col);
	// fprintf(file, "row console (y): %i\n\n", w.ws_row);
	fclose(file);
}

graph_info get_graph_info(double jd, double lat, double lng, double precision, double tz){
	double midpoint;	
	int n=0;
	double highest=0;
	s_coord2 graph;
	graph_info g;

	FILE* file;
	file = fopen("graph_info.txt","w+");

	while(1){
		graph = celestial(jd, lat, lng, n, 4.0, tz);
		if(graph.elevation > highest){
			highest = graph.elevation;
			midpoint = graph.azimuth;
		} else {
			g.midpoint = graph.azimuth;
			fprintf(file, 'midpoint: %f\t', g.midpoint);
			break;
		}
		n += precision;
	}

	fclose(file);

	return g;
}