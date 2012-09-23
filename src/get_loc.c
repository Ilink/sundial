#include "get_loc.h"
#include "types.h"
#include <stdio.h>
// #include "GeoIP.h" // this is included in GeoIPCity.h
#include <GeoIP.h>
#include <GeoIPCity.h>

geo_coord get_loc(char* ip){
	FILE           *f;
	FILE *file;
	GeoIP          *gi;
	GeoIPRecord    *gir;
	int             generate = 0;
	char            host[50];
	const char     *time_zone = NULL;
	char          **ret;
	geo_coord loc;

	gi = GeoIP_open("../data/GeoIPCity.dat", GEOIP_INDEX_CACHE);

	file = fopen("geo_ip.txt", "w+");

	// GeoIP * gi;
	// gi = GeoIP_new(GEOIP_STANDARD);
	if (gi == NULL) {
		// printf("Error opening database\n");
		exit(1);
	}
	// gi = GeoIP_open("GeoIPCity.dat", GEOIP_INDEX_CACHE);
	// fprintf(file, "code %s\n",
	// 	GeoIP_country_code_by_name(gi, "yahoo.com"));

	
	gir = GeoIP_record_by_name(gi, ip);
	if(gir != NULL){
		fprintf(file, "%s\t%s\n",
			gir->latitude,
			gir->longitude
		);
	} else {
		// printf('something wrong with GeoIP_record_by_ipnum');
		exit(1);
	}
	

	// if (gir != NULL) {
	// 	gir = GeoIP_record_by_ipnum(gi, (const char *) ip);
	// 	// time_zone = GeoIP_time_zone_by_country_and_region(gir->country_code, gir->region);
	// 	fprintf(file, "%s\t%s\t%s\n", host,
	// 		ret->latitude,
	// 		ret->longitude
	// 	);
	// 	// GeoIP_range_by_ip_delete(ret);
	// 	// GeoIPRecord_delete(gir);
	// 	loc.lat = ret->latitude;
	// 	loc.lng = ret->longitude;
	// }
	
	// GeoIP_delete(gi);

	
	loc.lat = 1.0;
	loc.lng = 1.0;
	return loc;
}