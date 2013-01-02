#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "types.h"
#include "sun.h"
#include "util.h"

static const char *opt_string = "l:n:h";
static struct option long_options[] = {
	{"lat", required_argument, 0, 'l'},
	{"lng", required_argument, 0, 'n'},
	{"help", no_argument, NULL, 'h'},
	{NULL, no_argument, NULL, 0}
};

static struct Required_arguments {
	int lat;
	int lng;
} required_arguments;

int main(int argc, char *argv[]){
	int c;
	double lat; double lng; double tz;
	while(1){
		int option_index = 0;
		c = getopt_long (argc, argv, opt_string, long_options, &option_index);
		if(c == -1) break;

		switch (c){
			case 'l':
				lat = strtod(optarg, NULL);
				required_arguments.lat = 1;
				break;
			case 'n':
				lng = strtod(optarg, NULL);
				required_arguments.lng = 1;
				break;
			case 'h':
				break;
			default:
				break;
		}
	}

	remove("../log/out.txt");
	FILE* file;
	file = fopen("../log/out.txt","a+");

	geo_coord loc;
	if(required_arguments.lat && required_arguments.lng){
		printf("ALL SYSTEMS ARE GO");
	} else {
		loc.lat = 37.871592;
		loc.lng = -122.272747;
		lat = 37.871592; lng = -122.2937;
	}

	remove("../log/shadow.txt");
	remove("../log/shadow2.txt");
	remove("../log/scaling.txt");
	remove("../log/ticks.txt");
	remove("../log/time.txt");
	remove("../log/JD.txt");

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