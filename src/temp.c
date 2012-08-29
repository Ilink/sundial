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

