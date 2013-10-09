// xml_file_loader.h
#if !defined(_XMLFILELOADER_H)
#define _XMLFILELOADER_H

#include <map>
#include <set>
#include <string>
#include <sstream>
#include <boost/shared_ptr.hpp>

#include "query_engine.h"
#include "xml_parser.h"

namespace cyclus {

class Context;

/**
   a class that encapsulates the methods needed to load input to
   a cyclus simulation from xml
 */
class XMLFileLoader {
 public:
  /**
     Constructor to create a new XML for loading. Defaults to using the main schema.
     @param ctx the simulation context
     @param load_filename The filename for the file to be loaded; defaults to
     an empty string
     @param use_main_schema whether or not to use the main schema to
  */
  XMLFileLoader(Context* ctx, const std::string load_filename = "",
                bool use_main_schema = true);

  /**
     @return the path to the main file schema (cyclus.rng)
   */
  std::string PathToMainSchema();

  /**
     applies a schema agaisnt the parser used by the file loader
     @param schema the schema representation
   */
  void ApplySchema(const std::stringstream& schema);

  /**
   Load an entire simulation from the inputfile.
   */
  void LoadAll();

  /**
     Method to load the simulation control parameters.
  */
  void LoadControlParams();

  /**
     Method to load recipes from either the primary input file
     or a recipeBook catalog.
  */
  void LoadRecipes();

  /**
     Method to load all dyamic modules
  */
  void LoadDynamicModules();

 protected:
  /// a map of module types to their paths in xml
  std::map<std::string, std::string> module_paths_;

  /**
     initialize the paths to modules
   */
  void initialize_module_paths();

  /**
     loads a file into a stringstream
     @param stream the stream to load the file into
     @param file the file name
   */
  void LoadStringstreamFromFile(std::stringstream& stream,
                                std::string file);

 private:
  /**
     dynamically load a set of modules
     @param type the type of module to load
     @param query_path the path to the set of module nodes
   */
  void LoadModulesOfType(std::string type, std::string query_path);

  /// loads a specific recipe
  void LoadRecipe(QueryEngine* qe);

  /// Fills out the cyclus.rng.in template with discovered modules.
  std::string BuildSchema();

  Context* ctx_;

  /// the input file name
  std::string file_;

  /// the parser
  boost::shared_ptr<XMLParser> parser_;
};
} // namespace cyclus

#endif
