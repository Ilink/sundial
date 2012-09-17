#ifndef SUN_H
#define SUN_H

s_coord celestial(double jd, double lat, double lng, double hour, double tz);
double get_ha(int hour);
double get_hla(double lat, double ha);
double get_jd(int year, int month, int day);
double get_local();

#endif