// XMLFileLoader.h
#if !defined(_XMLFILELOADER_H)
#define _XMLFILELOADER_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/relaxng.h>
#include <map>

// forward declarations
class QueryEngine;

/**
   a class that encapsulates the methods needed to load input to
   a cyclus simulation from xml
 */
class XMLFileLoader {
 public:
  /// the schema to validate the file against
  static std::string main_schema_;
  
  /// empty constructor 
  XMLFileLoader() {};
  
  /**
     Constructor to create a new XML for loading
     @param load_filename The filename for the file to be loaded
  */
  XMLFileLoader(std::string load_filename);
  
  /**
     method to allow for RNG validation of an open file using
     @param schema_file The filename of the schema to use for validation
  */
  void validate_file(std::string schema_file);

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
  
 protected:
  /// a map of module types to their paths in xml
  std::map<std::string,std::string> module_paths_;
  
  /**
     initialize the paths to modules given a set of module types
     @param module_types the set of module types
   */
  void initialize_module_paths();

  /**
     dynamically load a set of modules
     @param type the type of module to load
     @param query_path the path to the set of module nodes
   */
  void load_modules_of_type(std::string type, std::string query_path);

 private:
  /// the name of the input file
  std::string filename_;

  /// the main document entry point
  xmlDocPtr doc_;
};


#endif
