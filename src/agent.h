#ifndef CYCLUS_SRC_AGENT_H_
#define CYCLUS_SRC_AGENT_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "db_init.h"
#include "dynamic_module.h"
#include "infile_tree.h"
#include "exchange_context.h"
#include "pyne.h"
#include "query_backend.h"
#include "resource.h"
#include "state_wrangler.h"

class SimInitTest;

namespace cyclus {

class Ider {
 public:
  virtual const int id() const = 0;
};

class Material;
class Product;

/// map<inventory_name, vector<resources_in_inventory> >.  Used by agents in
/// their #SnapshotInv and #InitInv functions for saving+loading their internal
/// resource inventories.
typedef std::map<std::string, std::vector<Resource::Ptr> > Inventories;

/// The abstract base class used by all types of agents
/// that live and interact in a simulation.
///
/// There are several functions that must be implemented in support of simulation
/// initialization, snapshotting and restart: #InfileToDb,
/// InitFrom(QueryableBackend*), #Snapshot, #SnapshotInv, and #InitInv.  These
/// functions all do inter-related things.  Notably, the #InfileToDb, #InitFrom,
/// and #Snapshot functions must all write/read to/from the same database tables
/// (and table schemas).
class Agent : public StateWrangler, virtual public Ider {
  friend class SimInit;
  friend class ::SimInitTest;

 public:
  /// Creates a new agent that is managed by the given context. Note that the
  /// context takes ownership of its agents' memory and so agents should
  /// generally never be created on the stack.
  Agent(Context* ctx);

  /// Removes references to self from simulation context, parent, children,
  /// etc. All subclass destructors should also be virtual.
  virtual ~Agent();

  /// Returns a newly created/allocated prototype that is an exact copy of this.
  /// All initialization and state cloning operations should be done in the
  /// agent's InitFrom(Agent*) function. The new agent instance should NOT be
  /// created using a default copy-constructor. New agent instances should
  /// generally be created using a constructor that takes a single Context
  /// argument (the same context of the agent being cloned).  Example:
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

  /// Translates info for the agent from an input file to the database by
  /// reading parameters from the passed InfileTree (parsed from xml) and
  /// recording data via the DbInit variable.  The simulation id and agent id
  /// are automatically injected in all data transfered to the database through
  /// DbInit.  This function must be implemented by all agents.  This function must
  /// call the superclass' InfileToDb function before doing any other work.
  ///
  /// Agent parameters in the InfileTree are scoped in the "agent/*/" path.
  /// The superclass InitFrom expects the scope InfileTree passed to it to be
  /// unchanged from the agent's InfileTree arg.
  ///
  ///
  /// Example:
  ///
  /// @code
  /// class MyAgentClass : virtual public cyclus::Facility {
  ///   // ...
  ///
  ///   void InfileToDb(cyclus::InfileTree* tree, cyclus::DbInit di) {
  ///     cyclus::Facility::InitFrom(tree);
  ///
  ///     // now do MyAgentClass' initialitions, e.g.:
  ///     tree = tree->Query("agent/*"); // rescope the InfileTree
  ///
  ///     // retrieve all agent params
  ///     std::string recipe = tree->GetString("recipe");
  ///     std::string in_commod = tree->GetString("in_commod");
  ///     double cap = cyclus::Query<double>(tree, "cap");
  ///     di.NewDatum("MyAgentTable1")
  ///       ->AddVal("recipe", recipe)
  ///       ->AddVal("in_commod", in_commod)
  ///       ->AddVal("cap", cap)
  ///       ->Record();
  ///     // ...
  ///   };
  ///
  ///   // ...
  /// };
  /// @endcode
  ///
  /// @warning because 'SimId' 'SimTime', and 'AgentId' fields are automatically
  /// injected, those labels cannot be used for any other fields.
  ///
  /// @warning this function MUST NOT modify the agent's state.
  virtual void InfileToDb(InfileTree* qe, DbInit di);

  /// Intializes an agent's internal state from the database. Appropriate
  /// simulation id, agent id, and time filters are automatically included in
  /// all queries. If the agent is a direct subclass of the Agent class, than
  /// it should NOT call its superclass' InitFrom(QueryableBackend*) function.
  /// If, however, it is a subclasses other Agent subclasses (e.g. subclass of
  /// Facility, Region, etc.), then it MUST call its superclass'
  /// InitFrom(QueryableBackend*) function. Example:
  ///
  /// @code
  /// class MyAgentClass : virtual public cyclus::Facility {
  ///   // ...
  ///
  ///   void InitFrom(cyclus::QueryableBackend* b) {
  ///     cyclus::Facility::InitFrom(b);
  ///
  ///     cyclus::QueryResult qr = b->Query("MyAgentTable1", NULL);
  ///     recipe = qr.GetVal<std::string>("recipe");
  ///     in_commod = qr.GetVal<std::string>("in_commod");
  ///     cap = qr.GetVal<double>("cap");
  ///
  ///     // ...
  ///   };
  ///
  ///   std::string recipe;
  ///   std::string in_commod;
  ///   double cap;
  ///
  ///   // ...
  /// };
  ///
  /// @endcode
  ///
  /// @warning Agents should NOT create any resource objects in this function.
  virtual void InitFrom(QueryableBackend* b);

