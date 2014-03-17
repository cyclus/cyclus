// agent.h
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
#include "state_wrangler.h"

#define SHOW(X) \
  std::cout << __FILE__ << ":" << __LINE__ << ": "#X" = " << X << "\n"

namespace cyclus {

class Material;
class Product;
  
/// map<internal-inventory-name, vector<resources-inside-inventory> >
typedef std::map<std::string, std::vector<Resource::Ptr> > Inventories;

/// The abstract base class used by all types of agents
/// that live and interact in a simulation.
///
/// There are several methods that must be implemented in support of simulation
/// initialization, snapshotting and restart: InfileToDb,
/// InitFrom(QueryableBackend*), Snapshot, SnapshotInv, and InitInv.  These methods
/// all do inter-related things.  Notably, the InfileToDb, InitFrom, and
/// Snapshot methods must all write/read to/from the same database tables (and
/// table schemas).
class Agent : public StateWrangler {
  friend class SimInit;

 public:
  /// Constructor for the Agent Class
  ///
  /// @warning all constructors must set id_ and increment next_id_
  Agent(Context* ctx);

  /// Recursively destructs all children and removes references to self form
  /// parent.
  virtual ~Agent();

  /// Return a newly created/allocated prototype that is an exact copy of this.
  /// All initialization and state cloning operations should be done in the
  /// agent's InitFrom method. The new agent instance should generally NOT be
  /// created using a default copy-constructor.
  /// 
  /// Example:
  ///
  /// @code
  /// class MyAgentClass : virtual public Agent {
  ///   ...
  ///
  ///   virtual Agent* Clone() {
  ///     MyAgentClass* m = new MyAgentClass(context());
  ///     m->InitFrom(this);
  ///     return m;
  ///   };
  ///
  ///   ...
  /// };
  /// @endcode
  virtual Agent* Clone() = 0;

  /// Translates info for a agent from an input file to the database by reading
  /// parameters from the passed QueryEngine and recording data via the DbInit
  /// variable.  The simulation and agent id's are automatically injected in all
  /// data transfered through DbInit.  This method must be implemented by all
  /// agents.  This method must call the superclass' InfileToDb method before
  /// doing any other work.
  ///
  /// Agent parameters in the QueryEngine are scoped in the
  /// "agent/[agent-class-name]" path. The agent's class-name can be retrieved
  /// from the agent_impl method. The superclass InitFrom expects the QueryEngine
  /// passed to it to be scoped identically - do NOT pass a changed-scope
  /// QueryEngine to the superclass.
  ///
  /// Example:
  ///
  /// @code
  /// class MyAgentClass : virtual public cyclus::Facility {
  ///   // ...
  ///
  ///   void InfileToDb(QueryEngine* qe, DbInit di) {
  ///     cyclus::Facility::InitFrom(qe); // 
  ///     // now do MyAgentClass' initialitions, e.g.:
  ///     qe = qe->QueryElement("agent/" + agent_impl()); // rescope the QueryEngine
  ///
  ///     // retrieve all agent params
  ///     std::string recipe = qe->GetString("recipe");
  ///     std::string in_commod = qe->GetString("in_commod");
  ///     std::string out_commod = qe->GetString("out_commod");
  ///     di.NewDatum("MyAgentTable1")
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
  /// @warning this method MUST NOT modify the agent's state.
  virtual void InfileToDb(QueryEngine* qe, DbInit di);

  /// Intializes an agent's internal state from an output database. Appropriate
  /// simulation id, agent id, and time filters are automatically included in
  /// all queries.
  ///
  /// @warning Agents must NOT create any resource objects in this method.
  virtual void InitFrom(QueryableBackend* b);

  /// Snapshots agent-internal state to the output db via DbInit. This method MUST
  /// call the superclass' Snapshot method before doing any work. The simulation
  /// and agent id's in addition to the snapshot time are automatically included
  /// in all information transfered through DbInit.
  ///
  /// @warning because 'SimId' 'SimTime', and 'AgentId' fields are automatically
  /// injected, those labels cannot be used for any other fields.
  ///
  /// @warning This method MUST NOT modify the agent's state.
  virtual void Snapshot(DbInit di) = 0;

  /// Provides an agent's initial inventory of resources before a simulation
  /// begins. The resources are keyed in the same way the InitInv method
  /// returned. Agents should iterate through each named inventory provided and
  /// populate the corresponding resource containers.
  ///
  /// @warning agents should not modify any state outside the container filling
  virtual void InitInv(Inventories& inv) = 0;

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
  /// at the node for Agent m
  /// @param m the agent node to base as the root of this print tree
  std::vector<std::string> GetTreePrintOuts(Agent* m);

