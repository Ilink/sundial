#ifndef GET_LOC_H
#define GET_LOC_H

typedef struct {
	double lat;
	double lng;
} geo_coord;

geo_coord get_loc(char* ip);


#endif