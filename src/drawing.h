#ifndef DRAWING_H
#define DRAWING_H

typedef struct {
	double midpoint;
} graph_info;

// argument for tick-rendering thread
struct Arg {
	int x_offset;
	int y_offset;
	int lat;
	int a;
};

// scale_stuff console_scale(s_coord* coord, double midpoint);
scale_info scale_sun_pos(point_f* coord, double midpoint);
screen_info get_screen_info();
void* draw_ticks(void* o_args);
graph_info get_graph_info(double jd, double lat, double lng, double precision, double tz);
point_f shadow_point(point_f* sun_pos, int shadow_length, double midpoint, double y_midpoint);

#endif