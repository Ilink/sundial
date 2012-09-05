// the typedef should go in the header
typedef struct {
	int x; 
	int y;
} point;

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