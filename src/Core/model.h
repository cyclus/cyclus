// model.h
#ifndef MODEL_H_
#define MODEL_H_

#include <map>
#include <set>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "dynamic_module.h"
#include "transaction.h"
#include "query_engine.h"

namespace cyclus {

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
     return a set of the types of dynamic modules
   */
  static std::set<std::string> dynamic_module_types();

  /**
     returns a model given the template's name

     @param name name of the template as defined in the input file
   */
  static Model* GetModelByName(std::string name);

  /**
     prints the current list of models
   */
  static void PrintModelList();

  /**
     returns the current list of models
   */
  static std::vector<Model*> GetModelList();

  /**
     load a dynamic module
     @param model_type the type of model
     @param module_name the name of the module
   */
  static void LoadModule(std::string model_type, std::string module_name);

  /**
     closes the library of each dynamically loaded module and erases
     it from the loaded modules container
   */
  static void UnloadModules();

  /**
     constructs and initializes an entity
     @param model_type the type of entity
     @param qe a pointer to a QueryEngine object containing initialization data
   */
  static void InitializeSimulationEntity(Context* ctx, std::string model_type, QueryEngine* qe);

  /**
     uses the loaded modules to properly construct a model
     @param model_impl the implementation to construct
     @return the constructed model
   */
  static Model* ConstructModel(Context* ctx, std::string model_impl);

  /**
     uses the loaded modules to properly destruct a model
     @param model the model to delete
   */
  static void DeleteModel(Model* model);

  /**
     register a model as a market
     @param market the model to register
   */
  static void RegisterMarketWithSimulation(Model* market);

  /**
     register a model as a region
     @param region the model to register
   */
  static void RegisterRegionWithSimulation(Model* region);

  /**
     constructs the simulation in its initial state
   */
  static void ConstructSimulation();

  /**
     Initialize members related to core classes
     @param qe a pointer to a QueryEngine object containing initialization data
   */
  virtual void InitCoreMembers(QueryEngine* qe);

  /**
     Initialize members related to derived module class
     @param qe a pointer to a QueryEngine object containing initialization data
   */
  virtual void InitModuleMembers(QueryEngine* qe) {};

  /**
     Constructor for the Model Class

     @warning all constructors must set ID_ and increment next_id_

   */
  Model(Context* ctx);

  /**
     Destructor for the Model Class
   */
  virtual ~Model();

  /**
     get model instance name
   */
  const std::string name() const {
    return name_;
  };

  /**
     set model instance name
   */
  void SetName(std::string name) {
    name_ = name;
  };

  /**
     get model instance SN
   */
  const int ID() const {
    return ID_;
  };

  /**
     get model implementation name
   */
  const std::string ModelImpl();

  /**
     set model implementation
   */
  void SetModelImpl(std::string new_impl) {
    model_impl_ = new_impl;
  };

  /**
     get model type
   */
  const std::string ModelType() {
    return model_type_;
  };

  /**
     set model type
   */
  void SetModelType(std::string new_type) {
    model_type_ = new_type;
  };

  Context* context() {
    return ctx_;
  };

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
  int ParentID() {
    return parentID_;
  };

  /**
     return the born on date of this model
   */
  int BornOn() {
    return bornOn_;
  };

  /**
     return the died on of this model
   */
  int DiedOn() {
    return diedOn_;
  };

  /**
     add a child to the list of children.

     This does NOT set the specified child's parent to be this model.
   */
  void AddChild(Model* child);

  /**
     add a child to the list of children
   */
  void RemoveChild(Model* child);

  /**
     Return the number of children the model has
   */
  int NChildren() {
    return children_.size();
  }

  /**
     recursively prints the parent-child tree
   */
  std::string PrintChildren();

  /**
     returns a vector of strings representing the parent-child tree
     at the node for Model m
     @param m the model node to base as the root of this print tree
   */
  std::vector<std::string> GetTreePrintOuts(Model* m);

  /**
     creates the parent-child link and invokes the core-level and
     module-level enter simulation methods
     @param parent this model's parent
   */
  void EnterSimulation(Model* parent);

  /**
     perform core-related tasks when entering the simulation
   */
  virtual void EnterSimulationAsCoreEntity();

  /**
     perform module-specific tasks when entering the simulation
   */
  virtual void EnterSimulationAsModule();

  /**
     sets the parent_ member
     @param parent the model to set parent_ to
   */
  virtual void SetParent(Model* parent);

  /**
     set the bornOn date of this model
   */
  void SetBornOn(int date) {
    bornOn_ = date;
  };

  /**
     return the ith child
   */
  Model* children(int i) {
    return children_[i];
  }

  /**
     Transacted resources are extracted through this method.

     @warning This method should never be directly invoked.  All
     resource transfers should take place using the
     Message.ApproveTransfer() method.

     @param order the transaction for which Resource(s) are to be prepared

     @return list of resources to be sent for this order
   */
  virtual std::vector<Resource::Ptr> RemoveResource(Transaction order);

  /**
     Transacted resources are received through this method.

     @warning This method should never be directly invoked.  All
     resource transfers should take place using the
     Message.ApproveTransfer() method.

     @param trans the transaction that corresponds with the materials
     being received

     @param manifest is the set of resources being
   */
  virtual void AddResource(Transaction trans,
                           std::vector<Resource::Ptr> manifest);

 protected:
  /**
     a map of loaded modules. all dynamically loaded modules are
     registered with this map when loaded.
   */
  static std::map< std::string, boost::shared_ptr<DynamicModule> >
  loaded_modules_;

  /**
     the set of loaded dynamic libraries
   */
  static std::vector<void*> dynamic_libraries_;

  /**
     a set of registered markets
   */
  static std::set<Model*> markets_;

  /**
     a set of registered regions
   */
  static std::set<Model*> regions_;

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
  static void load_facilitycatalog(std::string filename, std::string ns,
                                   std::string format);

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
  void RemoveFromList(Model* model, std::vector<Model*>& mlist);

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

 private:
  Context* ctx_;

  /**
     add an agent to the transactiont table
   */
  void AddToTable();

};
} // namespace cyclus
#endif

