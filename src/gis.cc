#include "gis.h"

using namespace std;

namespace cyclus {

GIS::GIS() : latitude_(0), longitude_(0) {}

GIS::GIS(float decimal_lat, float decimal_lon) {
  latitude_ = setPrecision(decimal_lat * 3600, 1);
  longitude_ = setPrecision(decimal_lon * 3600, 1);
}

GIS::~GIS() {}

float GIS::get_latitude() const { return setPrecision(latitude_ / 3600, 6); }

float GIS::get_longitude() const { return setPrecision(longitude_ / 3600, 6); }

void GIS::set_latitude(float lat) { latitude_ = setPrecision(lat * 3600, 1); }

void GIS::set_longitude(float lon) { longitude_ = setPrecision(lon * 3600, 1); }

double GIS::Distance(GIS target) const {
  double curr_longitude = this->get_longitude() * M_PI / 180;
  double curr_latitude = this->get_latitude() * M_PI / 180;
  double target_longitude = target.get_longitude() * M_PI / 180;
  double target_latitude = target.get_latitude() * M_PI / 180;
  double dlong = target_longitude - curr_longitude;
  double dlat = target_latitude - curr_latitude;

  double temp =
      pow(sin(dlat / 2), 2) +
      pow(sin(dlong / 2), 2) * cos(curr_latitude) * cos(target_latitude);

  double temp2 = 2 * atan2(sqrt(temp), sqrt(1 - temp));
  return 6372.8 * temp2;  // 6372.8 is the radius of earth in KM
}

string GIS::toString(string return_format) const {
  stringstream lat_string;
  stringstream lon_string;
  float lat = this->get_latitude();
  float lon = this->get_longitude();
  switch (return_format) {
    case "d":
      lat_string << ToStringHelperLat(return_format, lat);
      lon_string << ToStringHelperLon(return_format, lon);

      lat_string << setprecision(7) << fabs(lat);
      lon_string << setprecision(7) << fabs(lon) << "/";
      break;
    case "dm":
      lat_string << ToStringHelperLat(return_format, lat);
      lon_string << ToStringHelperLon(return_format, lon);
      break;
    case "dms":
      lat_string << ToStringHelperLat(return_format, lat);
      lon_string << ToStringHelperLon(return_format, lon);
      break;
  }
  return lat_string.str() + lon_string.str();
}

float GIS::SetPrecision(float value, double precision) const {
  if (precision == 0) {
    return floor(value);
  }
  return (floor((value * pow(10, precision) + 0.5)) / pow(10, precision));
}

string GIS::ToStringHelperLat(string mode, float lat) {
  string lat_string;
  double lat_int;

  lat_string << ToStringHelper(lat);

  lat = modf(lat, &lat_int);
  if (((int)lat_int) / 10 == 0) {
    lat_string << "0";
  }

  switch (mode) {
    case "d":
      break;
    case "dm":
      lat_string << ToStringHelperDM("lat", lat);
      break;
    case "dms":
      lat_string << ToStringHelperDMS("lat", lat);
      break;
  }
  return lat_string;
}

string GIS::ToStringHelperLon(string mode, float lon) {
  string lon_string;
  double lon_int;

  lon_string << ToStringHelper(lon);

  lon = modf(lon, &lon_int);
  if (((int)lon_int) / 100 == 0) {
    if (((int)lon_int) / 10 == 0) {
      lon_string << "0";
    }
    lon_string << "0";
  }

  switch (mode) {
    case "d":
      break;
    case "dm":
      lon_string << ToStringHelperDM("lon", lon);
      break;
    case "dms":
      lon_string << ToStringHelperDMS("lon", lon);
      break;
  }
  return lon_string;
}

string GIS::ToStringHelper(float value) {
  string temp;
  if (value > 0) {
    temp << "+";
  } else {
    temp << "-";
  }
  return temp;
}

       {
  string temp;
  double value_int;
  switch (mode) {
    case "lat":
      temp << fabs(value_int);
      value = fabs(value) * 60;
      if ((int)fabs(value) / 10 == 0) {
        temp << "0";
      }
      temp << setprecision(5) << value;
      break;
    case "lon":
      temp << fabs(value_int);
      value = fabs(value) * 60;
      if ((int)fabs(value) / 10 == 0) {
        temp "0";
      }
      temp << setprecision(5) << value << "/";
      break;
  }
  return temp;
}

string GIS::ToStringHelperDMS(string mode, float value) {
  string temp;
  double value_int;
  switch (mode) {
    case "lat":
      temp << abs((int)value_int);
      value = modf(fabs(value) * 60, &value_int);
      if (((int)value_int) / 10 == 0) {
        temp << "0";
      }
      temp << fabs(value_int);
      value = value * 60;
      if ((int)fabs(value) / 10 == 0) {
        temp << "0";
      }
      temp << setprecision(1) << fixed << value;
      break;
    case "lon":
      temp << abs((int)value_int);
      value = modf(fabs(value) * 60, &value_int);
      if (((int)value_int) / 10 == 0) {
        temp << "0";
      }
      temp << fabs(value_int);
      value = value * 60;
      if ((int)fabs(value) / 10 == 0) {
        temp << "0";
      }
      temp << setprecision(1) << fixed << value << "/";
      break;
  }
  return temp;
}
}