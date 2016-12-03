#include "gis.h"

namespace cyclus {

GIS::GIS() {
  latitude = 0.0;
  longitude = 0.0;
}

GIS::GIS(float decimal_lat, float decimal_lon) {
  latitude = setPrecision(decimal_lat * 3600, 1);
  longitude = setPrecision(decimal_lon * 3600, 1);
}

GIS::~GIS() {}

float GIS::get_latitude_decimal() const {
  return setPrecision(latitude / 3600, 6);
}

float GIS::get_longitude_decimal() const {
  return setPrecision(longitude / 3600, 6);
}

void GIS::set_latitude_decimal(float lat) {
  latitude = setPrecision(lat * 3600, 1);
}

void GIS::set_longitude_decimal(float lon) {
  longitude = setPrecision(lon * 3600, 1);
}
/*
float GIS::get_latitude_degrees() const{
        // to be implemented if necessary
}
float GIS::get_longitude_degrees() const{
        // to be implemented if necessary
}

void GIS::set_latitude_degrees(float lat){
        // to be implemented if necessary
}

void GIS::set_longitude_degrees(float lon){
        // to be implemented if necessary
}
*/
double GIS::get_distance(GIS target) const {
  double curr_longitude = this.get_longitude_decimal();
  double curr_latitude = this.get_latitude_decimal();
  double target_longitude = target.get_longitude_decimal();
  double target_latitude = target.get_latitude_decimal();
  double diff_longitude = target_longitude - curr_longitude;
  double diff_latitude = target_latitude - curr_latitude;
  double angle = pow(sin(diff_latitude / 2), 2) + cos(curr_latitude)) * cos(target_latitude) * pow(sin(diff_longitude / 2), 2);
  double c = 2 * atan2(sqrt(angle), sqrt(1 - angle));
  return 6371.01 * c;
}
/*
GIS[] GIS::nearby(double range) const{
}

GIS[] GIS::nearby(const GIS *reference, double range) const{
}
*/
string GIS::toStringD() const {
  stringstream lat_string;
  stringstream lon_string;
  float lat = this.get_latitude_decimal();
  float lon = this.get_longitude_decimal();
  int temp_lat = to_string(setPrecision(fabs(lat), 0)).length();
  int temp_lon = to_string(setPrecision(fabs(lon), 0)).length();
  if (lat > 0) {
    lat_string << "+";
  } else {
    lat_string << "-";
  }
  if (temp_lat < 2) {
    lat_string << "0";
  }
  lat_string << setprecision(7) << fabs(lat);
  if (lon > 0) {
    lon_string << "+";
  } else {
    lon_string << "-";
  }
  for (int i = temp_lon; i < 3; i++) {
    lon_string << "0";
  }
  lon_string << setprecision(7) << fabs(lon) << "/";

  return lat_string.str() + lon_string.str();
}

string GIS::toStringDM() const {
  stringstream lat_string;
  stringstream lon_string;
  double lat = this.get_latitude_decimal();
  double lon = this.get_longitude_decimal();

  if (lat > 0) {
    lat_string << "+";
  } else {
    lat_string << "-";
  }
  if (lon > 0) {
    lon_string << "+";
  } else {
    lon_string << "-";
  }

  double lat_int, lon_int;
  lat = modf(lat, &lat_int);
  lon = modf(lon, &lon_int);
  if (((int) lat_int) / 10 == 0) {
    lat_string << "0";
  }
  if (((int) lon_int) / 100 == 0) {
    if (((int) lon_int) / 10 == 0) {
      lon_string << "0";
    }
    lon_string << "0";
  }
  lat_string << fabs(lat_int);
  lon_string << fabs(lon_int);

  lat = fabs(lat) * 60;
  lon = fabs(lon) * 60;
  lat_string << setprecision(5) << lat;
  lon_string << setprecision(5) << lon << "/";

  return lat_string.str() + lon_string.str();
}

string GIS::toStringDMS() const {
  stringstream lat_string;
  stringstream lon_string;
  double lat = this.get_latitude_decimal();
  double lon = this.get_longitude_decimal();
  if (lat > 0) {
    lat_string << "+";
  } else {
    lat_string << "-";
  }
  if (lon > 0) {
    lon_string << "+";
  } else {
    lon_string << "-";
  }
  double lat_int, lon_int;
  lat = modf(lat, &lat_int);
  lon = modf(lon, &lon_int);
  if (((int) lat_int) / 10 == 0) {
    lat_string << "0";
  }
  if (((int) lon_int) / 100 == 0) {
    if (((int) lon_int) / 10 == 0) {
      lon_string << "0";
    }
    lon_string << "0";
  }
  lat_string << fabs(lat_int);
  lon_string << fabs(lon_int);

  lat = modf(fabs(lat) * 60, &lat_int);
  lon = modf(fabs(lon) * 60, &lon_int);
  lat_string << fabs(lat_int);
  lon_string << fabs(lon_int);
  lat = lat * 60;
  lon = lon * 60;
  lat_string << setprecision(3) << lat;
  lon_string << setprecision(3) << lon << "/";
  return lat_string.str() + lon_string.str();
}

/*
double GIS::sort(double &list[]){
}
*/

float GIS::setPrecision(float value, int precision) {
  return (floor((value * pow(10, precision) + 0.5)) / pow(10, precision));
}