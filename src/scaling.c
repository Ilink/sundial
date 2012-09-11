#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include "types.h"
#include "scaling.h"
#include "sun.h"

void console_scale(s_coord2* coord, double midpoint){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	// these dont really work too well, they should be better
	double x_ratio = (double) w.ws_col / 700.0; // these are named wrong..let's keep it that way
	double y_ratio = (double) w.ws_row / 360.0;

	double half_width = w.ws_col / 2.0;
	// double x_offset = half_width - 

	double true_offset = midpoint*y_ratio+half_width;
	midpoint = true_offset - (y_ratio * midpoint);
	midpoint = half_width;

	// double new_azimuth = abs(w.ws_row - coord->azimuth * y_ratio);
	// double new_azimuth = -1*(coord->azimuth * y_ratio) + floor(w.ws_col/1.4);
	double new_azimuth = -1*(coord->azimuth * y_ratio) + floor(true_offset);
	coord->azimuth = new_azimuth;

	// double new_ele = abs(w.ws_col - coord->elevation * x_ratio);
	// double new_ele = -1*(coord->elevation * x_ratio) + floor(w.ws_col/7.5);
	double new_ele = -1*(coord->elevation * x_ratio * 0.9) + floor(w.ws_col/16.0);
	coord->elevation = new_ele;

	FILE *file; 
	file = fopen("out.txt","a+");  
	fprintf(file,"midpoint (x): %f\t", midpoint); 
	fprintf(file,"scaled azi (x): %f\t", new_azimuth); 
	fprintf(file,"scaled ele (y): %f\n", new_ele); 
	
	// fprintf(file, "col: %f\n", floor(w.ws_col/5.0));

	fprintf(file,"col console (x): %i\t", w.ws_col);
	fprintf(file, "row console (y): %i\n\n", w.ws_row);

	fclose(file);
}

void scaley(double* x, double* y){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	double x_ratio = (double) w.ws_col / 700.0;
	double y_ratio = (double) w.ws_row / 360.0;

	double half_width = w.ws_col / 2.0;
}

graph_info get_graph_info(double jd, double lat, double lng, double precision, double tz){
	double midpoint;	
	int n=0;
	double highest=0;
	s_coord2 graph;
	graph_info g;
	int first = 1;

	FILE* file;
	file = fopen("graph_info.txt","w+");

	while(1){
		graph = celestial(jd, lat, lng, n, 5.5, tz);
		if(first){
			first = 0;
			highest =  graph.elevation;
		}

		fprintf(file, "elevation: %f\t", graph.elevation);
		fprintf(file, "azimuth: %f\n", graph.azimuth);

		if(graph.elevation >= highest){
			
			highest = graph.elevation;
			midpoint = graph.azimuth;
		} else if(graph.elevation > 0){
			g.midpoint = midpoint;
			fprintf(file, "midpoint: %f\t", midpoint);
			fprintf(file, "highest: %f\n", highest);
			break;
		}
		n += precision;
		// usleep(100);
	}

	fclose(file);

	return g;
}