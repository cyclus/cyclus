// This includes the required header to add geographic coordinates to a
// archetypes.
// One only need to:
// - '#include "toolkit/position.cycpp.h"' in the core of the archetype class (as
// private)
// - and in the EnterNotify() method:
//   - set the coordinates 'coordinates = cyclus::toolkit::Position(latitude,
//   longitude);'
//   - call the record method: 'coordinates.RecordPosititon(this);'

cyclus::toolkit::Position coordinates;

#pragma cyclus var { \
"default": 0.0, \
"uilabel": "Geographical latitude in degrees as a double", \
"doc": "Latitude of the agent's geographical position. The value should " \
       "be expressed in degrees as a double." \
}
double latitude;

#pragma cyclus var { \
"default": 0.0, \
"uilabel": "Geographical longitude in degrees as a double", \
"doc": "Longitude of the agent's geographical position. The value should " \
       "be expressed in degrees as a double." \
}
double longitude;

protected:

  void InitializePosition(cyclus::Agent* agent) {
    coordinates.set_position(latitude, longitude);
    coordinates.RecordPosition(agent);
  }

// required for compilation but not added by the cycpp preprocessor...
std::vector<int> cycpp_shape_latitude = {0};
std::vector<int> cycpp_shape_longitude = {0};
