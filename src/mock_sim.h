#ifndef CYCLUS_SRC_MOCK_SIM_H_
#define CYCLUS_SRC_MOCK_SIM_H_

#include "cyclus.h"
#include "sqlite_back.h"
#include "timer.h"

namespace cyclus {

class Source;
class Sink;

/// MockAgent is a template for accumulating configuration information used to
/// generate a source or sink facility in a MockSimulation.  All parameters
/// other than commod have defaults. After all desired configuration is
/// completed, the Finalize function MUST be called.  All configure functions
/// return the MockAgent itself to enable chaining.  Default configuration is:
///
/// * no recipe - sources provide requested material, sinks take anything
/// * infinite per time step capacity
/// * start/deploy on time step zero
/// * infinite lifetime.
///
/// For examples on how to use MockAgent, see the MockSim API documentation.
class MockAgent {
 public:
  
  /// Initializes a MockAgent to create a source (is_source == true) or a
  /// sink (is_source == false) in the provided simulation context.  rec must be
  /// the recorder used to initialize ctx and b must be a backend registered
  /// with rec.
  MockAgent(Context* ctx, Recorder* rec, SqliteBack* b, bool is_source);

  /// Sets the commodity to be offered/requested by the source/sink.
  MockAgent commod(std::string commod);

  /// Sets the recipe to be offered/requested by the source/sink.
  MockAgent recipe(std::string recipe);

  /// Sets the per time step capacity/throughput limit for provided/received
  /// material in kg for the source/sink.
  MockAgent capacity(double cap);

  /// Sets the time step in the simulation that the source/sink should be
  /// deployed.
  MockAgent start(int timestep);

  /// Sets the lifetime in time steps of the source/sink before it is
  /// decommissioned.
  MockAgent lifetime(int duration);

  /// Finalize MUST be called after configuration is complete to actually
  /// create the source/sink agent (or schedule it to be built). The
  /// auto-generated prototype name of the created prototype is returned to
  /// support querying based on specific sources/sinks.
  std::string Finalize();

 private:
  static int nextid_;
  bool source_;
  std::string commod_;
  std::string recipe_;
  double cap_;
  int start_;
  int lifetime_;
  std::string proto_;
  Context* ctx_;
  Recorder* rec_;
  SqliteBack* back_;
};

/// MockSim is a helper for running full simulations entirely in-code to test
/// archetypes/agents without having to deal with input files, output database
/// files, and other pieces of the full Cyclus stack.  This is especially
/// convenient for writing unit-like tests (e.g. using gtest) for your
/// archetype's in-simulation behavior.   Initialize the MockSim indicating the
/// archetype you want to test and the simulation duration.  Then add any
/// number of sources and/or sinks to transact with your agent.  They can have
/// specific recipes (or not) and their deployment and lifetime (before
/// decommissioning) can be specified too.  Here is an example using the
/// agents:Source archetype in Cyclus as the tested agent:
///
/// @code
///
/// cyclus::CompMap m;
/// m[922350000] = .05;
/// m[922380000] = .95;
/// cyclus::Composition::Ptr fresh = cyclus::Composition::CreateFromMass(m);
///
/// std::string config =
///     "<commod>enriched_u</commod>"
///     "<recipe_name>fresh_fuel</recipe_name>"
///     "<capacity>10</capacity>";
///
/// int dur = 10;
/// cyclus::MockSim sim(cyclus::AgentSpec(":agents:Source"), config, dur);
/// sim.AddSink("enriched_u").Finalize();
/// sim.AddRecipe("fresh_fuel", fresh);
/// int src_id = sim.Run(); // capture the agent ID of the facility being tested
///
/// @endcode
///
/// Querying the results can be accomplished by getting a reference to the
/// in-memory database generated.  Not all data that is present in normal
/// full-stack simulations is available.  However, most of the key core tables
/// are fully available.  Namely, the Transactions, Composition, Resources,
/// ResCreators, AgentEntry, and AgentExit tables are available.  Any
/// custom-tables created by the tested archetype will also be available.  Here
/// is a sample query and test you might write if using the googletest
/// framework:
///
/// @code
///
/// // return all transactions where our source facility is the sender
/// std::vector<cyclus::Cond> conds;
/// conds.push_back("SenderId", "==", src_id);
/// cyclus::QueryResult qr = sim.db().Query("Transactions", &conds);
/// int n_trans = qr.rows.size();
/// EXPECT_EQ(10, n_trans) << "expected 10 transactions, got " << n_trans;
///
/// // reconstruct the material object for the first transaction
/// int res_id = qr.GetVal<int>("ResourceId", 0);
/// cyclus::Material::Ptr m = sim.GetMaterial(res_id);
/// EXPECT_DOUBLE_EQ(10, m->quantity());
///
/// // confirm composition is as expected
/// cyclus::toolkit::MatQuery mq(m);
/// EXPECT_DOUBLE_EQ(0.5, mq.mass(922350000));
/// EXPECT_DOUBLE_EQ(9.5, mq.mass(922380000));
///
/// @endcode
class MockSim {
 public:
  /// Creates and initializes a new empty mock simulation environment where
  /// duration is the length of the simulation in time steps. An agent must be
  /// specified before running an empty-constructed MockSim (i.e. manually
  /// construct an agent using MockSim::context() and setting MockSim::agent.
  MockSim(int duration);

