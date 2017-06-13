#include <math.h>
#include <stdio.h>
#include <sstream>

#include "position.h"

namespace cyclus {
namespace toolkit {

Position::Position() : latitude_(0), longitude_(0) {}

Position::Position(double decimal_lat, double decimal_lon) {
  latitude_ = SetPrecision(decimal_lat * CYCLUS_DECIMAL_SECOND_MULTIPLIER, 1);
  longitude_ = SetPrecision(decimal_lon * CYCLUS_DECIMAL_SECOND_MULTIPLIER, 1);
}

Position::~Position() {}

double Position::latitude() const {
  return SetPrecision(latitude_ / CYCLUS_DECIMAL_SECOND_MULTIPLIER, 6);
}

double Position::longitude() const {
  return SetPrecision(longitude_ / CYCLUS_DECIMAL_SECOND_MULTIPLIER, 6);
}

void Position::latitude(double lat) {
  latitude_ = SetPrecision(lat * CYCLUS_DECIMAL_SECOND_MULTIPLIER, 1);
}

void Position::longitude(double lon) {
  longitude_ = SetPrecision(lon * CYCLUS_DECIMAL_SECOND_MULTIPLIER, 1);
}

void Position::set_position(double lat, double lon) {
  latitude_ = SetPrecision(lat * CYCLUS_DECIMAL_SECOND_MULTIPLIER, 1);
  longitude_ = SetPrecision(lon * CYCLUS_DECIMAL_SECOND_MULTIPLIER, 1);
}

double Position::Distance(Position target) const {
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

std::string Position::ToString(Position::StringFormat format) const {
  std::stringstream lat_string;
  std::stringstream lon_string;
  double lat = this->latitude();
  double lon = this->longitude();
  switch (format) {
    case StringFormat::DEGREES:
      lat_string << ToStringHelperLat(format, lat);
      lon_string << ToStringHelperLon(format, lon);
      break;
    case StringFormat::DEGREES_MINUTES:
      lat_string << ToStringHelperLat(format, lat);
      lon_string << ToStringHelperLon(format, lon);
      break;
    case StringFormat::DEGREES_MINUTES_SECONDS:
      lat_string << ToStringHelperLat(format, lat);
      lon_string << ToStringHelperLon(format, lon);
      break;
  }
  return lat_string.str() + lon_string.str() + "/";
}

double Position::SetPrecision(double value, double precision) const {
  if (precision == 0) {
    return floor(value);
  }
  return (floor((value * pow(10, precision) + 0.5)) / pow(10, precision));
}

std::string Position::ToStringHelperLat(int mode, double lat) const {
  std::stringstream lat_string;
  double lat_int;
  lat_string << ToStringHelper(lat);

  lat = modf(lat, &lat_int);
  if (((int)lat_int) / 10 == 0) {
    lat_string << "0";
  }
  switch (mode) {
    case StringFormat::DEGREES:
      lat_string << std::setprecision(7) << fabs(lat_int + lat);
      break;
    case StringFormat::DEGREES_MINUTES:
      lat_string << abs((int)lat_int) << ToStringHelperDM(lat);
      break;
    case StringFormat::DEGREES_MINUTES_SECONDS:
      lat_string << abs((int)lat_int) << ToStringHelperDMS(lat);
      break;
  }
  return lat_string.str();
}

std::string Position::ToStringHelperLon(int mode, double lon) const {
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
    case StringFormat::DEGREES:
      lon_string << std::setprecision(7) << fabs(lon_int + lon);
      break;
    case StringFormat::DEGREES_MINUTES:
      lon_string << abs((int)lon_int) << ToStringHelperDM(lon);
      break;
    case StringFormat::DEGREES_MINUTES_SECONDS:
      lon_string << abs((int)lon_int) << ToStringHelperDMS(lon);
      break;
  }
  return lon_string.str();
}

std::string Position::ToStringHelper(double value) const {
  std::stringstream temp;
  if (value > 0) {
    temp << "+";
  } else {
    temp << "-";
  }
  return temp.str();
}

std::string Position::ToStringHelperDM(double value) const {
  std::stringstream temp;
  value = fabs(value) * 60;
  if (((int)fabs(value)) / 10 == 0) {
    temp << "0";
  }
  temp << std::setprecision(5) << value;

  return temp.str();
}

std::string Position::ToStringHelperDMS(double value) const {
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
