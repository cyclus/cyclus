// XMLFileLoader.h
#if !defined(_XMLFILELOADER_H)
#define _XMLFILELOADER_H

#include <map>
#include <set>
#include <string>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include "XMLParser.h"

/**
   a class that encapsulates the methods needed to load input to
   a cyclus simulation from xml
 */
class XMLFileLoader {
 public:    
  /**
     Constructor to create a new XML for loading
     @param load_filename The filename for the file to be loaded
     @param use_main_schema whether or not to use the main schema to 
     validate the file
  */
  XMLFileLoader(std::string load_filename, bool use_main_schema=true);
  
  /**
     @return the path to the main file schema (cyclus.rng)
   */
  std::string pathToMainSchema();

  /**
     applies a schema agaisnt the parser used by the file loader
     @param schema the schema representation
   */
  void applySchema(std::stringstream &schema);

  /**
     Method to load the simulation control parameters.
  */
  void load_control_parameters();

  /**
     Method to load recipes from either the primary input file 
     or a recipeBook catalog.
  */
  void load_recipes();
  
  /**
     Method to load all dyamic modules
     @params module_types the set of all types of modules
  */
  void load_dynamic_modules(std::set<std::string>& module_types);

  /**
     dynamically load a set of modules
     @param type the type of module to load
     @param query_path the path to the set of module nodes
   */
  void load_modules_of_type(std::string type, std::string query_path);
  
 protected:
  /// a map of module types to their paths in xml
  std::map<std::string,std::string> module_paths_;
  
  /**
     initialize the paths to modules given a set of module types
     @param module_types the set of module types
   */
  void initialize_module_paths();

  /**
     loads a file into a stringstream
     @param stream the stream to load the file into
     @param file the file name
   */
  void loadStringstreamFromFile(std::stringstream &stream,
                                std::string file);

 private:
  /// the parser
  boost::shared_ptr<XMLParser> parser_;
};


#endif
