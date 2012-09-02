#include <ncurses.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "main.h"

double const PI = atan(1)*4;
double jd2k = 2451545.0;
// int month_days[] = {0,31,28,31,30,31,30,31,31,30,31,30};

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

double deg_to_rad(double x){
	return PI * x / 180.0;
}

double rad_to_deg(double x){
	return 180.0 * x / PI;
}

// uses UT, not local time
double get_jd(int year, int month, int day){
	double UT;
	UT = get_ut();

	time_t t = time(NULL);
	struct tm *now = localtime(&t);
	int day_of_year = now->tm_yday;

	int a = floor(year/100.0);
	int b = 2 - a + floor(a/4.0);
	return floor(365.25*(year + 4716))+ floor(30.6001*(month+1))+day+b-1524.5;
}

int int_part(double x){
	return abs(x)*((int)x >> 31) | (((0 - (int)x) >> 31) & 1);
}

double range_deg(double x){
	double b = x / 360;
	double a = 360 * (b-int_part(b));
	if(a < 0) a += 360;
	return a;
}

// returns degrees
double calc_obliq_corr(double t){
	double seconds = 21.448 - t*(46.8150 + t*(0.00059 - t*(0.001813)));
	double e0 = 23.0 + (26.0 + (seconds/60.0))/60.0;
	double omega = 125.04 - 1934.136 * t;
	return e0 + 0.00256 * cos(deg_to_rad(omega));
}

// returns degrees
double calc_sun_center(double t) {
	double m = 357.52911 + t * (35999.05029 - 0.0001537 * t);
	double mrad = deg_to_rad(m);
	double sinm = sin(mrad);
	double sin2m = sin(mrad+mrad);
	double sin3m = sin(mrad+mrad+mrad);
	return sinm * (1.914602 - t * (0.004817 + 0.000014 * t)) + sin2m * (0.019993 - 0.000101 * t) + sin3m * 0.000289;
}

// returns degrees
double calc_mean_lng_sun(double t){
	double L0 = 280.46646 + t * (36000.76983 + t*(0.0003032));
	while(L0 > 360.0) L0 -= 360.0;
	while(L0 < 0.0) L0 += 360.0;
	return L0;
}

// returns degrees
double calc_sun_app_lng(double t){
	double l0 = calc_mean_lng_sun(t);
	double c = calc_sun_center(t);
	double o = l0 + c;
	double omega = 125.04 - 1934.136 * t;
	double lambda = o - 0.00569 - 0.00478 * sin(deg_to_rad(omega));
	return lambda;
}

double calc_earth_ecc(double t){
	return 0.016708634 - t * (0.000042037 + 0.0000001267 * t);
}

// returns AUs
double calc_sun_rad_vector(double t) {
	double l0 = calc_mean_lng_sun(t);
	double c = calc_sun_center(t);
	double v = l0 + c;
	double e = calc_earth_ecc(t);
	return (1.000001018 * (1 - e * e)) / (1 + e * cos(deg_to_rad(v)));
}

double calc_mean_anomaly_sun(double t){
	return 357.52911 + t * (35999.05029 - 0.0001537 * t);
}

// returns degrees
double calc_declination(double t){
	double e = calc_obliq_corr(t);
	double lambda = calc_sun_app_lng(t);

	return rad_to_deg(asin(sin(deg_to_rad(e)) * sin(deg_to_rad(lambda))));
}

double calc_eq_time(double t){
	double epsilon = calc_obliq_corr(t);
	printf("epsilon: %f\n", epsilon);
	double l = calc_mean_lng_sun(t);
	printf("l: %f\n", l);
	double e = 0.016708634 - t * (0.000042037 + 0.0000001267 * t);
	double m = calc_mean_anomaly_sun(t);


	double y = tan(deg_to_rad(epsilon)/2.0);
	y *= y;

	double sin2l0 = sin(2.0 * deg_to_rad(l));
	double sinm   = sin(deg_to_rad(m));
	double cos2l0 = cos(2.0 * deg_to_rad(l));
	double sin4l0 = sin(4.0 * deg_to_rad(l));
	double sin2m  = sin(2.0 * deg_to_rad(m));

	double Etime = y * sin2l0 - 2.0 * e * sinm + 4.0 * e * y * sinm * cos2l0 - 0.5 * y * y * sin4l0 - 1.25 * e * e * sin2m;
	return rad_to_deg(Etime)*4.0; // in minutes of time
}

