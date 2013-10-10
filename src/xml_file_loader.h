// xml_file_loader.h
#if !defined(_XMLFILELOADER_H)
#define _XMLFILELOADER_H

#include <map>
#include <set>
#include <string>
#include <sstream>
#include <boost/shared_ptr.hpp>

#include "dynamic_module.h"
#include "query_engine.h"
#include "xml_parser.h"

namespace cyclus {

class Context;

/// Builds and returns a master cyclus input xml schema that includes the
/// sub-schemas defined by all installed cyclus modules (e.g. facility models).
/// This is used to validate simulation input files.
std::string BuildMasterSchema();

/// a class that encapsulates the methods needed to load input to
/// a cyclus simulation from xml
class XMLFileLoader {
 public:
  /// Constructor to create a new XML for loading. Defaults to using the main schema.
  /// @param ctx all input file configuration will be loaded into here
  /// @param load_filename The filename for the file to be loaded; defaults to
  /// an empty string
  XMLFileLoader(Context* ctx, const std::string load_filename = "");

  virtual ~XMLFileLoader();

  /// applies a schema agaisnt the parser used by the file loader
  /// @param schema the schema representation
  void ApplySchema(const std::stringstream& schema);

  /// Load an entire simulation from the inputfile.
  ///
  /// @param use_main_schema whether or not to use the main schema
  void LoadSim(bool use_main_schema = true);

  /// Method to load the simulation control parameters.
  void LoadControlParams();

  /// Method to load recipes from either the primary input file
  /// or a recipeBook catalog.
  void LoadRecipes();

 private:
  /// Creates all initial agent instances from the input file.
  void LoadInitialAgents();

  /// Method to load all dyamic modules
  void LoadDynamicModules();

  /// loads a specific recipe
  void LoadRecipe(QueryEngine* qe);

  /// a map of module types to their paths in xml
  std::map<std::string, std::string> schema_paths_;

  Context* ctx_;

  /// the input file name
  std::string file_;

  /// the parser
  boost::shared_ptr<XMLParser> parser_;

  /// a map of loaded modules. all dynamically loaded modules are
  /// registered with this map when loaded.
  std::map< std::string, DynamicModule*> modules_;
};
} // namespace cyclus

#endif