  /// Snapshots agent-internal state to the database via the DbInit var di.  The
  /// simulation id , agent id, and the simulation time are automatically
  /// injected in all information transfered to the database through DbInit.
  /// If the agent is a direct subclass of the Agent class, than it should NOT
  /// call its superclass' Snapshot function.  If, however, it subclasses
  /// other Agent subclasses (e.g. subclass of Facility, Region, etc.), then it
  /// MUST call its superclass' Snapshot function. Example:
  ///
  /// @code
  /// class MyAgentClass : virtual public cyclus::Facility {
  ///   // ...
  ///
  ///   void Snapshot(cyclus::DbInit di) {
  ///     cyclus::Facility::Snapshot(di);
  ///
  ///     di.NewDatum("MyAgentTable1")
  ///       ->AddVal("recipe", recipe)
  ///       ->AddVal("in_commod", in_commod)
  ///       ->AddVal("cap", cap)
  ///       ->Record();
  ///
  ///     // ...
  ///   };
  ///
  ///   std::string recipe;
  ///   std::string in_commod;
  ///   double cap;
  ///
  ///   // ...
  /// };
  ///
  /// @endcode
  ///
  /// @warning because 'SimId' 'SimTime', and 'AgentId' fields are automatically
  /// injected, those labels cannot be used for any other fields.
  ///
  /// @warning This function MUST NOT modify the agent's internal state.
  /// @warning Do not record any information via the context's NewDatum
  /// function.
  virtual void Snapshot(DbInit di) = 0;

  /// Provides an agent's initial inventory of resources before a simulation
  /// begins. The resources are keyed in the same way as given in the agent's
  /// SnapshotInv function.  Agents should iterate through each named inventory
  /// provided and populate their corresponding resource containers with the
  /// given resources.
  ///
  /// @code
  /// class MyAgentClass : virtual public cyclus::Facility {
  ///   // ...
  ///
  ///   void InitInv(cyclus::Inventories& inv) {
  ///     buf1.PushAll(inv["buf1"]);
  ///     buf2.PushAll(inv["buf2"]);
  ///
  ///     // ...
  ///   };
  ///
  ///   cyclus::toolkit::ResourceBuff buf1;
  ///   cyclus::toolkit::ResourceBuff buf2;
  ///
  ///   // ...
  /// };
  ///
  /// @endcode
  ///
  /// @warning agents should not modify any state outside the container filling
  virtual void InitInv(Inventories& inv) = 0;

  /// Snapshots an agent's resource inventories to the database. The set of
  /// resources in each container should be stored under a string key unique
  /// for that container that will be used when re-initializing inventories for
  /// restarted simulations in the InitInv function.
  ///
  /// @code
  /// class MyAgentClass : virtual public cyclus::Facility {
  ///   // ...
  ///
  ///   cyclus::Inventories SnapshotInv() {
  ///     cyclus::Inventories invs;
  ///     std::vector<cyclus::Resource::Ptr> rs = buf1.PopN(buf1.count());
  ///     buf1.PushAll(rs); // Snapshot must not change agent's state
  ///     invs["buf1"] = rs;
  ///     rs = buf1.PopN(buf2.count());
  ///     buf1.PushAll(rs); // Snapshot must not change agent's state
  ///     invs["buf2"] = rs;
  ///
  ///     // ...
  ///
  ///     return invs;
  ///   };
  ///
  ///   cyclus::toolkit::ResourceBuff buf1;
  ///   cyclus::toolkit::ResourceBuff buf2;
  ///
  ///   // ...
  /// };
  ///
  /// @endcode
  ///
  /// @warning This function MUST NOT modify the agent's internal state.
  virtual Inventories SnapshotInv() = 0;

  /// recursively prints the parent-child tree
  std::string PrintChildren();

  /// returns a vector of strings representing the parent-child tree
  /// at the node for Agent m
  /// @param m the agent node to base as the root of this print tree
  std::vector<std::string> GetTreePrintOuts(Agent* m);

  /// returns true if this agent is in the parent-child family tree of an other
  /// agent
  /// @param other the other agent
  bool InFamilyTree(Agent* other);

  /// returns true if this agent is an ancestor of an other agent (i.e., resides
  /// above an other agent in the family tree)
  /// @param other the other agent
  bool AncestorOf(Agent* other);
  
