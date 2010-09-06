// Model.h
#if !defined(_MODEL_H)
#define _MODEL_H

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <libxml/tree.h>

using namespace std;

class Model;

typedef Model* mdl_ctor();
typedef void mdl_dtor(Model*);


/*!
 * The Model class is the abstract class used by all types of models
 * that will be available for dynamic loading.  This common interface
 * means that the basic process of loading and registering models can
 * be implemented in a single place.
 *
 * To allow serialization of different types of models in separate
 * ID space, this interface is inherited by type-specific abstract
 * classes, such as MarketModel, that has its own static integer
 * to keep track of the next available ID.
 */
class Model
{
public:
  /**
   * Adds a model to the simulation
   *
   * @param model_type is the type (region, inst, facility, market...) to add
   * @param model_name is the model name (NullFacility, StubMarket, ...) to add
   */
  static mdl_ctor* load(string model_type,string model_name);

  /**
   * Creates a model for use in the simulation
   *
   * @param model_type is the type (region, inst, facility, market...) to create
   * @param cur is the pointer to the xml input representing the model to create
   */
  static Model* create(string model_type, xmlNodePtr cur);

  /** 
   * Create a new model object based on an existing one
   *
   * @param src a pointer to the original Model to be mimicked
   */
  static Model* create(Model* src);

  /**
   * Destroy a model cleanly
   * 
   * @param model a pointer to the model being destroyed
   */
  static void* destroy(Model* model);
  
  /**
   * A method to initialize the model
   *
   * @param cur the pointer to the xml input for the model to initialize
   */
  virtual void init(xmlNodePtr cur);

  /**
   * A method to copy a model
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copy(Model* src);

  /**
   * Constructor for the Model Class
   */
  Model() {};

  /**
   * Destructor for the Model Class
   */
  virtual ~Model() {};

  /**
   * get model instance name
   */
  const string getName() const { return name; };

  /**
   * get model instance SN
   */
  const int getSN() const { return ID; };

  /**
   * get model implementation name
   */
  const string getModelName() { return modelImpl; };

  /**
   * every model should be able to print a verbose description
   */
  virtual void print();

  /**
   * loads the market models available to the simulation
   */
  static void load_markets();

  /**
   * loads the converter models available to the simulation
   */
  static void load_converters();

  /** 
   * loads the facility models available to the simulation
   */
  static void load_facilities();

  /**
   * loads the facilities specified in a file
   *
   * @param filename the name of the file holding the facility specification
   * @param ns the string to append to the current namespace modifier
   * @param format the format of the file (currently cyclus supports only xml)
   */
  static void load_facilitycatalog(string filename, string ns, string format);

  /**
   * loads the regions available to the simulation
   */
  static void load_regions();

  /**
   * loads the institutions available to the simulation
   */
  static void load_institutions();

protected:
  /**
   * every instance of a model should have a name
   */
  string name;

  /** 
   * every instance of a model should know its type
   */
  string model_type;

  /**
   * every instance of a model should know its implementation
   */
  string modelImpl;

  /**
   * every instance of a model will have a serialized ID
   */
  int ID;

private:    
  /**
   * map of constructor methods for each loaded model
   */
  static map<string, mdl_ctor*> create_map;

  /**
   * map of destructor methods for each loaded model
   */
  static map<string, mdl_dtor*> destroy_map;
  
};


#endif



