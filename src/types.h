#ifndef TYPES_H
#define TYPES_H

extern double const PI;
extern double const jd2k;
#define printf(fmt, ...) (0)
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
	double delta;
	double alpha;
	double r;
} celestial_coord;

typedef struct {
	double lat;
	double lng;
} ecliptic_coord;

typedef struct {
	double x;
	double y;
	double z;
} point_3d;

typedef struct {
	double zenith;
	double altitude;
	double r;
} s_coord;

typedef struct {
	double azimuth;
	double r;
	double elevation;
} s_coord2;

typedef struct {
	double x;
	double y;
} scale_factor;

#endif