  /// Called when the agent enters the smiulation as an active participant and
  /// is only ever called once.  Agents should NOT register for services (such
  /// as ticks/tocks and resource exchange) in this method. If agents implement
  /// this method, they must call their superclass's Build method at the
  /// BEGINING of their Build method.
  ///
  /// @param parent this agent's parent. NULL if this agent has no parent.
  virtual void Build(Agent* parent = NULL);

  /// Called to give the agent an opportunity to register for services (e.g.
  /// ticks/tocks and resource exchange).  Note that this may be called more
  /// than once, and so agents should track their registrations carefully.
  virtual void DoRegistration() {};

  /// Called when a new child of this agent has just been built. It is possible
  /// for this method to be called before the simulation has started when
  /// initially existing agents are being setup.
  virtual void BuildNotify(Agent* m) {};

  /// Called when a new child of this agent is about to be decommissioned.
  virtual void DecomNotify(Agent* m) {};

  /// Decommissions the agent, removing it from the simulation. Results in
  /// destruction of the agent object. If agents write their own Decommission
  /// method, they must call their superclass' Decommission method at the END of
  /// their Decommission method.
  virtual void Decommission();

  /// returns the ith child
  Agent* children(int i) {
    return children_[i];
  }

  /// default implementation for material preferences.
  virtual void AdjustMatlPrefs(PrefMap<Material>::type& prefs) {};
  
  /// default implementation for material preferences.
  virtual void AdjustProductPrefs(PrefMap<Product>::type& prefs) {};

  /// Returns a module's xml rng schema for initializing from input files. All
  /// concrete agents should override this method.
  virtual std::string schema() {
    return "<text />\n";
  };

  /// get agent instance name
  inline const std::string prototype() const { return prototype_; }

  /// get agent instance ID
  inline const int id() const { return id_; }

  /// get agent implementation
  inline std::string agent_impl() {return agent_impl_;}

  /// set agent implementation
  inline void set_agent_impl(std::string new_impl) {agent_impl_ = new_impl;}

  /// returns a string that describes the agent subclass (e.g. Region, etc.)
  inline const std::string kind() const {return kind_;};

  /// Returns this agent's current simulation context.
  inline Context* context() const { return ctx_; }

  /// every agent should be able to print a verbose description
  virtual std::string str();

  /// returns parent of this agent
  inline Agent* parent() const { return parent_; }

  /// returns the parent's id
  inline const int parent_id() const { return parent_id_; }

  /// returns the time this agent began operation (-1 if the agent has never
  /// been built).
  inline const int enter_time() const { return enter_time_; }

  /// returns the time this agent will cease operation (-1 if the agent is still
  /// operating). Returns -1 for indefinite lifetime.
  inline const int lifetime() const { return lifetime_; }

  /// returns a list of children this agent has
  inline const std::vector<Agent*>& children() const { return children_; }
  
 protected:
  /// Initializes a agent by copying parameters from the passed agent m. This
  /// method must be implemented by all agents.  This method must call the
  /// superclass' InitFrom method. The InitFrom method should only initialize
  /// this class' members - not inherited state. The superclass InitFrom should
  /// generally be called before any other work is done.
  ///
  /// @param m the agent containing state that should be used to initialize this
  /// agent.
  /// 
  /// Example:
  ///
  /// @code
  /// class MyAgentClass : virtual public cyclus::Facility {
  ///   // ...
  ///
  ///   void InitFrom(MyAgentClass* m) {
  ///     cyclus::Facility::InitFrom(m); // call superclass' InitFrom
  ///     // now do MyAgentClass' initialitions, e.g.:
  ///     my_var_ = m->my_var_;
  ///     // ...
  ///   };
  ///
  ///   // ...
  /// };
  /// @endcode
  void InitFrom(Agent* m);

  /// adds agent-specific information prefix to an error message
  virtual std::string InformErrorMsg(std::string msg);
  
  /// describes the agent subclass (e.g. Region, Inst, etc.). The in-core
  /// subclasses must set this variable in their constructor(s).
  std::string kind_;

 private:
  /// Prevents creation/use of copy constructors (including in subclasses).
  /// Cloning and InitFrom should be used instead.
  Agent(const Agent& m) {};

  /// adds an agent to the transactiont table
  void AddToTable();

  /// connects an agent to its parent.
  void Connect(Agent* parent);

  /// Stores the next available facility ID
  static int next_id_;

  /// children of this agent
  std::vector<Agent*> children_;

  /// parent of this agent
  Agent* parent_;

  /// parent's ID of this agent
  /// Note: we keep the parent id in the agent so we can reference it
  /// even if the parent is deallocated.
  int parent_id_;

  /// born on date of this agent
  int enter_time_;

  /// length of time this agent is intended to operate
  int lifetime_;

  std::string prototype_;

  /// concrete type of a agent (e.g. "MyReactorAgent")
  std::string agent_impl_;

  /// an instance-unique ID for the agent
  int id_;

  Context* ctx_;
};

} // namespace cyclus

#endif

