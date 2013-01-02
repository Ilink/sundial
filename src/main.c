#include <stdio.h>
// #include <math.h>
#include <time.h>
// #include <stdlib.h>
// #include <unistd.h>

#include "types.h"
#include "sun.h"

int main(int argc, char *argv[]){
	int c;
	double lat; double lng; double tz;

	// remove("../log/shadow.txt");
	// remove("../log/shadow2.txt");
	// remove("../log/scaling.txt");
	// remove("../log/ticks.txt");
	// remove("../log/time.txt");
	// remove("../log/JD.txt");
	remove("../log/out.txt");

	FILE* file;
	file = fopen("../log/out.txt","a+");

	geo_coord loc;

	loc.lat = 37.871592;
	loc.lng = -122.272747;
	lat = 37.871592; lng = -122.2937;

	time_t rawtime;
	struct tm *ptm;

	time(&rawtime);
	ptm = localtime(&rawtime);

	tz = -8.0;
	int year = 1900 + ptm->tm_year;
	int day = ptm->tm_mday;
	int month = ptm->tm_mon+1;
	double JD = get_jd(year, month, day);

	double time = get_local();
	fprintf(file, "hour (local*60) %f\t", time);
	s_coord sun_pos = get_sun_pos(JD, lat, lng , time, tz);
	
	fprintf(file, "azi: %f\t ele: %f\n", sun_pos.azimuth, sun_pos.elevation);
	fclose(file);
}