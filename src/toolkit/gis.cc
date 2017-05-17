#include "gis.h"

using namespace std;

namespace cyclus {
namespace toolkit {

GIS::GIS() : latitude_(0), longitude_(0) {}

GIS::GIS(float decimal_lat, float decimal_lon) {
  latitude_ = SetPrecision(decimal_lat * 3600, 1);
  longitude_ = SetPrecision(decimal_lon * 3600, 1);
}

GIS::~GIS() {}

float GIS::get_latitude() const { return SetPrecision(latitude_ / 3600, 6); }

float GIS::get_longitude() const { return SetPrecision(longitude_ / 3600, 6); }

void GIS::set_latitude(float lat) { latitude_ = SetPrecision(lat * 3600, 1); }

void GIS::set_longitude(float lon) { longitude_ = SetPrecision(lon * 3600, 1); }

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

string GIS::ToString(int return_format) const {
  stringstream lat_string;
  stringstream lon_string;
  float lat = this->get_latitude();
  float lon = this->get_longitude();
  switch (return_format) {
    case 1:
      lat_string << ToStringHelperLat(return_format, lat);
      lon_string << ToStringHelperLon(return_format, lon);
      break;
    case 2:
      lat_string << ToStringHelperLat(return_format, lat);
      lon_string << ToStringHelperLon(return_format, lon);
      break;
    case 3:
      lat_string << ToStringHelperLat(return_format, lat);
      lon_string << ToStringHelperLon(return_format, lon);
      break;
  }
  return lat_string.str() + lon_string.str() + "/";
}

float GIS::SetPrecision(float value, double precision) const {
  if (precision == 0) {
    return floor(value);
  }
  return (floor((value * pow(10, precision) + 0.5)) / pow(10, precision));
}

string GIS::ToStringHelperLat(int mode, float lat) const {
  stringstream lat_string;
  double lat_int;
  lat_string << ToStringHelper(lat);

  lat = modf(lat, &lat_int);
  if (((int)lat_int) / 10 == 0) {
    lat_string << "0";
  }
  switch (mode) {
    case 1:
      lat_string << setprecision(7) << fabs(lat_int + lat);
      break;
    case 2:
      lat_string << abs((int)lat_int) << ToStringHelperDM(lat);
      break;
    case 3:
      lat_string << abs((int)lat_int) << ToStringHelperDMS(lat);
      break;
  }
  return lat_string.str();
}

string GIS::ToStringHelperLon(int mode, float lon) const {
  stringstream lon_string;
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
    case 1:
      lon_string << setprecision(7) << fabs(lon_int + lon);
      break;
    case 2:
      lon_string << abs((int)lon_int) << ToStringHelperDM(lon);
      break;
    case 3:
      lon_string << abs((int)lon_int) << ToStringHelperDMS(lon);
      break;
  }
  return lon_string.str();
}

string GIS::ToStringHelper(float value) const {
  stringstream temp;
  if (value > 0) {
    temp << "+";
  } else {
    temp << "-";
  }
  return temp.str();
}

string GIS::ToStringHelperDM(float value) const {
  stringstream temp;
  value = fabs(value) * 60;
  if (((int)fabs(value)) / 10 == 0) {
    temp << "0";
  }
  temp << setprecision(5) << value;

  return temp.str();
}

string GIS::ToStringHelperDMS(float value) const {
  stringstream temp;
  double value_int;
  value = fabs(value) * 60;
  value = modf(value, &value_int);
  if (((int)value_int) / 10 == 0) {
    temp << "0";
  }
  temp << fabs(value_int);
  value = value * 60;
  if ((int)fabs(value) / 10 == 0) {
    temp << "0";
  }
  temp << setprecision(1) << fixed << value;

  return temp.str();
}

}  // namespace toolkit
}  // namespace cyclus
