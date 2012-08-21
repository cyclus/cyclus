// XMLFileLoader.h
#if !defined(_XMLFILELOADER_H)
#define _XMLFILELOADER_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/relaxng.h>

class XMLFileLoader {

 private:

  enum CatalogType {
    recipeBook,
    facilityCatalog
  };

  static std::string main_schema_;
  static std::string recipe_book_schema_;
  static std::string facility_catalog_schema_;

  std::string filename;
  xmlDocPtr doc;

  /**
     Internal method to invoke the ModelFactory for each 

     @params modelPath XMLQueryEngine path to the search for model definitions
     @params factoryType type indication to pass to ModelFactory
  */
  void load_models(std::string modelPath, std::string factoryType);

 public:

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
     Method to load some kind of catalog of a type defined in
     CatalogType.  Since catalog loading can be recursive, a namespace
     is passed in and potentially extended.

     @param catalogElement An XML snippet with the definition of the catalog

     @param catalogType An enum variable indicating the type of this
     
     @param cur_ns The current namespace for entries from this catalog
  */
  void load_catalog(std::string catalogElement, CatalogType catalogType, std::string cur_ns);

  /** 
      Method to load recipes from either the primary input file or a recipeBook catalog.

      @param cur_ns the current namespace for entries being registered here
  */
  void load_recipes(std::string cur_ns);

  /** 
      Method to load facilities from either the primary input file or a facilityCatalog catalog.

      @param cur_ns the current namespace for entries being registered here
  */
  void load_facilities(std::string cur_ns);

  /**
     Method to load all the agent models in a given order.

   */
  void load_all_models();

  /**
     Method to load the simulation control parameters.
  */
  void load_params();

 protected:


};


#endif
