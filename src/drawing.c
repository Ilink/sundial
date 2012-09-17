#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include "types.h"
#include "drawing.h"
#include "sun.h"

// use this instead of accessing the terminal size directly
// the ratios are kind of optional
screen_info get_screen_info(){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	double x_ratio = (double) w.ws_col / 360.0;
	double y_ratio = (double) w.ws_row / 360.0;

	screen_info screen;
	screen.height = w.ws_row;
	screen.width = w.ws_col;
	screen.scale_x = x_ratio / 1.4;
	screen.scale_y = y_ratio / 1.4;
	return screen;
}

point_f tick_point(double hla, int shadow_length){
	point_f pos;
	pos.y = 20+sin(hla)*shadow_length;
	pos.x = cos(hla)*shadow_length;

	return pos;
}

double fit_bound(double min, double max, double x, double min_o, double max_o){
	return (max - min) * (x-min) / (max_o - min_o) + min;
}

// expects: int x_offset, int y_offset, int lat, int a
// see Arg in header
void* draw_ticks(void* o_args){
	struct Arg* args = (struct Arg*) o_args;
	FILE *file;
	file = fopen("ticks.txt","a+");
	
	while(1){
		int hours[] = {6,7,8,9,10,11,12,13,14,15,16,17,18};

		int* iter;
		int size = sizeof(hours) / sizeof(int);

		screen_info screen = get_screen_info();
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
			fprintf(file, "offset: %i\t\t", args->x_offset);

			int x = args->x_offset+12+ceil(tick.x*screen.scale_x*3);
			int y = half_width-(args->y_offset*2.25)+ceil(tick.y*screen.scale_y*10);
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
}

point_f shadow_point(point_f* sun_pos, int shadow_length, double midpoint, double y_midpoint){
	FILE *file; 
	file = fopen("shadow2.txt","a+");

	// convert to relative coordinates
	double rel_x = sun_pos->x - midpoint;
	double rel_y = sun_pos->y - y_midpoint;

	double angle = atan2(rel_y,rel_x);

	double x;
	if(angle >= PI/2){
		x = cos(-1*angle+PI) * shadow_length + midpoint;
	} else {
		x = cos(angle+PI) * shadow_length + midpoint;
	}
	double y = sin(-1*angle) * shadow_length + y_midpoint;

	fprintf(file, "y: %f\t", y);
	fprintf(file, "x: %f\t", x);
	fprintf(file, "angle (deg): %f\t", angle* (180/PI));
	fprintf(file, "angle (rad): %f\t", angle);
	fprintf(file, "rel_x: %f\t",rel_x);
	fprintf(file, "rel_y: %f\n", rel_y);

	point_f p;
	p.x = x;
	p.y = y;
	fclose(file);
	return p;
}

scale_info scale_sun_pos(point_f* coord, double midpoint){
	FILE *file; 
	file = fopen("out.txt","a+");  
	
	screen_info screen = get_screen_info();

	fprintf(file,"unscaled x: %f\t unscaled y: %f \n", coord->y, coord->x); 

	double y_ratio = screen.height / 500.0;
	double x_ratio = screen.width / 1000.0;

	double half_width = screen.width / 2.0;
	double half_height = screen.height / 2.0;

	double true_offset = midpoint*x_ratio+half_width;
	midpoint = half_width;

	coord->x = -1*(coord->x * x_ratio) + floor(true_offset);
	coord->y = -1*(coord->y * y_ratio) + floor(half_height/3);

	
	fprintf(file,"midpoint (x): %f\t", midpoint); 
	fprintf(file,"scaled x: %f\t", coord->y); 
	fprintf(file,"scaled y: %f\n", coord->x); 
	
	fprintf(file,"width console: %i\t", screen.width);
	fprintf(file, "height console: %i\n\n", screen.height);

	fclose(file);
	scale_info s;
	s.midpoint = midpoint;
	s.x_scale = x_ratio;
	s.y_scale = y_ratio;
	return s;
}

/*
This is pretty slow. Is O(N), could be Log(n) without too much effort.
However, it is only used once.
*/
graph_info get_graph_info(double jd, double lat, double lng, double precision, double tz){
	double midpoint;	
	double hour = 5.5;
	double highest = 0;
	s_coord graph;
	graph_info g;
	int first = 1;

	FILE* file;
	file = fopen("graph_info.txt","w+");

	while(1){
		graph = celestial(jd, lat, lng, hour, tz);
		if(first){
			first = 0;
			highest = graph.elevation;
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
		hour += precision;
	}

	fclose(file);
	return g;
}

void scale_ticks(){

}

void scale_shadow(){
	
}