#ifndef CYCLUS_SRC_GIS_H_
#define CYCLUS_SRC_GIS_H_

#include <math.h>
#include <stdio.h>
#include <sstream>
#include <string>

#include "cyclus.h"

// forward declarations
namespace cyclus {
class GIS;
}

namespace cyclus {

/// @class GIS
/// The GIS class is a basic class that stores the geographic location of each
/// agent in latitude and longitude and follows the ISO
/// 6709 standard. Allows string expression of GIS objects as specified in
/// ANNEX H of ISO 6709 found here: 'https://www.iso.org/standard/39242.html'
/// or a quick simplified version found here
/// 'https://en.wikipedia.org/wiki/ISO_6709'
///
/// Brief Description of ISO 6709:
/// 1. Latitude comes before Longitude
/// 2. North latitude is positive
/// 3. East longitude is positive
/// 4. Fraction of degrees (decimal values) is prefered in digital data
/// exchange.
///
/// The class is adapted from 'https://github.com/jaime-olivares/coordinate'
/// under the MIT License.
///
/// Longitude and Latitude is stored as seconds of degrees. This was also
/// adapted from 'https://github.com/jaime-olivares/coordinate'. This, according
/// to the jaime-olivares, allows the coordinate elements such as degrees,
/// minutes, and seconds to "remain on the integral portion of values, with the
/// exception of decimal of seconds, avoiding loss of precision." This is
/// calculated by multiplying decimal degrees by 3600.
/// example: 05.2169 -> 18780.84

class GIS {
 public:
  /// The default constructor for GIS. This will creat an object with
  /// lat =0, and long=0.
  GIS();

  /// GIS constructor with latitude and longditude as degrees as a decimal
  /// number.
  /// @param deciaml_lat latitude expressed in degrees as a decimal number.
  /// @param decimal_lon longditude expressed in degrees as a decimal number.
  GIS(float decimal_lat, float decimal_lon);

  /// The default destructor for GIS
  ~GIS();

  /// Returns the current latitude.
  /// @return decimal representation of latitude.
  float get_latitude() const;

  /// Returns the current longitude.
  /// @return decimal representation of longitude.
  float get_longitude() const;

  /// Sets a new latitude
  /// @param lat latitude expressed in decimal degrees.
  void set_latitude(float lat);

  /// Sets a new longitude
  /// @param lon longitude expressed in decimal degrees.
  void set_longitude(float lon);

  /// Returns the distance (in km) between this GIS object and the target GIS
  /// object.
  /// @param target the GIS object some distnace away from the current one
  /// @return distance between this and target in kilometers
  double Distance(GIS target) const;

  /// Converts GIS location into a string expression that follows ISO 6709 Annex
  /// H.
  /// @param return_format
  /// @parblock
  /// 			The format of output. Options are 'd', 'dm', or 'dms':
  ///
  /// 			d:   return in degrees format
  ///			dm:  return in degrees and minutes format
  ///			dms: return in egrees minutes seconds format
  /// @endparblock
  /// @return String representation of the GIS object that complies with ISO6709
  /// Annex H
  std::string ToString(std::string return_format) const;

 private:
  /// Latitude is stored as seconds of degree. Explanation and example is
  /// available above in class documentation
  float latitude_;

  /// Longitude is stored as seconds of degree.
  float longitude_;

  /// Sets the precision for float values.
  /// @param value that requires change of precision
  /// @param precision the number decimal places wanted
  /// @return value with the number of decimal places (precision) as defined
  float SetPrecision(float value, double precision) const;

  /// Formats longitude for toString function
  /// @param mode
  /// @parblock
  /// 			The format of output. Options are 'd', 'dm' or 'dms' :
  ///
  /// 			d:   return in degrees format
  ///			dm:  return in degrees and minutes format
  ///			dms: return in degrees minutes seconds format
  /// @endparblock
  /// @param lon longitude of the object
  /// @return partially formatted string
  string ToStringHelperLon(std::string mode, int lon);

  /// Formats latitude for toString function
  /// @param mode
  /// @parblock
  /// 			The format of output. Options are 'd', 'dm' or 'dms' :
  ///
  /// 			d:   return in degrees format
  ///			dm:  return in degrees and minutes format
  ///			dms: return in degrees minutes seconds format
  /// @endparblock
  /// @param lat latitude of the object
  /// @return partially formatted string
  string ToStringHelperLat(std::string mode, int lat);

  /// Adds "+" or "-" sign for the ToStringHelperLon/Lat function
  /// @param value longitude or latitude of the object
  /// @returns "+" or "-" for the ToString function
  string ToStringHelper(float value);

  /// Formats longitude and latitude in degrees and minutes for the
  /// ToStringHelperLon/Lat function
  /// @param mode
  /// @parblock
  /// 			The format of input. Options are "lon" and "lat":
  ///
  /// 			lon:  return formatted string representation of
  /// longitude
  ///			lat:  return formatted string representation of latitude
  /// @endparblock
  /// @param value longitude or latitude of the object
  /// @return formatted string representation of longiiiitude or latitude
  string ToStringHelperDM(string mode, float value);

  /// Formats longitude and latitude in degrees minutes and seconds for the
  /// ToStringHelperLon/Lat function
  /// @param mode
  /// @parblock
  /// 			The format of input. Options are "lon" and "lat":
  ///
  /// 			lon:  return formatted string representation of
  /// longitude
  ///			lat:  return formatted string representation of latitude
  /// @endparblock
  /// @param value longitude or latitude of the object
  /// @return formatted string representation of longiiiitude or latitude
  string ToStringHelperDMS(string mode, float value);
};
}

#endif  // CYCLUS_SRC_GIS_H_
