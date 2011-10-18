// Model.h
#if !defined(_MODEL_H)
#define _MODEL_H

#include <map>
#include <string>
#include <libxml/tree.h>

class Model;

typedef Model* mdl_ctor();
typedef void mdl_dtor(Model*);

/*! 
 * @brief defines the possible model types
 * 
 * @warning DO NOT manually set int values for the constants - other code 
 *          will break.
 *
 * @todo consider changing to a vector of strings & consolidating with
 *       the model_type instance variable of the model class
 */
enum ModelType {REGION, INST, FACILITY, MARKET, CONVERTER, END_MODEL_TYPES};

/*!
 * The Model class is the abstract class used by all types of models
 * that will be available for dynamic loading.  This common interface
 * means that the basic process of loading and registering models can
 * be implemented in a single place.
 *
 * To allow serialization of different types of models in unified
 * ID space, this interface is inherited by type-specific abstract
 * classes, such as MarketModel, that has its own static integer
 * to keep track of the next available ID.
 *
 * @warning all constructors must set ID_ and increment next_id_
 * 
 */
class Model {
public:
  /**
   * @brief Creates a model instance for use in the simulation
   *
   * @param model_type model type (region, inst, facility, ...) to create
   * @param cur pointer to the xml input representing the model to create
   */
  static Model* create(std::string model_type, xmlNodePtr cur);

  /** 
   * @brief Create a new model object based on an existing one
   *
   * @param model_orig a pointer to the original Model to be mimicked
   */
  static Model* create(Model* model_orig);

  /**
   * @brief dynamically loads a model constructor from a shared object file
   *
   * @param model_type model type (region, inst, facility, ...) to add
   * @param model_name name of model (NullFacility, StubMarket, ...) to add
   */
  static mdl_ctor* loadConstructor(std::string model_type, std::string model_name);

  /**
   * @brief Destroy a model cleanly
   * 
   * @param model a pointer to the model being destroyed
   */
  static void* destroy(Model* model);
  
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
   * @param filename name of the file holding the facility specification
   * @param ns the string to append to the current namespace modifier
   * @param format format of the file (currently cyclus supports only xml)
   */
  static void load_facilitycatalog(std::string filename, std::string ns, std::string format);

  /**
   * loads the regions available to the simulation
   */
  static void load_regions();

  /**
   * loads the institutions available to the simulation
   */
  static void load_institutions();

  /**
   * A method to initialize the model
   *
   * @param cur the pointer to the xml input for the model to initialize
   */
  virtual void init(xmlNodePtr cur);

  /**
   * A method to copy a model
   *
   * @param model_orig pointer to the original (usu initialized) model to be copied
   */
  virtual void copy(Model* model_orig);

  /**
   * Drills down the dependency tree to initialize all relevant 
   * parameters/containers.
   *
   * Note that this function must be defined only in the specific model in 
   * question and not in any inherited models preceding it.
   *
   * @param model_orig pointer to (usu initialized) model to be copied
   */
  virtual void copyFreshModel(Model* model_orig)=0;

  /**
   * Constructor for the Model Class
   * 
   * @warning all constructors must set ID_ and increment next_id_
   * 
   */
  Model();

  /**
   * Destructor for the Model Class
   */
  virtual ~Model();

  /**
   * get model instance name
   */
  const std::string getName() const { return name_; };

  /**
   * get model instance SN
   */
  const int getSN() const { return ID_; };

  /**
   * set model instance SN
   */
  void setSN(int new_id) { ID_ = new_id; };

  /**
   * get model implementation name
   */
  const std::string getModelImpl() {return model_impl_; };

  /**
   * get model type
   */
  const std::string getModelType() { return model_type_; };

  /**
   * set model type
   */
  void setModelType(std::string new_type) { model_type_ = new_type; };

  /**
   * get model instance handle
   */
  const std::string getHandle() const { return handle_; };

  /**
   * every model should be able to print a verbose description
   */
  virtual void print();

private:
  /// Stores the next available facility ID
  static int next_id_;

  /**
   * map of constructor methods for each loaded model
   */
  static std::map<std::string, mdl_ctor*> create_map_;

  /**
   * map of destructor methods for each loaded model
   */
  static std::map<std::string, mdl_dtor*> destroy_map_;
  
  /**
   * every instance of a model should have a handle
   * perhaps this is redundant with name. Discuss amongst yourselves.
   */
  std::string handle_;

  /**
   * generate model handle
   */
  std::string generateHandle();

  /**
   * every instance of a model should have a name
   */
  std::string name_;

  /** 
   * every instance of a model should know its type
   */
  std::string model_type_;

  /**
   * every instance of a model should know its implementation
   */
  std::string model_impl_;

  /**
   * every instance of a model will have a serialized ID
   */
  int ID_;

};

#endif

