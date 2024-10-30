
// This includes the required header to metadata to a
// archetypes.
// One only need to:
// - '#include "toolkit/metadata.cycpp"' in the core of the archetype class (as
// private)
// - and in the EnterNotify() method:
//   - set the metadata 
//      'metadata.SetWorkLabel(work_label);
//      'metadata.LoadData(metadata_);
//      'metadata.LoadData(usage_metadata_);


#pragma cyclus var { \
    "default": {}, \
    "alias": ["usagemetadata", "keyword", ["usage", "key", "value"]], \
    "uitype": ["oneormore", "string", ["oneormore", "string", "double"]], \
    "uilabel": "", \
    "doc": "", \
  }
  std::map<std::string, std::map<std::string, double> > usage_metadata_;
// required for compilation but not added by the cycpp preprocessor...
  std::vector<int> cycpp_shape_usage_metadata_;

#pragma cyclus var { \
    "default": {}, \
    "alias": ["metadata", "key", "value"], \
    "uitype": ["oneormore", "string", "string"], \
    "uilabel": "", \
    "doc": "", \
  }
  std::map<std::string, std::string> metadata_;
//// required for compilation but not added by the cycpp preprocessor...
  std::vector<int> cycpp_shape_metadata_;
  std::string work_label;

