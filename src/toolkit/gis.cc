#include <math.h>
#include <stdio.h>
#include <sstream>

#include "gis.h"

namespace cyclus {
namespace toolkit {

GIS::GIS() : latitude_(0), longitude_(0) {}

GIS::GIS(float decimal_lat, float decimal_lon) {
  latitude_ = SetPrecision(decimal_lat * DecimalSecondsMultiplier, 1);
  longitude_ = SetPrecision(decimal_lon * DecimalSecondsMultiplier, 1);
}

GIS::~GIS() {}

float GIS::latitude() const {
  return SetPrecision(latitude_ / DecimalSecondsMultiplier, 6);
}

float GIS::longitude() const {
  return SetPrecision(longitude_ / DecimalSecondsMultiplier, 6);
}

void GIS::latitude(float lat) {
  latitude_ = SetPrecision(lat * DecimalSecondsMultiplier, 1);
}

void GIS::longitude(float lon) {
  longitude_ = SetPrecision(lon * DecimalSecondsMultiplier, 1);
}

void GIS::set_position(float lat, float lon) {
  latitude_ = SetPrecision(lat * DecimalSecondsMultiplier, 1);
  longitude_ = SetPrecision(lon * DecimalSecondsMultiplier, 1);
}

double GIS::Distance(GIS target) const {
  double earth_radius = 6372.8;  // in kilometers (KM)
  double curr_longitude = this->longitude() * M_PI / 180;
  double curr_latitude = this->latitude() * M_PI / 180;
  double tarlongitude = target.longitude() * M_PI / 180;
  double tarlatitude = target.latitude() * M_PI / 180;
  double dlong = tarlongitude - curr_longitude;
  double dlat = tarlatitude - curr_latitude;

  double half_chord_length_sq =
      pow(sin(dlat / 2), 2) +
      pow(sin(dlong / 2), 2) * cos(curr_latitude) * cos(tarlatitude);

  double angular_distance =
      2 * atan2(sqrt(half_chord_length_sq), sqrt(1 - half_chord_length_sq));
  return earth_radius * angular_distance;
}

std::string GIS::ToString(GIS::StringFormat format) const {
  std::stringstream lat_string;
  std::stringstream lon_string;
  float lat = this->latitude();
  float lon = this->longitude();
  switch (format) {
    case 1:
      lat_string << ToStringHelperLat(format, lat);
      lon_string << ToStringHelperLon(format, lon);
      break;
    case 2:
      lat_string << ToStringHelperLat(format, lat);
      lon_string << ToStringHelperLon(format, lon);
      break;
    case 3:
      lat_string << ToStringHelperLat(format, lat);
      lon_string << ToStringHelperLon(format, lon);
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

std::string GIS::ToStringHelperLat(int mode, float lat) const {
  std::stringstream lat_string;
  double lat_int;
  lat_string << ToStringHelper(lat);

  lat = modf(lat, &lat_int);
  if (((int)lat_int) / 10 == 0) {
    lat_string << "0";
  }
  switch (mode) {
    case 1:
      lat_string << std::setprecision(7) << fabs(lat_int + lat);
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

std::string GIS::ToStringHelperLon(int mode, float lon) const {
  std::stringstream lon_string;
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
      lon_string << std::setprecision(7) << fabs(lon_int + lon);
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

std::string GIS::ToStringHelper(float value) const {
  std::stringstream temp;
  if (value > 0) {
    temp << "+";
  } else {
    temp << "-";
  }
  return temp.str();
}

std::string GIS::ToStringHelperDM(float value) const {
  std::stringstream temp;
  value = fabs(value) * 60;
  if (((int)fabs(value)) / 10 == 0) {
    temp << "0";
  }
  temp << std::setprecision(5) << value;

  return temp.str();
}

std::string GIS::ToStringHelperDMS(float value) const {
  std::stringstream temp;
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
  temp << std::setprecision(1) << std::fixed << value;

  return temp.str();
}

}  // namespace toolkit
}  // namespace cyclus
