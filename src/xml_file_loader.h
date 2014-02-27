// xml_file_loader.h
#ifndef CYCLUS_XML_FILE_LOADER_H_
#define CYCLUS_XML_FILE_LOADER_H_

#include <map>
#include <string>
#include <sstream>
#include <boost/shared_ptr.hpp>

#include "composition.h"
#include "dynamic_module.h"
#include "query_engine.h"
#include "xml_parser.h"

namespace cyclus {

class Context;

/// Reads the given file path into the passed stream.
void LoadStringstreamFromFile(std::stringstream& stream, std::string file);

/// Builds and returns a master cyclus input xml schema that includes the
/// sub-schemas defined by all installed cyclus modules (e.g. facility models).
/// This is used to validate simulation input files.
std::string BuildMasterSchema(std::string schema_path);

/// Creates a composition from the recipe in the query engine.
Composition::Ptr ReadRecipe(QueryEngine* qe);

/// a class that encapsulates the methods needed to load input to
/// a cyclus simulation from xml
class XMLFileLoader {
 public:
  /// Constructor to create a new XML for loading. Defaults to using the main schema.
  /// @param ctx all input file configuration will be loaded into here
  /// @param load_filename The filename for the file to be loaded; defaults to
  /// an empty string
  XMLFileLoader(Context* ctx, std::string schema_path,
                const std::string load_filename = "");

  virtual ~XMLFileLoader();

  /// applies a schema agaisnt the parser used by the file loader
  /// @param schema the schema representation
  void ApplySchema(const std::stringstream& schema);

  /// Load an entire simulation from the inputfile.
  ///
  /// @param use_flat_schema whether or not to use the flat schema
  virtual void LoadSim();

  /// Method to load the simulation exchange solver.
  void LoadSolver();

  /// Method to load the simulation control parameters.
  void LoadControlParams();

  /// Method to load recipes from either the primary input file
  /// or a recipeBook catalog.
  void LoadRecipes();

  /// Creates all initial agent instances from the input file.
  virtual void LoadInitialAgents();

 protected:
  virtual std::string master_schema();

  /// Processes commodity orders, such that any without a defined order (i.e.,
  /// are nonpositive), are given an order value greater the last known
  /// commodity
  void ProcessCommodities(std::map<std::string, double>* commodity_order);

  /// Method to load all dyamic modules
  void LoadDynamicModules();

  Context* ctx_;

  /// filepath to the schema
  std::string schema_path_;

  /// a map of module types to their paths in xml
  std::map<std::string, std::string> schema_paths_;

  /// the parser
  boost::shared_ptr<XMLParser> parser_;

  /// a map of loaded modules. all dynamically loaded modules are
  /// registered with this map when loaded.
  std::map< std::string, DynamicModule*> modules_;

  /// loads a specific recipe
  void LoadRecipe(QueryEngine* qe);

  /// the input file name
  std::string file_;

};
} // namespace cyclus

#endif // ifndef CYCLUS_XML_FILE_LOADER_H_

