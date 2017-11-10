#ifndef CYCLUS_SRC_TOOLKIT_POSITION_H_
#define CYCLUS_SRC_TOOLKIT_POSITION_H_
#define CYCLUS_DECIMAL_SECOND_MULTIPLIER 3600

#include <string>

#include "cyclus.h"

namespace cyclus {
namespace toolkit {
/// @class Position
/// The Position class is a basic class that stores the geographic location of
/// each
/// agent in latitude and longitude and follows the ISO
/// 6709 standard. Allows string expression of Position objects as specified in
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
///   Copyright (c) 2015 Jaime Olivares
///
///   Permission is hereby granted, free of charge, to any person obtaining a
///   copy of this software and associated documentation files (the "Software")
///   , to deal in the Software without restriction, including without
///   limitation the rights to use, copy, modify, merge, publish, distribute,
///   sublicense, and/or sell copies of the Software, and to permit persons to
///   whom the Software is furnished to do so, subject to the following
///   conditions:
///
///   The above copyright notice and this permission notice shall be included
///   in all copies or substantial portions of the Software.
///
///   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
///   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
///   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
///   NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
///   DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
///   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
///   USE OR OTHER DEALINGS IN THE SOFTWARE.
///
/// Longitude and Latitude is stored as seconds of degrees. This was also
/// adapted from 'https://github.com/jaime-olivares/coordinate'. This, according
/// to the jaime-olivares, allows the coordinate elements such as degrees,
/// minutes, and seconds to "remain on the integral portion of values, with the
/// exception of decimal of seconds, avoiding loss of precision." This is
/// calculated by multiplying decimal degrees by 3600.
/// example: 05.2169 -> 18780.84

class Position {
 public:
  enum StringFormat {
    DEGREES = 1,
    DEGREES_MINUTES,
    DEGREES_MINUTES_SECONDS,
  };

  /// The default constructor for Position. This will creat an object with
  /// lat=0, and long=0.
  Position();

  /// Position constructor with latitude and longditude as degrees as a double
  /// point precision floating point.
  /// @param deciaml_lat latitude expressed in degrees as a double.
  /// @param decimal_lon longditude expressed in degrees as a double.
  Position(double decimal_lat, double decimal_lon);

  /// The default destructor for Position
  ~Position();

  /// Returns the current latitude.
  /// @return decimal representation of latitude.
  double latitude() const;

  /// Returns the current longitude.
  /// @return decimal representation of longitude.
  double longitude() const;

  /// Sets a new latitude
  /// @param lat latitude expressed in decimal degrees.
  void latitude(double lat);

  /// Sets a new longitude
  /// @param lon longitude expressed in decimal degrees.
  void longitude(double lon);

  /// Sets a new latitude and longitude
  /// @param lat latitude, and lon longitude expressed in decimal degrees.
  void set_position(double lat, double lon);

  /// Returns the distance (in km) between this Position object and the target
  /// Position object.
  /// @param target the Position object some distnace away from the current one
  /// @return distance between this and target in kilometers
  double Distance(Position target) const;

  /// Converts Position location into a string expression that follows ISO 6709
  /// Annex
  /// H. Function can be passed without any parameters for degrees format.
  /// @param return_format_
  /// @parblock_
  ///     The format of output. Options are 'Position::StringFormat::DEGREES',
  ///                                       'Position::StringFormat::DEGREES_
  ///                                                                MINUTES_',
  ///                                       'Position::StringFormat::DEGREES_
  ///                                                                MINUTES_
  ///                                    _                           SECONDS':
  ///
  ///     Position::StringFormat::DEGREES                :return in degrees
  ///                                                     format
  ///     Position::StringFormat::DEGREES_MINUTES        :return in degrees
  ///                                                     and minutes format
  ///     Position::StringFormat::DEGREES_MINUTES_SECONDS:return in degrees
  ///                                                     minutes seconds
  ///                                                     format
  /// @endparblock
  /// @return String representation of the Position object that complies with
  /// ISO6709 Annex H
  std::string ToString(
      Position::StringFormat format = StringFormat::DEGREES) const;

 private:
  /// Latitude is stored as seconds of degree. Explanation and example is
  /// available above in class documentation
  double latitude_;

  /// Longitude is stored as seconds of degree.
  double longitude_;

  /// Sets the precision for double values.
  /// @param value that requires change of precision
  /// @param precision the number decimal places wanted
  /// @return value with the number of decimal places (precision) as defined
  double SetPrecision(double value, double precision) const;

  /// Formats longitude for toString function
  /// @param mode
  /// @parblock
  ///       The format of output. Options are '1', '2' or '3' :
  ///
  ///     1:   return in degrees format
  ///     2:   return in degrees and minutes format
  ///     3:   return in degrees minutes seconds format
  /// @endparblock
  /// @param lon longitude of the object
  /// @return partially formatted string
  std::string ToStringHelperLon(int mode, double lon) const;

  /// Formats latitude for toString function
  /// @param mode
  /// @parblock
  ///       The format of output. Options are '1', '2' or '3' :
  ///
  ///     1:   return in degrees format
  ///     2:   return in degrees and minutes format
  ///     3:   return in degrees minutes seconds format
  /// @endparblock
  /// @param lat latitude of the object
  /// @return partially formatted string
  std::string ToStringHelperLat(int mode, double lat) const;

  /// Adds "+" or "-" sign for the ToStringHelperLon/Lat function
  /// @param value longitude or latitude of the object
  /// @returns "+" or "-" for the ToString function
  std::string ToStringHelper(double value) const;

  /// Formats longitude and latitude in degrees and minutes for the
  /// ToStringHelperLon/Lat function
  /// @param value longitude or latitude of the object
  /// @return formatted string representation of longiiiitude or latitude
  std::string ToStringHelperDM(double value) const;

  /// Formats longitude and latitude in degrees minutes and seconds for the
  /// ToStringHelperLon/Lat function
  /// @param value longitude or latitude of the object
  /// @return formatted string representation of longiiiitude or latitude
  std::string ToStringHelperDMS(double value) const;
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_POSITION_H_
