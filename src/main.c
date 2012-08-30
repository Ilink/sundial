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

double get_ut(){
	time_t rawtime;
	struct tm *ptm;

	time(&rawtime);
	ptm = gmtime(&rawtime);

	double sec = ptm->tm_sec / 60.0 / 60.0;
	double min = ptm->tm_min / 60.0;
	return ptm->tm_hour + sec + min;
}

double get_jd(int year, int month, int day){
	double UT;
	UT = get_ut();

	int a = (month+9)/12;
	int b = (7*(year+a))/4;
	int c = 275 * month / 9;
	return 367*year - b + c + day + 1721013.5 + UT/24;


	// return (double) day-32075+1461*(year+4800+(month-14)/12) /
	// 	4+367*(month-2-(month-14)/12*12)/12-3*((year+4900+(month-14)/12)/100)/4;
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

point_3d celestial(double jd){
	double rads = 180/PI;

	double j2k = get_jd(2000, 1, 1);
	jd = j2k-jd;

	double L = 280.461 * rads + .9856474 * rads * jd;
	while(L < 360){
		L += 360;
	}

	double g = 357.528 * rads + .9856003 * rads * jd;
	while(g < 360){
		g += 360;
	}

	double lambda = L + 1.915 * sin(g) + 0.020 * sin(2*g);
	double epsilon = 23.439 - 0.0000004 * jd;

	// RA (alpha) and Declination (delta)
	double y = cos(epsilon) * sin(lambda);
	double x = cos(lambda);

	double a = atan2(y,x);
	double alpha;
	if(x < 0) alpha = a + 180;
	if(y < 0 && x > 0) alpha = a + 360;
	else alpha = a;

	double delta = asin(sin(epsilon)*sin(lambda));
	double R = 1.00014 - 0.01671*cos(g) - 0.00014*cos(2*g);

	double X = R*cos(lambda);
	double Y = R*cos(epsilon)*sin(lambda);
	double Z = R*sin(epsilon)*sin(lambda);

	celestial_coord coords;
	coords.delta = delta;
	coords.alpha = alpha;
	coords.r = 1.00014 - 0.01671*cos(g) - 0.00014*cos(2*g);

	point_3d point;
	point.x = X;
	point.y = Y;
	point.z = Z;
	// double X = R*cos(lambda);
	// double Y = R*cos(epsilon)*sin(lambda);
	// double Z = R*sin(epsilon)*sin(lambda);
	
	return point;
}

s_coord spherical(int day, int hour, int minute, int second, double timezone){

	double frac_year = 2*PI/365*(day-1+(hour-12)/24);
	double eqtime = 229.18*(0.000075+0.001868*cos(frac_year)-0.032077*sin(frac_year)-0.014615*cos(2*frac_year)-0.040849*sin(2*frac_year));

	double decl = 0.006918 - 0.399912*cos(frac_year)+0.070257*sin(frac_year) - 0.006758*cos(2*frac_year) + 0.000907*sin(2*frac_year)-0.002697*cos(3*frac_year)+0.00148*sin(3*frac_year);

	double time_offset = eqtime-4*lng + 60*timezone;
	double tst = hour*60+minute+sec/60+time_offset;
	double ha = (tst/4) - 180;

	s_coord coord;
	s_coord.zenith = 1.0;
	s_coord.altitude = 1.0;
	s_coord.r = 1.0;
}


double awesome_calc(int year, int month, int day, 
	int _hour, int min, int sec, double lat, double lng){

	double twopi = 2*PI;
	double rad = PI/180;

	int days_month[] = {0,31,28,31,30,31,30,31,31,30,31,30};
	int days = 0;
	// this might be off by a month..use month-1?
	int i;
	for(i=0; i < month; i++){
		days += days_month[i];
	}
	// does not handle leap years like a boss
	days += day;

	double hour = _hour + min/60 + sec/3600;
	int delta = year - 1949;
	double jd = 32916.5 + delta * 365 + day + hour / 24;
	double time = jd - 51545;

	return (double) 1.0;



// 	sunPosition <- function(year, month, day, hour=12, min=0, sec=0,
//                         lat=46.5, long=6.5) {
//   twopi <- 2 * pi
//   deg2rad <- pi / 180

//   # Get day of the year, e.g. Feb 1 = 32, Mar 1 = 61 on leap years
//   month.days <- c(0,31,28,31,30,31,30,31,31,30,31,30)
//   day <- day + cumsum(month.days)[month]
//   leapdays <- year %% 4 == 0 & (year %% 400 == 0 | year %% 100 != 0) & day >= 60
//   day[leapdays] <- day[leapdays] + 1

//   # Get Julian date - 2400000
//   hour <- hour + min / 60 + sec / 3600 # hour plus fraction
//   delta <- year - 1949
//   leap <- trunc(delta / 4) # former leapyears
//   jd <- 32916.5 + delta * 365 + leap + day + hour / 24

//   # The input to the Atronomer's almanach is the difference between
//   # the Julian date and JD 2451545.0 (noon, 1 January 2000)
//   time <- jd - 51545.

//   # Ecliptic coordinates

//   # Mean longitude
//   mnlong <- 280.460 + .9856474 * time
//   mnlong <- mnlong %% 360
//   mnlong[mnlong < 0] <- mnlong[mnlong < 0] + 360

//   # Mean anomaly
//   mnanom <- 357.528 + .9856003 * time
//   mnanom <- mnanom %% 360
//   mnanom[mnanom < 0] <- mnanom[mnanom < 0] + 360
//   mnanom <- mnanom * deg2rad

//   # Ecliptic longitude and obliquity of ecliptic
//   eclong <- mnlong + 1.915 * sin(mnanom) + 0.020 * sin(2 * mnanom)
//   eclong <- eclong %% 360
//   eclong[eclong < 0] <- eclong[eclong < 0] + 360
//   oblqec <- 23.429 - 0.0000004 * time
//   eclong <- eclong * deg2rad
//   oblqec <- oblqec * deg2rad

//   # Celestial coordinates
//   # Right ascension and declination
//   num <- cos(oblqec) * sin(eclong)
//   den <- cos(eclong)
//   ra <- atan(num / den)
//   ra[den < 0] <- ra[den < 0] + pi
//   ra[den >= 0 & num < 0] <- ra[den >= 0 & num < 0] + twopi
//   dec <- asin(sin(oblqec) * sin(eclong))

//   # Local coordinates
//   # Greenwich mean sidereal time
//   gmst <- 6.697375 + .0657098242 * time + hour
//   gmst <- gmst %% 24
//   gmst[gmst < 0] <- gmst[gmst < 0] + 24.

//   # Local mean sidereal time
//   lmst <- gmst + long / 15.
//   lmst <- lmst %% 24.
//   lmst[lmst < 0] <- lmst[lmst < 0] + 24.
//   lmst <- lmst * 15. * deg2rad

//   # Hour angle
//   ha <- lmst - ra
//   ha[ha < -pi] <- ha[ha < -pi] + twopi
//   ha[ha > pi] <- ha[ha > pi] - twopi

//   # Latitude to radians
//   lat <- lat * deg2rad

//   # Azimuth and elevation
//   el <- asin(sin(dec) * sin(lat) + cos(dec) * cos(lat) * cos(ha))
//   az <- asin(-cos(dec) * sin(ha) / cos(el))
//   elc <- asin(sin(dec) / sin(lat))
//   az[el >= elc] <- pi - az[el >= elc]
//   az[el <= elc & ha > 0] <- az[el <= elc & ha > 0] + twopi

//   el <- el / deg2rad
//   az <- az / deg2rad
//   lat <- lat / deg2rad

//   return(list(elevation=el, azimuth=az))
// }
}

int main(){

	point a;
	a.x = 5;
	a.y = 5;

	point b;
	b.x = 0;
	b.y = 0;

	double JD2 = get_jd(2012, 8, 31);
	double J2k = get_jd(2000, 1, 1);
	point_3d sun_pos = celestial(JD2);

	printf("%f\n", J2k);
	printf("%f\n", JD2);
	printf("%f\n", JD2-J2k);
	printf("%f\n", get_ut());

	printf("%f\n", sun_pos.x);
	printf("%f\n", sun_pos.y);
	printf("%f\n", sun_pos.z);
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