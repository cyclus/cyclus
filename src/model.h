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
#include "query_backend.h"
#include "db_init.h"

#define SHOW(X) \
  std::cout << __FILE__ << ":" << __LINE__ << ": "#X" = " << X << "\n"

namespace cyclus {

class Material;
class GenericResource;
  
/// defines the possible model types
///
/// @warning DO NOT manually set int values for the constants - other
/// code will break.
///
/// @todo consider changing to a vector of strings & consolidating with
/// the model type instance variable of the model class
enum ModelType {REGION, INST, FACILITY, END_MODEL_TYPES};

/// map<internal-inventory-name, vector<resources-inside-inventory> >
typedef std::map<std::string, std::vector<Resource::Ptr> > Inventories;

/// @class Model
///
/// @section desc Description
/// The Model class is the abstract class used by all types of models
/// that will be available for dynamic loading.  This common interface
/// means that the basic process of loading and registering models can
/// be implemented in a single place.
class Model {
  friend class SimInit;

 public:
  /// Constructor for the Model Class
  ///
  /// @warning all constructors must set id_ and increment next_id_
  Model(Context* ctx);

  /// Recursively destructs all children and removes references to self form
  /// parent.
  virtual ~Model();

  /// Return a newly created/allocated prototype that is an exact copy of this.
  /// All initialization and state cloning operations should be done in the
  /// model's InitFrom method. The new model instance should generally NOT be
  /// created using a default copy-constructor.
  /// 
  /// Example:
  ///
  /// @code
  /// class MyModelClass : virtual public Model {
  ///   ...
  ///
  ///   virtual Model* Clone() {
  ///     MyModelClass* m = new MyModelClass(context());
  ///     m->InitFrom(this);
  ///     return m;
  ///   };
  ///
  ///   ...
  /// };
  /// @endcode
  virtual Model* Clone() = 0;

  /// Translates info for a model from an input file to the database by reading
  /// parameters from the passed QueryEngine and recording data via the DbInit
  /// variable.  The simulation and agent id's are automatically injected in all
  /// data transfered through di.  This method must be implemented by all
  /// agents.  This method must call the superclass' InfileToDb method before
  /// doing any other work.
  ///
  /// Model parameters in the QueryEngine are scoped in the
  /// "model/[model-class-name]" path. The model's class-name can be retrieved
  /// from the model_impl method. The superclass InitFrom expects the QueryEngine
  /// passed to it to be scoped identically - do NOT pass a changed-scope
  /// QueryEngine to the superclass.
  ///
  /// Example:
  ///
  /// @code
  /// class MyModelClass : virtual public cyclus::FacilityModel {
  ///   // ...
  ///
  ///   void InfileToDb(QueryEngine* qe, DbInit di) {
  ///     cyclus::FacilityModel::InitFrom(qe); // 
  ///     // now do MyModelClass' initialitions, e.g.:
  ///     qe = qe->QueryElement("model/" + model_impl()); // rescope the QueryEngine
  ///
  ///     // retrieve all model params
  ///     std::string recipe = qe->GetElementContent("recipe");
  ///     std::string in_commod = qe->GetElementContent("in_commod");
  ///     std::string out_commod = qe->GetElementContent("out_commod");
  ///     di.NewDatum("MyModelTable1")
  ///       ->AddVal("recipe", recipe)
  ///       ->AddVal("in_commod", in_commod)
  ///       ->AddVal("out_commod", out_commod)
  ///       ->Record();
  ///     // ...
  ///   };
  ///
  ///   // ...
  /// };
  /// @endcode
  ///
  /// @warning this method MUST NOT use or modify any instance state.
  virtual void InfileToDb(QueryEngine* qe, DbInit di);

  /// Intializes an agent's internal state from an output database. Appropriate
  /// simulation id, agent id, and time filters are automatically included in
  /// all queries.
  ///
  /// @warning Agents must NOT create any resource objects in this method.
  virtual void InitFrom(QueryBackend* b);

  /// Snapshots agent-internal state to the output db via di. This method MUST
  /// call the superclass' Snapshot method before doing any work. The simulation
  /// and agent id's in addition to the snapshot time are automatically included
  /// in all information transfered through di.
  ///
  /// @warning because a 'Time' field is automatically injected, that label
  /// cannot be used for any other fields.
  ///
  /// @warning This method must NOT modify the agent's state.
  virtual void Snapshot(DbInit di) = 0;

  /// Provides an agent's initial inventory of resources before a simulation
  /// begins. The resources are keyed in the same way the InitInv method
  /// returned. Agents should iterate through each named inventory provided and
  /// populate the corresponding resource containers.
  ///
  /// @warning agents should not modify any state outside the container filling
  virtual void InitInv(const Inventories& inv) = 0;

  /// Snapshots and agent's resource inventories to the output db. The set of
  /// resources in each container should be stored under a string key specific
  /// to that container that will be used when re-initializing inventories for
  /// restarted simulations.
  ///
  /// @warning This method should NOT modify the agent's state.
  virtual Inventories SnapshotInv() = 0;

  /// recursively prints the parent-child tree
  std::string PrintChildren();

