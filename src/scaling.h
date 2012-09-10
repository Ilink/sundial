#ifndef SCALING_H
#define SCALING_H

typedef struct {
	double midpoint;
} graph_info;

void console_scale(s_coord2* coord, double midpoint);
graph_info get_graph_info(double jd, double lat, double lng, double precision, double tz);

#endif