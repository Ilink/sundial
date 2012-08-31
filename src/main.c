#include <ncurses.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "main.h"

double const PI = atan(1)*4;
double jd2k = 2451545.0;

int linear_eq(int y2, int x1, int x2){
	int m = round(y2 / x1);
	int y = m * (round(x2 - x1)) + y2;
	return y;
}

void draw_line(int x0, int y0, int x1, int y1, char render_char){
	int dx = abs(x1-x0);
	int dy = abs(y1-y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;

	// this examines how steep the slope is
	int err = dx - dy;

	for(;;){
		mvaddch(y0, x0, render_char);
		if(x0 == x1 && y0 == y1) break;
		int e2 = 2*err; 

		// i think this examines whether or not the slope is steep enough to have Y be the driving force. A steep line should iterate over Y instead of X.
		if(e2 > -dy){
			err -= dy; // reset the accumulation to 0
			x0 += sx;
		} else if(e2 < dx){
			err += dx; // accumulate more fractions
			y0 += sy;
		}
	}
}

// returns hour + fraction
double get_ut(){
	time_t rawtime;
	struct tm *ptm;

	time(&rawtime);
	ptm = gmtime(&rawtime);

	double sec = ptm->tm_sec / 3600.0;
	double min = ptm->tm_min / 60.0;
	double time = ptm->tm_hour + sec + min;

	return time;
}

// uses UT, not local time
double get_jd(int year, int month, int day){
	double UT;
	UT = get_ut();

	int a = (month+9)/12;
	int b = (7*(year+a))/4;
	int c = 275 * month / 9;
	int delta = year - 1949;
	return 32916.5 + delta * 365 + day + UT / 24;
	// return 367*year - b + c + day + 1721013.5 + UT/24;
}

// ecliptic_coord sun_pos(double jd){
// 	ecliptic_coord coord;
// 	double n = jd - jd2k;
// 	// all in degrees
// 	double L = 280.460 + 0.9856474*n; // mean ecliptic lng of the sun
// 	while(L < 360){
// 		L += 360;
// 	}
// 	double g = 357.528 + 0.9856003*n; // mean anomaly of the sun
// 	while(g < 360){
// 		g += 360;
// 	}
// 	double lambda = L + 1.915 * sin(g) + 0.020 * sin(2*g);
// 	double beta = 0; // ecliptic latitude
// 	double R = 1.00014 - 0.01671*cos(g) - 0.00014*cos(2*g);

// 	// double alpha = atan()

// 	coord.lat = 1.0;
// 	coord.lng = 1.0;
// }

s_coord2 celestial(double jd, double lat, double lng){
	double hour = get_ut();
	printf("Hour: %f\n", hour);
	double rads = 180/PI;

	double j2k = get_jd(2000, 1, 1);
	double time;
	time = jd-j2k;
	time = jd-51545;

	printf("Time: %f\n", time);

	double L = 280.460 + .9856474 * time;
	while(L > 360){
		L -= 360;
	}

	printf("L: %f\n", L);

	double g = 357.528 + .9856003 * time;
	while(g > 360){
		g -= 360;
	}

	printf("G: %f\n", g);

	double lambda = L + 1.915 * sin(g) + 0.020 * sin(2*g);
	while(lambda > 360){
		lambda -= 360;
	}

	printf("Lambda: %f\n", L);

	double epsilon = 23.439 - 0.0000004 * time;
	printf("Epsilon: %f\n", epsilon);
	lambda *= rads;
	epsilon *= rads;

	// RA (alpha) and Declination (delta)
	double y = cos(epsilon) * sin(lambda);
	double x = cos(lambda);

	printf("X: %f\n", x);
	printf("Y: %f\n", y);

	double a = atan(y/x);
	printf("A: %f\n", a);
	double alpha;
	if(x < 0) alpha = a + PI;
	if(y < 0 && x > 0) alpha = a + 2*PI;
	else alpha = a;
	printf("Alpha: %f\n", alpha);
	double delta = asin(sin(epsilon)*sin(lambda));

	printf("Delta: %f\n", delta);

	// Local coordinates
	double gmst = 6.697375 + .0657098242 * time + hour;
	while(gmst < 24){
		gmst += 24;
	}
	printf("GMST: %f\n", gmst);

	double lmst = gmst + lng / 15;
	while(lmst > 24){
		lmst -= 24;
	}
	lmst *=  rads;
	printf("LMST: %f\n", lmst);

	double ha = lmst - alpha;
	if(ha < -1*PI) ha += (2*PI);
	if(ha > PI) ha -= (2*PI);

	lat *= rads;

	// azi and elevation
	double el = asin(sin(delta)*sin(lat)+cos(delta)*cos(lat)*cos(ha));
	double az = asin(-1*cos(delta)*sin(ha)/cos(el));

	double cos_az_pos;
	if(0 < sin(delta) - sin(el)*sin(lat)){
		if(sin(az) < 0) az += (2*PI);
	} else {
		az -= PI;
	}

	// el /= rads;
	// az /= rads;
	// lat /= rads;

	s_coord2 coord;
	coord.azimuth = az;
	coord.latitude = lat;
	coord.elevation = el;

	return coord;

	// double R = 1.00014 - 0.01671*cos(g) - 0.00014*cos(2*g);

	// double X = R*cos(lambda);
	// double Y = R*cos(epsilon)*sin(lambda);
	// double Z = R*sin(epsilon)*sin(lambda);

	// celestial_coord coords;
	// coords.delta = delta;
	// coords.alpha = alpha;
	// coords.r = 1.00014 - 0.01671*cos(g) - 0.00014*cos(2*g);

	// point_3d point;
	// point.x = X;
	// point.y = Y;
	// point.z = Z;
	// double X = R*cos(lambda);
	// double Y = R*cos(epsilon)*sin(lambda);
	// double Z = R*sin(epsilon)*sin(lambda);
	
	// return point;
}

s_coord spherical(int day, int hour, int minute, int sec, double timezone, double lat, double lng){

	double frac_year = (2*PI/365)*(day-1+((hour-12)/24));

	printf("%f\n", frac_year);

	double eqtime = 229.18*(0.000075+0.001868*cos(frac_year)-1.0*0.032077*sin(frac_year)-0.014615*cos(2.0*frac_year)-0.040849*sin(2.0*frac_year));

	printf("%f\n", eqtime);
	eqtime = -0.21;

	double decl = 0.006918 - 0.399912*cos(frac_year)+0.070257*sin(frac_year) - 0.006758*cos(2*frac_year) + 0.000907*sin(2*frac_year)-0.002697*cos(3*frac_year)+0.00148*sin(3*frac_year);
	printf("%f\n", decl);


	double time_offset = eqtime-4*lng + 60*timezone;
	double tst = hour*60+minute+sec/60+time_offset;
	double ha = (tst/4) - 180;

	double zenith = acos(sin(lat)*sin(decl)+cos(lat)*cos(decl)*cos(ha));
	double azimuth = -1*(sin(lat)*cos(zenith)-sin(decl)) / cos(lat)*sin(zenith);
	

	s_coord coord;
	coord.zenith = zenith;
	coord.altitude = azimuth;
	coord.r = 1.0;
	return coord;
}


int main(){

	point a;
	a.x = 5;
	a.y = 5;

	point b;
	b.x = 0;
	b.y = 0;

	double degs = 180.0 / PI;

	double JD2 = get_jd(2012, 8, 31);
	double J2k = get_jd(2000, 1, 1);
	s_coord2 sun_pos = celestial(JD2, 37.9232, 122.2937);

	// printf("%f\n", J2k);
	printf("%f\n", JD2);
	// printf("%f\n", JD2-J2k);
	// printf("%f\n", get_ut());

	// printf("%f\n", sun_pos.x);
	// printf("%f\n", sun_pos.y);
	// printf("%f\n", sun_pos.z);

	printf("Latitude: %f\n", sun_pos.latitude * degs);
	printf("Azimuth: %f\n", sun_pos.azimuth*degs);
	printf("Elevation: %f\n", sun_pos.elevation*degs);

	// celestial_coord test = celestial()

	// // Initialize ncurses
	// initscr();
	// clear();
	// noecho();
	// cbreak();
	// keypad(stdscr, TRUE);
	// curs_set(0);	

	// // test symbol
	// char main_char = '@';
	// char ch;

	// // Rendering loop

	// draw_line(1,1,20,10, '@');
	// refresh();
	// while(1) {
	// 	ch = getch();
	// 	if(ch == 'q' || ch == 'Q') {
	// 		break;
	// 	}
	// }
	// endwin(); // clear ncurses's junk
}