s_coord2 celestial(double jd, double lat, double lng){
	double hour = get_ut();
	hour = 12;
	printf("Hour: %f\n", hour);
	double tz = -8.0; // todo: un-hardcode

	double j2k = get_jd(2000, 1, 1);
	double time;
	time = jd + hour/1440.0 - tz/24.0;
	time = (time - 2451545.0)/36525.0;
	printf("time: %f\n", time);

	double eqtime = calc_eq_time(time);
	printf("eqtime: %f\n", eqtime);

	double delta = calc_declination(time);
	printf("delta: %f\n", delta);
	double true_solar_time = hour + eqtime + 4.0 * lng - 60.0 * tz;
	while(true_solar_time > 1440) true_solar_time -= 1440;
	double ha = true_solar_time / 4.0 - 180.0;
	if(ha < -180) ha += 360.0;

	printf("ha: %f\n", ha);

	double r = calc_sun_rad_vector(time);
	double ha_rad = deg_to_rad(ha);
	double cos_zenith = sin(deg_to_rad(lat)) * sin(deg_to_rad(delta)) + cos(deg_to_rad(lat)) * cos(deg_to_rad(delta)) * cos(ha_rad);
	if(cos_zenith > 1.0) cos_zenith = 1.0;
	else if (cos_zenith < -1.0) cos_zenith = -1.0;
	double zenith = rad_to_deg(acos(cos_zenith));

	double azimuth_denom = cos(deg_to_rad(lat))*sin(deg_to_rad(zenith));
	double azimuth;
	if(abs(azimuth_denom) > 0.001){
		double azimuth_rad = ((sin(deg_to_rad(lat)) * cos(deg_to_rad(zenith))) - sin(deg_to_rad(delta))) / azimuth_denom;
		if(abs(azimuth_rad)>1.0) {
			if(azimuth_rad < 0) azimuth_rad = -1.0;
			else azimuth_rad = 1.0;
		}
		azimuth = 180.0 - rad_to_deg(acos(azimuth_rad));
		if(ha > 0.0) azimuth *= -1;
	} else {
		if(lat > 0.0) azimuth = 180.0;
		else azimuth = 0.0;
	}
	if (azimuth < 0) azimuth += 360.0;

	double ref_corr;
	double eo_ele = 90.0 - zenith;
	if(eo_ele > 85.0){
		ref_corr = 0.0;
	} else {
		double te = tan(deg_to_rad(eo_ele));
		if(eo_ele > 5.0) {
			ref_corr = 1735.0 + eo_ele * (-518.2 + eo_ele * (103.4 + eo_ele * (-12.79 + eo_ele * 0.711)));
		} else {
			ref_corr = -20.774 / te;
		}
		ref_corr /= 3600.0;
	}
	double solar_zenith = zenith - ref_corr;

	s_coord2 coord;
	coord.azimuth = azimuth;
	coord.latitude = lat;
	double el = floor((90.0-solar_zenith)*100+0.5)/100.0;
	coord.elevation = el;

	return coord;
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

	double JD2 = get_jd(2012, 8, 31);
	printf("JD 8/31/12: %f\n", JD2);
	double J2k = get_jd(2000, 1, 1);
	s_coord2 sun_pos = celestial(JD2, 37.9232, 122.2937);

	printf("Latitude: %f\n", sun_pos.latitude);
	printf("Azimuth: %f\n", sun_pos.azimuth);
	printf("Elevation: %f\n", sun_pos.elevation);

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