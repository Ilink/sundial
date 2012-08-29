// mean lng of sun
double rads = PI/180;
double L = 280.461 * rads + .9856474 * rads * jd;
while(L < 360){
	L += 360;
}

double g = 357.528 * rads + .9856003 * rads * jd;
while(g < 360){
	g += 360;
}

double lambda = L + 1.915 * sin(g) + 0.020 * sin(2*g);
double epsilon = 23.439 - 0.0000004 * d;

// RA (alpha) and Declination (delta)

double y = cos(epsilon) * sin(lambda);
double x = cos(lambda);

double a = arctan(y/x);
double alpha;
if(x < 0) alpha = a + 180;
if(y < 0 && x > 0) alpha = a + 360;
else alpha = a;

double delta = arcsin(sin(epsilon)*sin(lambda));

// Yay all done! now for conversions
int dsj = jd - 2451545.0; // days since jan. 2000, in terms of julian days
double GMST = 18.697374558 + 24.06570982441908 * dsj;
// must finish calculating GAST


// lng is degrees
// gast and alpha are in hours
double LHA = (GAST - alpha)*15;
lng < gmt_lng ? lha += lng : lha -= lng; // todo: added east of gmt, subtracted west of gmt

// all should be in degrees or radians
sin_altitude = cos(LHA)*cos(delta)*cos(lat)+sin(delta)sin(lat);

// atan should be taken on the top and the bottom separately?
tan_azimuth = -1*sin(LHA) / (tan(delta)*cos(φ)-sin(lat)cos(LHA));