  /// Creates and initializes a new mock simulation environment to test the
  /// archetype identified by spec.  config should contain the
  /// archetype-specific xml snippet excluding the wrapping "<config>" and
  /// "<[AgentName]>" tags.  duration is the length of the simulation in time
  /// steps.
  MockSim(AgentSpec spec, std::string config, int duration);

  /// Creates and initializes a new mock simulation environment to test the
  /// archetype identified by spec.  config should contain the
  /// archetype-specific xml snippet excluding the wrapping "<config>" and
  /// "<[AgentName]>" tags.  duration is the length of the simulation in time
  /// steps.  'lifetime' is the lifetime of the agent being tested.
  MockSim(AgentSpec spec, std::string config, int duration, int lifetime);

  ~MockSim();

  /// AddSource adds a source facility that can offer/provide material to the
  /// archetype being tested.  commod specifies the commodity the source will
  /// offer on. AddSource can be called multiple times to generate many sources
  /// for the simulation.  The returned MockAgent object has several functions
  /// that can be called to configure the source's behavior further.  Don't
  /// forget to call the MockAgent object's "Finalize" function when you are
  /// done configuring it.  MockAgent's functions support chaining:
  ///
  /// @code
  ///
  /// cyclus::MockSim sim(...);
  /// sim.AddSource("fresh_mox")
  ///    .start(7).lifetime(3).recipe("mox_fuel")
  ///    .Finalize();
  ///
  /// @endcode
  MockAgent AddSource(std::string commod);

  /// AddSink adds a sink facility that can request+receive material from the
  /// archetype being tested.  commod specifies the commodity the sink will
  /// request on. AddSink can be called multiple times to generate many sinks
  /// for the simulation.  The returned MockAgent object has several functions
  /// that can be called to configure the sink's behavior further.  Don't
  /// forget to call the MockAgent object's "Finalize" function when you are
  /// done configuring it.  MockAgent's functions support chaining:
  ///
  /// @code
  ///
  /// cyclus::MockSim sim(...);
  /// sim.AddSink("spent_mox")
  ///    .start(7).lifetime(3).recipe("spent_fresh_mox")
  ///    .Finalize();
  ///
  /// @endcode
  MockAgent AddSink(std::string commod);

  /// AddRecipe adds a recipe to the mock simulation environment (i.e. to the
  /// simulation context).  Any recipes that your archetype expects to find in
  /// the simulation context must be added this way; if the xml configuration
  /// snippet for the archetype being tested contains a recipe name, add it
  /// with this function.
  void AddRecipe(std::string name, Composition::Ptr c);

  /// Adds a dummy prototype to the simulation that can be used by
  /// institutions and other agents for deployment/decommission testing.
  void DummyProto(std::string name);

  /// Adds a dummy prototype to the simulation that can be used by
  /// institutions and other agents for demand-driven deployment testing.
  void DummyProto(std::string name, std::string commod, double capacity);

  /// Runs the simulation.  This can only be called once.  After the simulation
  /// has been run, this MockSim object CANNOT be reused to run other
  /// simulations.  Run returns the agent ID for the agent being tested for
  /// use in queries.
  int Run();

  /// Reconstructs a material object from the simulation results database with
  /// the given resource state id.
  Material::Ptr GetMaterial(int resid);

  /// Reconstructs a product object from the simulation results database with
  /// the given resource state id.
  Product::Ptr GetProduct(int resid);

  /// Returns the underlying in-memory database containing results for
  /// the simulation.  Run must be called before the database will contain
  /// anything.
  SqliteBack& db();

  /// Returns the context for the mock simulation environment.
  Context* context() {return &ctx_;}

  /// the agent being tested by the mock simulation environment.
  Agent* agent;

 private:
  Context ctx_;
  Timer ti_;
  Recorder rec_;
  SqliteBack* back_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_MOCK_SIM_H_
