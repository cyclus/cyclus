#ifndef CYCLUS_SRC_GIS_H_
#define CYCLUS_SRC_GIS_H_

// @TODO: find what to include

#include <math.h>
#include <stdio.h>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "cycamore_version.h"
#include "cyclus.h"

// forward declarations
namespace cyclus {
class GIS;
}  // namespace cycamore

// forward includes
#include "gis_tests.h"

namespace cyclus {

/// @class GIS
/// The GIS class is a basic class that stores the geographic location of each
/// agents in longditude and latitude (subject to change) and follows the ISO
/// 6709 standard. Allows, string expression (and XML representation in the
/// future)).
///
/// Brief Description of ISO 6709:
/// 1. Latitude comes before Longitude
/// 2. North latitude is positive
/// 3. East longitude is positive
/// 4. Fraction of degrees is prefered in digital data exchange.
///
/// The class is adapted from 'https://github.com/jaime-olivares/coordinate'
/// under the MIT License.
///
/// @TODO Imprementation of GIS information.

class GIS : public cyclus::Ider {
  friend class GISTests public :
      /// The default constructor for GIS.
      GIS();
  /*
    /// GIS constructor with latitude and longditude as sexagesimal number.
    /// @param latitude and longditude expressed in Degrees. Latitude: DDMMSS,
    /// Longitude: DDMMSS (DD: Degree, MM: minutes, SS: seconds)
    GIS(float sexagesimal_lat, float sexagesimal_lon);
  */
  /// GIS constructor with latitude and longditude as decimal number.
  /// @param latitude and longditude expressed in Degrees.
  GIS(float decimal_lat, float decimal_lon);

  /// The default destructor for GIS
  ~GIS();

  /// Getters
  float get_latitude_decimal() const;
  // float get_latitude_degrees() const;
  float get_longitude_decimal() const;
  // float get_longitude_degrees() const;

  /// Setter for latitude in decimals
  /// @param latitude in decimals
  void set_latitude_decimal(float lat);

  /// Setter for latitude in sexagesimal number (degrees)
  /// @param latitude in degrees: DDMMSS (DD: degrees, MM: minutes, SS:
  /// seconds)
  // void set_latitude_degrees(float lat);

  /// Setter for latitude in decimals
  /// @param longitude in decimals
  void set_longitude_decimal(float lon);

  /// Setter for longitude in sexagesimal number (degrees)
  /// @param longitude in degrees: DDDMMSS (DD: degrees, MM: minutes, SS:
  /// seconds)
  // void set_longitude_degrees(float lon);

  /// returns the distance between the GIS object the function has been called
  /// on and the parameter.
  double get_distance(GIS target) const;

  /// returns an array of agents that are within a specified distance, in
  /// kilometers, from an agent in ascending order.
  /// @param range should be in kilometers not miles.
  // GIS[] nearby(double range) const;

  /// returns an array of agents that are within a specified distance, in
  /// kilometers, from an agent in ascending order.
  /// @param range should be in kilometers not miles. reference should be an gis
  /// object.
  // GIS[] nearby(const GIS *reference, double range) const;

  /// converts GIS location into a string expression that follows ISO 6709 Annex
  /// H.
  string toString() const;

 private:
  /// Latitude is stored as seconds of degree.
  float latitude;

  /// Longitude is stored as seconds of degree.
  float longitude;

  /// HELPER Functions
  /*
  /// Sorts the array of agents by distance in ascending order.
  double sort(double &list[]);
  */
  /// Converts decimal to sexagesimal
  float decimaltoSexagesimal(float decimal)
      /// Converts degrees of seconds to sexagesimal
      float degreeSecondstoSexagesimal(float degreeSeconds)

          float setPrecision(float value, double precision);
};
}

#endif  // CYCLUS_SRC_GIS_H_
