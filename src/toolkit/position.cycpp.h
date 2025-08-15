// This includes the required header to add geographic coordinates to a
// archetypes.
// One only needs to:
// - '#include "toolkit/position.cycpp.h"' in the header of the archetype class
//   (it is strongly recommended to inject this snippet as `private:`, but
//   archetype developers are free to make other choices)
// - In the EnterNotify() method:
//   - Call the InitializePosition(); function

cyclus::toolkit::Position coordinates;

// clang-format off
#pragma cyclus var { \
       "default": 0.0, \
       "uilabel": "Geographical latitude in degrees as a double", \
       "doc": "Latitude of the agent's geographical position. The value should " \
       "be expressed in degrees as a double." \
}
double latitude = 0.0;

#pragma cyclus var { \
       "default": 0.0, \
       "uilabel": "Geographical longitude in degrees as a double", \
       "doc": "Longitude of the agent's geographical position. The value should " \
              "be expressed in degrees as a double." \
}
double longitude = 0.0;
// clang-format on

// Provided default values to give the option to manually override
void InitializePosition() {
  coordinates.set_position(latitude, longitude);
  coordinates.RecordPosition(this);
}

// required for compilation but not added by the cycpp preprocessor...
std::vector<int> cycpp_shape_latitude = {0};
std::vector<int> cycpp_shape_longitude = {0};
