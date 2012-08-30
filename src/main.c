#include <ncurses.h>
#include <stdio.h>
#include <math.h>
#include "main.h"

double const PI = atan(1)*4;

// the typedef should go in the header
typedef struct {
	int x; 
	int y;
} point;

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

double awesome_calc(int year, int month, int day, 
	int _hour, int min, int sec, double lat, double lng){

	double twopi = 2*PI;
	double pi_rad = PI/180;

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

	double mnlong = 280.460 + .9856474 * time;
	double mnlong = mnlong % 360;
	double mnlong[mnlong < 0] <- mnlong[mnlong < 0] + 360



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

	
	// printf("%d", y);

	// Initialize ncurses
	initscr();
	clear();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0);	

	// test symbol
	char main_char = '@';
	char ch;

	// Rendering loop

	draw_line(1,1,20,10, '@');
	refresh();
	while(1) {
		ch = getch();
		if(ch == 'q' || ch == 'Q') {
			break;
		}
	}
	endwin(); // clear ncurses's junk
}