  /// returns a vector of strings representing the parent-child tree
  /// at the node for Model m
  /// @param m the model node to base as the root of this print tree
  std::vector<std::string> GetTreePrintOuts(Model* m);

  /// Called when the agent enters the smiulation as an active participant and
  /// is only ever called once.  Agents should NOT register for services (such
  /// as ticks/tocks and resource exchange) in this method. If agents implement
  /// this method, they must call their superclass's Build method at the
  /// BEGINING of their Build method.
  ///
  /// @param parent this agent's parent
  virtual void Build(Model* parent = NULL);

  /// Called to give the agent an opportunity to register for services (e.g.
  /// ticks/tocks and resource exchange).  Note that this may be called more
  /// than once, and so agents should track their registrations carefully.
  virtual void DoRegistration() {};

  /// Called when a new child of this agent has just been built. It is possible
  /// for this method to be called before the simulation has started when
  /// initially existing agents are being setup.
  virtual void BuildNotify(Model* m) {};

  /// Called when a new child of this agent is about to be decommissioned.
  virtual void DecomNotify(Model* m) {};

  /// Decommissions the model, removing it from the simulation. Results in
  /// destruction of the model object. If agents write their own Decommission
  /// method, they must call their superclass' Decommission method at the END of
  /// their Decommission method.
  virtual void Decommission();

  /// returns the ith child
  Model* children(int i) {
    return children_[i];
  }

  /// default implementation for material preferences.
  virtual void AdjustMatlPrefs(PrefMap<Material>::type& prefs) {};
  
  /// default implementation for material preferences.
  virtual void AdjustGenRsrcPrefs(PrefMap<GenericResource>::type& prefs) {};

  /// Returns a module's xml rng schema for initializing from input files. All
  /// concrete models should override this method.
  virtual std::string schema() {
    return "<text />\n";
  };

  /// get model instance name
  inline const std::string prototype() const { return prototype_; }

  /// get model instance ID
  inline const int id() const { return id_; }

  /// get model implementation
  inline std::string model_impl() {return model_impl_;}

  /// set model implementation
  inline void set_model_impl(std::string new_impl) {model_impl_ = new_impl;}

  /// returns a string that describes the model subclass (e.g. Region, etc.)
  inline const std::string kind() const {return kind_;};

  /// Returns this model's current simulation context.
  inline Context* context() const { return ctx_; }

  /// every model should be able to print a verbose description
  virtual std::string str();

  /// returns parent of this model
  inline Model* parent() const { return parent_; }

  /// returns the parent's id
  inline const int parent_id() const { return parent_id_; }

  /// returns the time this model began operation (-1 if the model has never
  /// been built).
  inline const int birthtime() const { return birthtime_; }

  /// returns the time this model will cease operation (-1 if the model is still
  /// operating). Returns -1 for indefinite lifetime.
  inline const int lifetime() const { return lifetime_; }

  /// returns a list of children this model has
  inline const std::vector<Model*>& children() const { return children_; }
  
 protected:
  /// Initializes a model by copying parameters from the passed model m. This
  /// method must be implemented by all models.  This method must call the
  /// superclass' InitFrom method. The InitFrom method should only initialize
  /// this class' members - not inherited state. The superclass InitFrom should
  /// generally be called before any other work is done.
  ///
  /// @param m the model containing state that should be used to initialize this
  /// model.
  /// 
  /// Example:
  ///
  /// @code
  /// class MyModelClass : virtual public cyclus::FacilityModel {
  ///   // ...
  ///
  ///   void InitFrom(MyModelClass* m) {
  ///     cyclus::FacilityModel::InitFrom(m); // call superclass' InitFrom
  ///     // now do MyModelClass' initialitions, e.g.:
  ///     my_var_ = m->my_var_;
  ///     // ...
  ///   };
  ///
  ///   // ...
  /// };
  /// @endcode
  void InitFrom(Model* m);

  /// adds model-specific information prefix to an error message
  virtual std::string InformErrorMsg(std::string msg);
  
  /// describes the model subclass (e.g. Region, Inst, etc.). The in-core
  /// subclasses must set this variable in their constructor(s).
  std::string kind_;

 private:
  /// Prevents creation/use of copy constructors (including in subclasses).
  /// Cloning and InitFrom should be used instead.
  Model(const Model& m) {};

  /// adds an agent to the transactiont table
  void AddToTable();

  /// Builds an agent - without recording any output data
  void BuildInner(Model* parent);

  /// Stores the next available facility ID
  static int next_id_;

  /// children of this model
  std::vector<Model*> children_;

  /// parent of this model
  Model* parent_;

  /// parent's ID of this model
  /// Note: we keep the parent id in the model so we can reference it
  /// even if the parent is deallocated.
  int parent_id_;

  /// born on date of this model
  int birthtime_;

  /// length of time this model is intended to operate
  int lifetime_;

  std::string prototype_;

  /// concrete type of a model (e.g. "MyReactorModel")
  std::string model_impl_;

  /// an instance-unique ID for the model
  int id_;

  Context* ctx_;
};

} // namespace cyclus

#endif

