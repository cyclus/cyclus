// Model.h
#if !defined(_MODEL_H)
#define _MODEL_H

#include <map>
#include <string>
#include <libxml/tree.h>
#include <vector>

#include "Transaction.h"

class Model;
class Message;
class Transaction;

typedef Model* mdl_ctor();

/** 
   defines the possible model types 
    
   @warning DO NOT manually set int values for the constants - other 
   code will break. 
    
   @todo consider changing to a vector of strings & consolidating with 
   the model_type instance variable of the model class 
 */
enum ModelType {REGION, INST, FACILITY, MARKET, CONVERTER, END_MODEL_TYPES};

/**
   @class Model 
    
   @section desc Description 
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
     returns a model template given the template's name 
      
     @param name name of the template as defined in the input file 
   */
  static Model* getTemplateByName(std::string name);

  /**
     returns a model given the template's name 
      
     @param name name of the template as defined in the input file 
   */
  static Model* getModelByName(std::string name);

  /**
     prints the current list of models 
   */
  static void printModelList();

  /**
     returns the current list of models 
   */
  static std::vector<Model*> getModelList();

  /**
     dynamically loads a model constructor from a shared object file 
     
     as this is system dependent, a function is provided in the 
     LoadConstructor files in the src/Core/Config folder.
      
     @param model_type model type (region, inst, facility, ...) to add 
     @param model_name name of model (NullFacility, StubMarket, ...) to 
   */
  static mdl_ctor* loadConstructor(std::string model_type, std::string model_name);

  /**
     provides a constructed simulation entity
     @param model_type the type of entity
     @param cur the model snippet position in input
     @return a pointer to the construced entity
   */
  static Model* getEntityViaConstructor(std::string model_type,xmlNodePtr cur);

  /**
     constructs and initializes an entity
     @param model_type the type of entity
     @param cur the initialization snippet position in input
   */
  static void initializeSimulationEntity(std::string model_type, xmlNodePtr cur);

  /**
     constructs and initializes a prototype
     @param model_type the type of entity
     @param cur the initialization snippet position in input
   */
  static void initializePrototype(std::string model_type, xmlNodePtr cur);

  /**
     loads all globally accessible elements
   */
  static void loadGlobalElements();

  /**
     loads all simulation entities
   */
  static void loadEntities();

  /**
     load all facilities 
   */
  static void load_facilities();

  /**
     Constructor for the Model Class 
      
     @warning all constructors must set ID_ and increment next_id_ 
      
   */
  Model();

  /**
     A method to initialize the model 
      
     @param cur the pointer to the xml input for the model to initialize 
   */
  virtual void init(xmlNodePtr cur);

  /**
     Destructor for the Model Class 
   */
  virtual ~Model();

  /**
     get model instance name 
   */
  const std::string name() const { return name_; };

  /**
     set model instance name 
   */
  void setName(std::string name) { name_ = name; };

  /**
     get model instance SN 
   */
  const int ID() const { return ID_; };

  /**
     get model implementation name 
   */
  const std::string modelImpl();

  /**
     set model implementation 
   */
  void setModelImpl(std::string new_impl) { model_impl_ = new_impl; };

  /**
     get model type 
   */
  const std::string modelType() { return model_type_; };

  /**
     set model type 
   */
  void setModelType(std::string new_type) { model_type_ = new_type; };

  /**
     every model should be able to print a verbose description 
   */
  virtual std::string str();

  /**
     return parent of this model 
   */
  Model* parent();

  /**
     return the parent' id 
   */
  int parentID() {return parentID_;};

  /**
     return the born on date of this model 
   */
  int bornOn() {return bornOn_;};

  /**
     return the died on of this model 
   */
  int diedOn() {return diedOn_;};

  /**
     add a child to the list of children.

     This does NOT set the specified child's parent to be this model.
   */
  void addChild(Model* child);

  /**
     add a child to the list of children 
   */
  void removeChild(Model* child);

  /**
     Return the number of children the model has 
   */
  int nChildren() {return children_.size();}

  /**
     recursively prints the parent-child tree
   */
  std::string printChildren();

  /**
     returns a vector of strings representing the parent-child tree
     at the node for Model m
     @param m the model node to base as the root of this print tree
   */
  std::vector<std::string> getTreePrintOuts(Model* m);

  /**
     calls setParent() and sets other model-specific members
   */
  virtual void enterSimulation(Model* parent);

  /**
     sets the parent_ member
     @param parent the model to set parent_ to
   */
  virtual void setParent(Model* parent);

  /**
     set the bornOn date of this model 
   */
  void setBornOn(int date) {bornOn_ = date;};

  /**
     return the ith child 
   */
  Model* children(int i){return children_[i];}

  /**
     Transacted resources are extracted through this method. 
      
     @warning This method should never be directly invoked.  All 
     resource transfers should take place using the 
     Message.approveTransfer() method.  

     @param order the transaction for which resource(s) are to be prepared 

     @return list of resources to be sent for this order 
   */ 
  virtual std::vector<rsrc_ptr> removeResource(Transaction order);

  /**
     Transacted resources are received through this method. 
      
     @warning This method should never be directly invoked.  All 
     resource transfers should take place using the 
     Message.approveTransfer() method.  

     @param trans the transaction that corresponds with the materials 
     being received

     @param manifest is the set of resources being 
   */ 
  virtual void addResource(Transaction trans,
                              std::vector<rsrc_ptr> manifest);

  /**
     Asks if a model can build a certain prototype. Returns false by
     default.
   */
  virtual bool canBuild(Model* prototype) {return false;}
    
 protected:
  /**
     children of this model 
   */
  std::vector<Model*> children_;

  /**
     parent of this model 
   */
  Model* parent_;

 private:
  /**
     loads the facilities specified in a file 
      
     @param filename name of the file holding the facility specification 
     @param ns the string to append to the current namespace modifier 
     @param format format of the file (currently cyclus supports only 
   */
  static void load_facilitycatalog(std::string filename, std::string ns, std::string format);

  /**
     load all markets 
   */
  static void load_markets();

  /**
     load all converters 
   */
  static void load_converters();

  /**
     load all regions 
   */
  static void load_regions();

  /**
     load all institutions 
   */
  static void load_institutions();

  /**
     Stores the next available facility ID 
   */
  static int next_id_;

  /**
     comprehensive list of all initialized models. 
   */
  static std::vector<Model*> model_list_;

  /**
     used to remove model instance refs from static model lists 
   */
  void removeFromList(Model* model, std::vector<Model*> &mlist);

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
     every instance of a model should have a name 
   */
  std::string name_;

  /** 
     every instance of a model should know its type 
   */
  std::string model_type_;

  /**
     every instance of a model should know its implementation 
   */
  std::string model_impl_;

  /**
     every instance of a model will have a serialized ID 
   */
  int ID_;

  /**
     wheter or not the model has been born 
   */
  bool born_;

// -------- output database related members  -------- 
 public:
  /**
     the agent database table 
   */
  static table_ptr agent_table;
  
  /**
     return the agent table's primary key 
   */
  primary_key_ref pkref(){ return pkref_;}

 private:
  /**
     Define the database table on the first Message's init 
   */
  static void define_table();

  /**
     add an agent to the transactiont table 
   */
  void addToTable();

  /**
     Store information about the transactions's primary key 
   */
  primary_key_ref pkref_;
// -------- output database related members  --------   
};

#endif

