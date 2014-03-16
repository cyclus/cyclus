// xml_flat_loader.h
#ifndef CYCLUS_SRC_XML_FLAT_LOADER_H_
#define CYCLUS_SRC_XML_FLAT_LOADER_H_

#include "xml_file_loader.h"

namespace cyclus {

/// Builds and returns a master cyclus input xml schema defining a flat
/// prototype and instance structure that includes the sub-schemas defined by
/// all installed cyclus modules (e.g. facility models).  This is used to
/// validate simulation input files.
std::string BuildFlatMasterSchema(std::string schema_path);

/// a class that encapsulates the methods needed to load input to
/// a cyclus simulation from xml
class XMLFlatLoader : public XMLFileLoader {
 public:
  XMLFlatLoader(Recorder* r, QueryBackend* b, std::string schema_path,
                const std::string load_filename = "")
      : XMLFileLoader(r, b, schema_path, load_filename) {}

  /// Creates all initial agent instances from the input file.
  void LoadInitialAgents();

 protected:
  virtual std::string master_schema();
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_XML_FLAT_LOADER_H_
