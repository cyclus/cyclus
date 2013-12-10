// model.h
#ifndef MODEL_H_
#define MODEL_H_

#include <map>
#include <set>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "dynamic_module.h"
#include "resource.h"
#include "query_engine.h"
#include "exchange_context.h"

namespace cyclus {

class Material;
class GenericResource;
  
/**
   defines the possible model types

   @warning DO NOT manually set int values for the constants - other
   code will break.

   @todo consider changing to a vector of strings & consolidating with
   the model_type instance variable of the model class
 */
enum ModelType {REGION, INST, FACILITY, END_MODEL_TYPES};

/**
   @class Model

   @section desc Description
   The Model class is the abstract class used by all types of models
   that will be available for dynamic loading.  This common interface
   means that the basic process of loading and registering models can
   be implemented in a single place.
 */
class Model {
 public:
  /**
     return a set of the types of dynamic modules
   */
  static std::set<std::string> dynamic_module_types();

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

     @warning all constructors must set id_ and increment next_id_

   */
  Model(Context* ctx);

  /**
     Destructor for the Model Class
   */
  virtual ~Model();

  /**
     Return a newly created/allocated prototype that is an exact copy of this.
     This method should ONLY be impelemented by the LEAVES of the model
     inheritance hierarchy (i.e. not superclasses). It must call the
     superclass' InitFrom method with "this" as the argument.  All
     initialization/cloning operations must be done AFTER calling InitFrom.
     Example:

     @code
     class MyModelClass : virtual public Model {
       ...

       virtual Model* Clone() {
         MyModelClass* m = new MyModelClass(*this);
         m->InitFrom(this);

         // put custom initialization/cloning details here
         ...

         return m;
       };

       ...
     };
     @endcode
   */
  virtual Model* Clone() = 0;

  /**
     get model implementation name
   */
  const std::string ModelImpl();

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
  virtual void Deploy(Model* parent = NULL);

  /**
     decommissions the model, removing it from the simulation
   */
  virtual void Decommission();

  /**
     sets the parent_ member
     @param parent the model to set parent_ to
   */
  virtual void SetParent(Model* parent);

  /**
     return the ith child
   */
  Model* children(int i) {
    return children_[i];
  }

  /** @brief default implementation for material preferences */
  virtual void AdjustMatlPrefs(PrefMap<Material>::type& prefs) {};
  
  /** @brief default implementation for material preferences */
  virtual void AdjustGenRsrcPrefs(PrefMap<GenericResource>::type& prefs) {};

  /**
  Returns a module's xml rng schema for initializing from input files. All
  concrete models should override this method.
  */
  virtual std::string schema() {
    return "<text />\n";
  };

  /**
     get model instance name
   */
  inline const std::string name() const { return name_; }

  /**
     set model instance name
   */
  inline void SetName(std::string name) { name_ = name; }

  /**
     get model instance SN
   */
  inline const int id() const { return id_; }

  /**
     set model implementation
   */
  inline void SetModelImpl(std::string new_impl) {
    model_impl_ = new_impl;
  }

  /**
     get model type
   */
  inline const std::string ModelType() const { return model_type_; }

  /**
     set model type
   */
  inline void SetModelType(std::string new_type) {
    model_type_ = new_type;
  };

  /**
   Returns this model's current simulation context.
   */
  inline Context* context() const { return ctx_; }

  /**
     every model should be able to print a verbose description
   */
  virtual std::string str();

  /**
     return parent of this model
   */
  inline Model* parent() const { return parent_; }

  /**
     return the parent' id
   */
  inline const int parent_id() const { return parent_id_; }

  /**
     returns the time this model began operation (-1 if the model has never been
     deployed).
   */
  inline const int birthtime() const { return birthtime_; }

  /**
     returns the time this model ceased operation (-1 if the model is still
     operating).
   */
  inline const int deathtime() const { return deathtime_; }

  /**
     @return a list of children this model has
   */
  inline const std::vector<Model*>& children() const { return children_; }
  
 protected:
  /**
     A method that must be implemented by and only by classes in the model
     heirarchy that have been subclassed.  This method must call the
     superclass' InitFrom method. The InitFrom method should only initialize
     this class' members - not inherited state.

     @param m the model containing state that should be used to initialize this
     model.
  */
  virtual void InitFrom(Model* m);

  /**
     @brief adds model-specific information to an error message
   */
  virtual std::string InformErrorMsg(std::string msg);
  
 private:
  /**
     add an agent to the transactiont table
   */
  void AddToTable();

  /**
     used to remove model instance refs from static model lists
   */
  void RemoveFromList(Model* model, std::vector<Model*>& mlist);

  /**
     Stores the next available facility ID
   */
  static int next_id_;

  /**
     children of this model
   */
  std::vector<Model*> children_;

  /**
     parent of this model
   */
  Model* parent_;

  /**
     parent's ID of this model
     Note: we keep the parent id in the model so we can reference it
     even if the parent is deallocated.
   */
  int parent_id_;

  /**
     born on date of this model
   */
  int birthtime_;

  /**
     died on date of this model
   */
  int deathtime_;

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
  int id_;

  Context* ctx_;
};

} // namespace cyclus

#endif

