
// This includes the required header to add geographic coordinates to a
// archetypes.
// One only need to:
// - '#include "toolkit/metadata.cycpp"' in the core of the archetype class (as
// private)
// - and in the EnterNotify() method:
//   - set the metadata 
//      'coordinates = cyclus::toolkit::Position(latitude,longitude);'

#pragma cyclus var { \
    "alias": ["usagemetadata", "keyword", ["usage", "key", "value"]], \
    "uitype": ["onemore", "string", ["onemore", "string", "double"]], \
    "uilabel": "", \
    "doc": "", \
  }
  std::map<std::string, std::map<std::string, double> > usage_metadata_;
// required for compilation but not added by the cycpp preprocessor...
  std::vector<int> cycpp_shape_usage_metadata_;

#pragma cyclus var { \
    "alias": ["metadata", "key", "value"], \
    "uitype": ["onemore", "string", "string"], \
    "uilabel": "", \
    "doc": "", \
  }
  std::map<std::string, std::string> metadata_;
// required for compilation but not added by the cycpp preprocessor...
  std::vector<int> cycpp_shape_metadata_;

  cyclus::toolkit::Metadata metadata; 
