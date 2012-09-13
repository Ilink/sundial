#ifndef TYPES_H
#define TYPES_H

extern double const PI;
extern double const jd2k;
#define printf(fmt, ...) (0) // shouldnt be printing to the screen with ncurses going on
// #define fprintf(fmt,...) (0)

typedef struct {
	int x; 
	int y;
} point;

typedef struct {
	double x; 
	double y;
} point_f;

typedef struct {
	double lat;
	double lng;
} ecliptic_coord;

typedef struct {
	double azimuth;
	double r;
	double elevation;
} s_coord;

typedef struct {
	double x;
	double y;
} scale_factor;

typedef struct{
	double midpoint;
} scale_stuff;

typedef struct{
	double midpoint;
	double x_scale;
	double y_scale;
} scale_info;

#endif