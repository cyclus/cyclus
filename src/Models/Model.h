// Model.h
#if !defined(_MODEL_H)
#define _MODEL_H

#include <map>
#include <string>
#include <libxml/tree.h>
#include <vector>

#include "Message.h"
#include "Resource.h"
#include "Table.h"

#include "boost/intrusive_ptr.hpp"

class Model;
class Message;
struct Transaction;

typedef Model* mdl_ctor();
typedef boost::intrusive_ptr<Message> msg_ptr;

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
 The Model class is the abstract class used by all types of models
 that will be available for dynamic loading.  This common interface
 means that the basic process of loading and registering models can
 be implemented in a single place.
 
 To allow serialization of different types of models in unified
 ID space, this interface is inherited by type-specific abstract
 classes, such as MarketModel, that has its own static integer
 to keep track of the next available ID.
 
 @warning all constructors must set ID_ and increment next_id_
 
 */
class Model {
public:
  /**
     @brief returns a model template given the template's name
     
     @param name name of the template as defined in the input file
   */
  static Model* getTemplateByName(std::string name);

  /**
     @brief returns a model given the template's name
     
     @param name name of the template as defined in the input file
   */
  static Model* getModelByName(std::string name);

  static void printModelList();

  static std::vector<Model*> getModelList();

  /**
   * @brief Creates a model instance for use in the simulation
   *
   * @param model_type model type (region, inst, facility, ...) to create
   * @param cur pointer to the xml input representing the model to create
   */
  static void create(std::string model_type, xmlNodePtr cur);

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
   * loads all models appropriately ordered by type
   */
  static void load_models();

  static void load_facilities();

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
  const std::string name() const { return name_; };

  /**
   * set model instance name
   */
  void setName(std::string name) { name_ = name; };

  /**
   * get model instance SN
   */
  const int ID() const { return ID_; };

  /**
   * get model implementation name
   */
  const std::string modelImpl();

  /**
   * get model type
   */
  const std::string modelType() { return model_type_; };

  /**
   * set model type
   */
  void setModelType(std::string new_type) { model_type_ = new_type; };

  /// every model should be able to print a verbose description
  virtual void print();

  /// return parent of this model
  Model* parent();

  /// return the parent' id
  int parentID() {return parentID_;};

  /// return the born on date of this model
  int bornOn() {return bornOn_;};

  /// return the died on of this model
  int diedOn() {return diedOn_;};

  /// add a child to the list of children
  void addChild(Model* child);

  /// add a child to the list of children
  void removeChild(Model* child);

  /// Return the number of children the model has
  int nChildren() {return children_.size();}

  /// set the parent of this model (default)
  void setParent(Model* parent);

  /// set the parent of this model (for testing)
  void setParent(Model* parent, bool log);

  /// set the bornOn date of this model
  void setBornOn(int date) {bornOn_ = date;};

  /// return the ith child
  Model* children(int i){return children_[i];}

  /*!
  @brief Transacted resources are extracted through this method.

  @warning This method should never be directly invoked.  All resource
  transfers should take place using the Message.approveTransfer() method.
  
  @param order the msg/order for which resource(s) are to be prepared
  @return list of resources to be sent for this order
  
  */ 
  virtual std::vector<rsrc_ptr> removeResource(msg_ptr order);

  /*!
  Transacted resources are received through this method.
 
  @warning This method should never be directly invoked.  All resource
  transfers should take place using the Message.approveTransfer() method.
  
  @param msg the sent message that corresponds with the materials being received
  @param manifest is the set of resources being received
  */ 
  virtual void addResource(msg_ptr msg,
                              std::vector<rsrc_ptr> manifest);

  bool isTemplate() {return is_template_;};
  void setIsTemplate(bool is_template);
    
protected:
  /// children of this model
  std::vector<Model*> children_;

private:

  /*!
   loads the facilities specified in a file
   
   @param filename name of the file holding the facility specification
   @param ns the string to append to the current namespace modifier
   @param format format of the file (currently cyclus supports only xml)
  */
  static void load_facilitycatalog(std::string filename, std::string ns, std::string format);

  static void load_markets();

  static void load_converters();

  static void load_regions();

  static void load_institutions();

  /// Stores the next available facility ID
  static int next_id_;

  /// comprehensive list of all templated models.
  static std::vector<Model*> template_list_;

  /// comprehensive list of all initialized models.
  static std::vector<Model*> model_list_;

  /// used to remove model instance refs from static model lists
  void removeFromList(Model* model, std::vector<Model*> &mlist);

  /**
   * parent of this model
   */
  Model* parent_;

  /**
     parent's ID of this model
     Note: we keep the parent id in the model so we can reference it
     even if the parent is deallocated.
   */
  int parentID_;

  /**
     born on date of this model
   */
  int bornOn_;

  /**
     died on date of this model
   */
  int diedOn_;
  
  /**
   * map of constructor methods for each loaded model
   */
  static std::map<std::string, mdl_ctor*> create_map_;

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

  bool is_template_;

  bool born_;

  /*
    output database info
  */
 public:
  // the database table and related information
  static table_ptr agent_table;
  
  /*!
    return the agent table's primary key
  */
  primary_key_ref pkref(){ return pkref_;}

 private:
  /*!
    Define the database table on the first Message's init
   */
  static void define_table();

  // add an agent to the transactiont table
  void addToTable();

  /*!
    Store information about the transactions's primary key
   */
  primary_key_ref pkref_;
  
};

#endif

