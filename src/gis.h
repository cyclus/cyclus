#ifndef CYCLUS_SRC_GIS_H_
#define CYCLUS_SRC_GIS_H_

#include <math.h>
#include <stdio.h>
#include <sstream>
#include <string>

#include <boost/shared_ptr.hpp>

#include "cyclus.h"

// forward declarations
namespace cyclus {
class GIS;
}

namespace cyclus {

///
/// @class GIS
/// The GIS class is a basic class that stores the geographic location of each
/// agents in latitude and longitude and follows the ISO
/// 6709 standard. Allows, string expression of GIS objects as specified in
/// ANNEX H.
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

class GIS {
 public:
  /// The default constructor for GIS. Make sure to add latitude and longitude
  /// if this constructor is used.
  GIS();

  /// GIS constructor with latitude and longditude as decimal number.
  /// @param latitude and longditude expressed in Degrees.
  GIS(float decimal_lat, float decimal_lon);

  /// The default destructor for GIS
  ~GIS();

  /// Getters for latitude and longitude
  /// @return decimal representation of latitude or longitude.
  float get_latitude_decimal() const;
  float get_longitude_decimal() const;

  /// Setter for latitude in decimals. Please input the latitude as degrees not
  /// as DDMMSS.S
  /// @param latitude in decimals
  void set_latitude_decimal(float lat);

  /// Setter for latitude in decimals. Please input the longitude as degrees not
  /// as DDDMMSS.S
  /// @param longitude in decimals
  void set_longitude_decimal(float lon);

  /// returns the distance between the GIS object the function has been called
  /// in kilometers
  /// @param GIS object that this is compared to
  /// @return distance between this and target in kilometers
  double get_distance(GIS target) const;

  /// converts GIS location into a string expression that follows ISO 6709 Annex
  /// H as degrees.
  /// @return string representation of the GIS object that follows ISO 6709
  /// Annex H. Degrees format.
  std::string toStringD() const;

  /// converts GIS location into a string expression that follows ISO 6709 Annex
  /// H as degrees and minutes.
  /// @return string representation of the GIS object that follows ISO 6709
  /// Annex H. Degrees and Minutes format.
  std::string toStringDM() const;

  /// converts GIS location into a string expression that follows ISO 6709 Annex
  /// H as degrees minutes and seconds.
  /// @return string representation of the GIS object that follows ISO 6709
  /// Annext H. Degrees, minutes and seconds format.
  std::string toStringDMS() const;

 private:
  /// Latitude is stored as seconds of degree.
  float latitude;

  /// Longitude is stored as seconds of degree.
  float longitude;

  /// Sets the precision for float values.
  /// @param value to set the precision, and the precision
  /// @return value with the precision as defined
  float setPrecision(float value, double precision) const;
};
}

#endif  // CYCLUS_SRC_GIS_H_