  /// returns true if this agent is an decendent of an other agent (i.e., resides
  /// below an other agent in the family tree)
  /// @param other the other agent
  bool DecendentOf(Agent* other);
  
  /// Called when the agent enters the smiulation as an active participant and
  /// is only ever called once.  Agents should NOT register for services (such
  /// as ticks/tocks and resource exchange) in this function. If agents implement
  /// this function, they must call their superclass' Build function at the
  /// BEGINING of their Build function.
  ///
  /// @param parent this agent's parent. NULL if this agent has no parent.
  virtual void Build(Agent* parent);

  /// Called to give the agent an opportunity to register for services (e.g.
  /// ticks/tocks and resource exchange).  Note that this may be called more
  /// than once, and so agents should track their registrations carefully. If
  /// agents implement this function, they must call their superclass's
  /// EnterNotify function at the BEGINNING of their EnterNotify function.
  virtual void EnterNotify();

  /// Called when a new child of this agent has just been built. It is possible
  /// for this function to be called before the simulation has started when
  /// initially existing agents are being setup.
  virtual void BuildNotify(Agent* m) {}

  /// Called when a new child of this agent is about to be decommissioned.
  virtual void DecomNotify(Agent* m) {}

  /// Decommissions the agent, removing it from the simulation. Results in
  /// destruction of the agent object. If agents write their own Decommission
  /// function, they must call their superclass' Decommission function at the END of
  /// their Decommission function.
  virtual void Decommission();

  /// default implementation for material preferences.
  virtual void AdjustMatlPrefs(PrefMap<Material>::type& prefs) {}

  /// default implementation for material preferences.
  virtual void AdjustProductPrefs(PrefMap<Product>::type& prefs) {}

  /// Returns an agent's xml rng schema for initializing from input files. All
  /// concrete agents should override this function. This must validate the same
  /// xml input that the InfileToDb function receives.
  virtual std::string schema() {
    return "<text />\n";
  }

  /// Returns an agent's json annotations for all state variables and any other
  /// information the developer wishes to provide. All concrete agents should
  /// override this function.
  virtual Json::Value annotations() {
    return Json::Value(Json::objectValue);
  }

  /// Returns the agent's prototype.
  inline const std::string prototype() const { return prototype_; }

  /// Sets the agent's prototype.  This should generally NEVER be called
  /// explicitly by code outside the cyclus kernel.
  inline void prototype(std::string p) { prototype_ = p; }

  /// The agent instance's unique ID within a simulation.
  virtual const int id() const { return id_; }

  /// The string representation of the agent spec that uniquely identifies the
  /// concrete agent class/module.  See CEP21 for details..
  inline std::string spec() { return spec_; }

  /// Sets this agent's agent spec.  This should generally NEVER be called
  /// explicitly by code outside the cyclus kernel.
  inline void spec(std::string new_impl) { spec_ = new_impl; }

  /// Returns a string that describes the agent subclass (e.g. Region,
  /// Facility, etc.)
  inline const std::string kind() const { return kind_; }

  /// Returns this agent's simulation context.
  inline Context* context() const { return ctx_; }

  /// Description of this agent.
  virtual std::string str();

  /// Returns parent of this agent.  Returns NULL if the agent has no parent.
  inline Agent* parent() const { return parent_; }

  /// Returns the id for this agent's parent. Returns -1 if this agent has no
  /// parent.
  inline const int parent_id() const { return parent_id_; }

  /// Returns the time step at which this agent's Build function was called (-1 if
  /// the agent has never been built).
  inline const int enter_time() const { return enter_time_; }

  /// Returns the number of time steps this agent operates between building and
  /// decommissioning (-1 if the agent has an infinite lifetime).
  inline const int lifetime() const { return lifetime_; }

  /// Returns a list of children this agent has
  inline const std::set<Agent*>& children() const { return children_; }

 protected:
  /// Initializes a agent by copying parameters from the passed agent m. This
  /// function must be implemented by all agents.  This function must call the
  /// superclass' InitFrom function. The InitFrom function should only initialize
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

  /// describes the agent subclass (e.g. Region, Inst, etc.). The in-kernel
  /// subclasses must set this variable in their constructor(s).
  std::string kind_;

 private:
  /// Prevents creation/use of copy constructors (including in subclasses).
  /// Cloning and InitFrom should be used instead.
  Agent(const Agent& m) {}

  /// adds an agent to the transactiont table
  void AddToTable();

  /// connects an agent to its parent.
  void Connect(Agent* parent);

  /// Stores the next available facility ID
  static int next_id_;

  /// children of this agent
  std::set<Agent*> children_;

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
  std::string spec_;

  /// an instance-unique ID for the agent
  int id_;

  Context* ctx_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_AGENT_H_
