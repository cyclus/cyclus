#include <gtest/gtest.h>

#include <sstream>

#include "cyclus.h"

using pyne::nucname::id;
using cyclus::Composition;
using cyclus::Material;
using cyclus::QueryResult;
using cyclus::Cond;
using cyclus::toolkit::MatQuery;

namespace cycamore {
namespace reactortests {

Composition::Ptr c_uox() {
  cyclus::CompMap m;
  m[id("u235")] = 0.04;
  m[id("u238")] = 0.96;
  return Composition::CreateFromMass(m);
};

Composition::Ptr c_mox() {
  cyclus::CompMap m;
  m[id("u235")] = .7;
  m[id("u238")] = 100;
  m[id("pu239")] = 3.3;
  return Composition::CreateFromMass(m);
};

Composition::Ptr c_spentuox() {
  cyclus::CompMap m;
  m[id("u235")] =  .8;
  m[id("u238")] =  100;
  m[id("pu239")] = 1;
  return Composition::CreateFromMass(m);
};

Composition::Ptr c_spentmox() {
  cyclus::CompMap m;
  m[id("u235")] =  .2;
  m[id("u238")] =  100;
  m[id("pu239")] = .9;
  return Composition::CreateFromMass(m);
};

Composition::Ptr c_water() {
  cyclus::CompMap m;
  m[id("O16")] =  1;
  m[id("H1")] =  2;
  return Composition::CreateFromAtom(m);
};

// Test that with a zero refuel_time and a zero capacity fresh fuel buffer
// (the default), fuel can be ordered and the cycle started with no time step
// delay.
TEST(ReactorTests, JustInTimeOrdering) {
  std::string config =
     "  <fuel_inrecipes>  <val>lwr_fresh</val>  </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>lwr_spent</val>  </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>enriched_u</val> </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>      </fuel_outcommods>  "
     "  <fuel_prefs>      <val>1.0</val>        </fuel_prefs>  "
     ""
     "  <cycle_time>1</cycle_time>  "
     "  <refuel_time>0</refuel_time>  "
     "  <assem_size>300</assem_size>  "
     "  <n_assem_core>1</n_assem_core>  "
     "  <n_assem_batch>1</n_assem_batch>  ";

  int simdur = 50;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("enriched_u").Finalize();
  sim.AddRecipe("lwr_fresh", c_uox());
  sim.AddRecipe("lwr_spent", c_spentuox());
  int id = sim.Run();

  QueryResult qr = sim.db().Query("Transactions", NULL);
  EXPECT_EQ(simdur, qr.rows.size()) << "failed to order+run on fresh fuel inside 1 time step";
}

// tests that the correct number of assemblies are popped from the core each
// cycle.
TEST(ReactorTests, BatchSizes) {
  std::string config =
     "  <fuel_inrecipes>  <val>uox</val>      </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>spentuox</val> </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>uox</val>      </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>    </fuel_outcommods>  "
     ""
     "  <cycle_time>1</cycle_time>  "
     "  <refuel_time>0</refuel_time>  "
     "  <assem_size>1</assem_size>  "
     "  <n_assem_core>7</n_assem_core>  "
     "  <n_assem_batch>3</n_assem_batch>  ";

  int simdur = 50;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("uox").Finalize();
  sim.AddRecipe("uox", c_uox());
  sim.AddRecipe("spentuox", c_spentuox());
  int id = sim.Run();

  QueryResult qr = sim.db().Query("Transactions", NULL);
  // 7 for initial core, 3 per time step for each new batch for remainder
  EXPECT_EQ(7+3*(simdur-1), qr.rows.size());
}

// tests that the refueling period between cycle end and start of the next
// cycle is honored.
TEST(ReactorTests, RefuelTimes) {
  std::string config =
     "  <fuel_inrecipes>  <val>uox</val>      </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>spentuox</val> </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>uox</val>      </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>    </fuel_outcommods>  "
     ""
     "  <cycle_time>4</cycle_time>  "
     "  <refuel_time>3</refuel_time>  "
     "  <assem_size>1</assem_size>  "
     "  <n_assem_core>1</n_assem_core>  "
     "  <n_assem_batch>1</n_assem_batch>  ";

  int simdur = 49;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("uox").Finalize();
  sim.AddRecipe("uox", c_uox());
  sim.AddRecipe("spentuox", c_spentuox());
  int id = sim.Run();

  QueryResult qr = sim.db().Query("Transactions", NULL);
  int cyclet = 4;
  int refuelt = 3;
  int n_assem_want = simdur/(cyclet+refuelt)+1; // +1 for initial core
  EXPECT_EQ(n_assem_want, qr.rows.size());
}


// tests that a reactor decommissions on time without producing
// power at the end of its lifetime.
TEST(ReactorTests, DecomTimes) {
  std::string config =
     "  <fuel_inrecipes>  <val>uox</val>      </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>spentuox</val> </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>uox</val>      </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>    </fuel_outcommods>  "
     ""
     "  <cycle_time>2</cycle_time>  "
     "  <refuel_time>2</refuel_time>  "
     "  <assem_size>1</assem_size>  "
     "  <n_assem_core>3</n_assem_core>  "
     "  <power_cap>1000</power_cap>  "
     "  <n_assem_batch>1</n_assem_batch>  ";

  int simdur = 12;
  int lifetime = 7;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur, lifetime);
  sim.AddSource("uox").Finalize();
  sim.AddRecipe("uox", c_uox());
  sim.AddRecipe("spentuox", c_spentuox());
  int id = sim.Run();

  // operating for 2+2 months and shutdown for 2+1
  int on_time = 4;
  std::vector<Cond> conds;
  conds.push_back(Cond("Value", "==", 1000));
  QueryResult qr = sim.db().Query("TimeSeriesPower", &conds);
  EXPECT_EQ(on_time, qr.rows.size());

  int off_time = 3;
  conds.clear();
  conds.push_back(Cond("Value", "==", 0));
  qr = sim.db().Query("TimeSeriesPower", &conds);
  EXPECT_EQ(off_time, qr.rows.size());
}


// Tests if a reactor produces power at the time of its decommission
// given a refuel_time of zero.
TEST(ReactorTests, DecomZeroRefuel) {
  std::string config =
     "  <fuel_inrecipes>  <val>uox</val>      </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>spentuox</val> </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>uox</val>      </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>    </fuel_outcommods>  "
     ""
     "  <cycle_time>2</cycle_time>  "
     "  <refuel_time>0</refuel_time>  "
     "  <assem_size>1</assem_size>  "
     "  <n_assem_core>3</n_assem_core>  "
     "  <power_cap>1000</power_cap>  "
     "  <n_assem_batch>1</n_assem_batch>  ";

  int simdur = 8;
  int lifetime = 6;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur, lifetime);
  sim.AddSource("uox").Finalize();
  sim.AddRecipe("uox", c_uox());
  sim.AddRecipe("spentuox", c_spentuox());
  int id = sim.Run();

  // operating for 2+2 months and shutdown for 2+1
  int on_time = 6;
  std::vector<Cond> conds;
  conds.push_back(Cond("Value", "==", 1000));
  QueryResult qr = sim.db().Query("TimeSeriesPower", &conds);
  EXPECT_EQ(on_time, qr.rows.size());
}

// tests that new fuel is ordered immediately following cycle end - at the
// start of the refueling period - not before and not after. - thie is subtly
// different than RefuelTimes test and is not a duplicate of it.
TEST(ReactorTests, OrderAtRefuelStart) {
  std::string config =
     "  <fuel_inrecipes>  <val>uox</val>      </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>spentuox</val> </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>uox</val>      </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>    </fuel_outcommods>  "
     ""
     "  <cycle_time>4</cycle_time>  "
     "  <refuel_time>3</refuel_time>  "
     "  <assem_size>1</assem_size>  "
     "  <n_assem_core>1</n_assem_core>  "
     "  <n_assem_batch>1</n_assem_batch>  ";

  int simdur = 7;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("uox").Finalize();
  sim.AddRecipe("uox", c_uox());
  sim.AddRecipe("spentuox", c_spentuox());
  int id = sim.Run();

  QueryResult qr = sim.db().Query("Transactions", NULL);
  int cyclet = 4;
  int refuelt = 3;
  int n_assem_want = simdur/(cyclet+refuelt)+1; // +1 for initial core
  EXPECT_EQ(n_assem_want, qr.rows.size());
}

// tests that the reactor handles requesting multiple types of fuel correctly
// - with proper inventory constraint honoring, etc.
TEST(ReactorTests, MultiFuelMix) {
  std::string config =
     "  <fuel_inrecipes>  <val>uox</val>      <val>mox</val>      </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>spentuox</val> <val>spentmox</val> </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>uox</val>      <val>mox</val>      </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>    <val>waste</val>    </fuel_outcommods>  "
     ""
     "  <cycle_time>1</cycle_time>  "
     "  <refuel_time>0</refuel_time>  "
     "  <assem_size>1</assem_size>  "
     "  <n_assem_fresh>3</n_assem_fresh>  "
     "  <n_assem_core>3</n_assem_core>  "
     "  <n_assem_batch>3</n_assem_batch>  ";

  // it is important that the sources have cumulative capacity greater than
  // the reactor can take on a single time step - to test that inventory
  // capacity constraints are being set properly.  It is also important that
  // each source is smaller capacity thatn the reactor orders on each time
  // step to make it easy to compute+check the number of transactions.
  int simdur = 50;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("uox").capacity(2).Finalize();
  sim.AddSource("mox").capacity(2).Finalize();
  sim.AddRecipe("uox", c_uox());
  sim.AddRecipe("spentuox", c_spentuox());
  sim.AddRecipe("mox", c_spentuox());
  sim.AddRecipe("spentmox", c_spentuox());
  int id = sim.Run();

  QueryResult qr = sim.db().Query("Transactions", NULL);
  // +3 is for fresh fuel inventory
  EXPECT_EQ(3*simdur+3, qr.rows.size());
}

// tests that the reactor halts operation when it has no more room in its
// spent fuel inventory buffer.
TEST(ReactorTests, FullSpentInventory) {
  std::string config =
     "  <fuel_inrecipes>  <val>uox</val>      </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>spentuox</val> </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>uox</val>      </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>    </fuel_outcommods>  "
     ""
     "  <cycle_time>1</cycle_time>  "
     "  <refuel_time>0</refuel_time>  "
     "  <assem_size>1</assem_size>  "
     "  <n_assem_core>1</n_assem_core>  "
     "  <n_assem_batch>1</n_assem_batch>  "
     "  <n_assem_spent>3</n_assem_spent>  ";

  int simdur = 10;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("uox").Finalize();
  sim.AddRecipe("uox", c_uox());
  sim.AddRecipe("spentuox", c_spentuox());
  int id = sim.Run();

  QueryResult qr = sim.db().Query("Transactions", NULL);
  int n_assem_spent = 3;

  // +1 is for the assembly in the core + the three in spent
  EXPECT_EQ(n_assem_spent+1, qr.rows.size());
}

// tests that the reactor shuts down, ie., does not generate power, when the
// spent fuel inventory is full and the core cannot be unloaded.
TEST(ReactorTests, FullSpentInventoryShutdown) {
  std::string config =
    " <fuel_inrecipes> <val>uox</val> </fuel_inrecipes> "
    " <fuel_outrecipes> <val>spentuox</val> </fuel_outrecipes> "
    " <fuel_incommods> <val>uox</val> </fuel_incommods> "
    " <fuel_outcommods> <val>waste</val> </fuel_outcommods> "
    ""
    " <cycle_time>1</cycle_time> "
    " <refuel_time>0</refuel_time> "
    " <assem_size>1</assem_size> "
    " <n_assem_core>1</n_assem_core> "
    " <n_assem_batch>1</n_assem_batch> "
    " <n_assem_spent>1</n_assem_spent> "
    " <power_cap>100</power_cap> ";

  int simdur = 3;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("uox").Finalize();
  sim.AddRecipe("uox", c_uox());
  sim.AddRecipe("spentuox", c_spentuox());
  int id = sim.Run();

  QueryResult qr = sim.db().Query("TimeSeriesPower", NULL);
  EXPECT_EQ(0, qr.GetVal<double>("Value", simdur - 1));

}

// tests that the reactor cycle is delayed as expected when it is unable to
// acquire fuel in time for the next cycle start.  This checks that after a
// cycle is delayed past an original scheduled start time, as soon as enough fuel is
// received, a new cycle pattern is established starting from the delayed
// start time.
TEST(ReactorTests, FuelShortage) {
  std::string config =
     "  <fuel_inrecipes>  <val>uox</val>      </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>spentuox</val> </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>uox</val>      </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>    </fuel_outcommods>  "
     ""
     "  <cycle_time>7</cycle_time>  "
     "  <refuel_time>0</refuel_time>  "
     "  <assem_size>1</assem_size>  "
     "  <n_assem_core>3</n_assem_core>  "
     "  <n_assem_batch>3</n_assem_batch>  ";

  int simdur = 50;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("uox").lifetime(1).Finalize(); // provide initial full batch
  sim.AddSource("uox").start(9).lifetime(1).capacity(2).Finalize(); // provide partial batch post cycle-end
  sim.AddSource("uox").start(15).Finalize(); // provide remainder of batch much later
  sim.AddRecipe("uox", c_uox());
  sim.AddRecipe("spentuox", c_spentuox());
  int id = sim.Run();

  // check that we never got a full refueled batch during refuel period
  std::vector<Cond> conds;
  conds.push_back(Cond("Time", "<", 15));
  QueryResult qr = sim.db().Query("Transactions", &conds);
  EXPECT_EQ(5, qr.rows.size());

  // after being delayed past original scheduled start of new cycle, we got
  // final assembly for core.
  conds.clear();
  conds.push_back(Cond("Time", "==", 15));
  qr = sim.db().Query("Transactions", &conds);
  EXPECT_EQ(1, qr.rows.size());

  // all during the next (delayed) cycle we shouldn't be requesting any new fuel
  conds.clear();
  conds.push_back(Cond("Time", "<", 21));
  qr = sim.db().Query("Transactions", &conds);
  EXPECT_EQ(6, qr.rows.size());

  // as soon as this delayed cycle ends, we should be requesting/getting 3 new batches
  conds.clear();
  conds.push_back(Cond("Time", "==", 22));
  qr = sim.db().Query("Transactions", &conds);
  EXPECT_EQ(3, qr.rows.size());
}

// tests that discharged fuel is transmuted properly immediately at cycle end.
TEST(ReactorTests, DischargedFuelTransmute) {
  std::string config =
     "  <fuel_inrecipes>  <val>uox</val>      </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>spentuox</val> </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>uox</val>      </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>    </fuel_outcommods>  "
     ""
     "  <cycle_time>4</cycle_time>  "
     "  <refuel_time>3</refuel_time>  "
     "  <assem_size>1</assem_size>  "
     "  <n_assem_core>1</n_assem_core>  "
     "  <n_assem_batch>1</n_assem_batch>  ";

  int simdur = 7;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("uox").Finalize();
  sim.AddSink("waste").Finalize();
  sim.AddRecipe("uox", c_uox());
  Composition::Ptr spentuox = c_spentuox();
  sim.AddRecipe("spentuox", spentuox);
  int id = sim.Run();

  std::vector<Cond> conds;
  conds.push_back(Cond("SenderId", "==", id));
  int resid = sim.db().Query("Transactions", &conds).GetVal<int>("ResourceId");
  Material::Ptr m = sim.GetMaterial(resid);
  MatQuery mq(m);
  EXPECT_EQ(spentuox->id(), m->comp()->id());
  EXPECT_TRUE(mq.mass(942390000) > 0) << "transmuted spent fuel doesn't have Pu239";
}

// tests that spent fuel is offerred on correct commods according to the
// incommod it was received on - esp when dealing with multiple fuel commods
// simultaneously.
TEST(ReactorTests, SpentFuelProperCommodTracking) {
  std::string config =
     "  <fuel_inrecipes>  <val>uox</val>      <val>mox</val>      </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>spentuox</val> <val>spentmox</val> </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>uox</val>      <val>mox</val>      </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste1</val>   <val>waste2</val>   </fuel_outcommods>  "
     ""
     "  <cycle_time>1</cycle_time>  "
     "  <refuel_time>0</refuel_time>  "
     "  <assem_size>1</assem_size>  "
     "  <n_assem_core>3</n_assem_core>  "
     "  <n_assem_batch>3</n_assem_batch>  ";

  int simdur = 7;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("uox").capacity(1).Finalize();
  sim.AddSource("mox").capacity(2).Finalize();
  sim.AddSink("waste1").Finalize();
  sim.AddSink("waste2").Finalize();
  sim.AddRecipe("uox", c_uox());
  sim.AddRecipe("spentuox", c_spentuox());
  sim.AddRecipe("mox", c_mox());
  sim.AddRecipe("spentmox", c_spentmox());
  int id = sim.Run();

  std::vector<Cond> conds;
  conds.push_back(Cond("SenderId", "==", id));
  conds.push_back(Cond("Commodity", "==", std::string("waste1")));
  QueryResult qr = sim.db().Query("Transactions", &conds);
  EXPECT_EQ(simdur-1, qr.rows.size());

  conds[1] = Cond("Commodity", "==", std::string("waste2"));
  qr = sim.db().Query("Transactions", &conds);
  EXPECT_EQ(2*(simdur-1), qr.rows.size());
}

// The user can optionally omit fuel preferences.  In the case where
// preferences are adjusted, the ommitted preference vector must be populated
// with default values - if it wasn't then preferences won't be adjusted
// correctly and the reactor could segfault.  Check that this doesn't happen.
TEST(ReactorTests, PrefChange) {
  // it is important that the fuel_prefs not be present in the config below.
  std::string config =
     "  <fuel_inrecipes>  <val>lwr_fresh</val>  </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>lwr_spent</val>  </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>enriched_u</val> </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>      </fuel_outcommods>  "
     ""
     "  <cycle_time>1</cycle_time>  "
     "  <refuel_time>0</refuel_time>  "
     "  <assem_size>300</assem_size>  "
     "  <n_assem_core>1</n_assem_core>  "
     "  <n_assem_batch>1</n_assem_batch>  "
     ""
     "  <pref_change_times>   <val>25</val>         </pref_change_times>"
     "  <pref_change_commods> <val>enriched_u</val> </pref_change_commods>"
     "  <pref_change_values>  <val>-1</val>         </pref_change_values>";

  int simdur = 50;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("enriched_u").Finalize();
  sim.AddRecipe("lwr_fresh", c_uox());
  sim.AddRecipe("lwr_spent", c_spentuox());
  int id = sim.Run();

  QueryResult qr = sim.db().Query("Transactions", NULL);
  EXPECT_EQ(25, qr.rows.size()) << "failed to adjust preferences properly";
}

TEST(ReactorTests, RecipeChange) {
  // it is important that the fuel_prefs not be present in the config below.
  std::string config =
     "  <fuel_inrecipes>  <val>lwr_fresh</val>  </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>lwr_spent</val>  </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>enriched_u</val> </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>      </fuel_outcommods>  "
     ""
     "  <cycle_time>1</cycle_time>  "
     "  <refuel_time>0</refuel_time>  "
     "  <assem_size>300</assem_size>  "
     "  <n_assem_core>1</n_assem_core>  "
     "  <n_assem_batch>1</n_assem_batch>  "
     ""
     "  <recipe_change_times>   <val>25</val>         <val>35</val>         </recipe_change_times>"
     "  <recipe_change_commods> <val>enriched_u</val> <val>enriched_u</val> </recipe_change_commods>"
     "  <recipe_change_in>      <val>water</val>      <val>water</val>      </recipe_change_in>"
     "  <recipe_change_out>     <val>lwr_spent</val>  <val>water</val>      </recipe_change_out>";

  int simdur = 50;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("enriched_u").Finalize();
  sim.AddSink("waste").Finalize();
  sim.AddRecipe("lwr_fresh", c_uox());
  sim.AddRecipe("lwr_spent", c_spentuox());
  sim.AddRecipe("water", c_water());
  int aid = sim.Run();

  std::vector<Cond> conds;
  QueryResult qr;

  // check that received recipe is not water
  conds.clear();
  conds.push_back(Cond("Time", "==", 24));
  conds.push_back(Cond("ReceiverId", "==", aid));
  qr = sim.db().Query("Transactions", &conds);
  MatQuery mq = MatQuery(sim.GetMaterial(qr.GetVal<int>("ResourceId")));

  EXPECT_TRUE(0 < mq.qty());
  EXPECT_TRUE(0 == mq.mass(id("H1")));

  // check that received recipe changed to water
  conds.clear();
  conds.push_back(Cond("Time", "==", 26));
  conds.push_back(Cond("ReceiverId", "==", aid));
  qr = sim.db().Query("Transactions", &conds);
  mq = MatQuery(sim.GetMaterial(qr.GetVal<int>("ResourceId")));

  EXPECT_TRUE(0 < mq.qty());
  EXPECT_TRUE(0 < mq.mass(id("H1")));

  // check that sent recipe is not water
  conds.clear();
  conds.push_back(Cond("Time", "==", 34));
  conds.push_back(Cond("SenderId", "==", aid));
  qr = sim.db().Query("Transactions", &conds);
  mq = MatQuery(sim.GetMaterial(qr.GetVal<int>("ResourceId")));

  EXPECT_TRUE(0 < mq.qty());
  EXPECT_TRUE(0 == mq.mass(id("H1")));

  // check that sent recipe changed to water
  conds.clear();
  conds.push_back(Cond("Time", "==", 36));
  conds.push_back(Cond("SenderId", "==", aid));
  qr = sim.db().Query("Transactions", &conds);
  mq = MatQuery(sim.GetMaterial(qr.GetVal<int>("ResourceId")));

  EXPECT_TRUE(0 < mq.qty());
  EXPECT_TRUE(0 < mq.mass(id("H1")));
}

TEST(ReactorTests, Retire) {
  std::string config =
     "  <fuel_inrecipes>  <val>lwr_fresh</val>  </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>lwr_spent</val>  </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>enriched_u</val> </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>      </fuel_outcommods>  "
     ""
     "  <cycle_time>7</cycle_time>  "
     "  <refuel_time>0</refuel_time>  "
     "  <assem_size>300</assem_size>  "
     "  <n_assem_fresh>1</n_assem_fresh>  "
     "  <n_assem_core>3</n_assem_core>  "
     "  <n_assem_batch>1</n_assem_batch>  "
     "  <power_cap>1</power_cap>  "
     "";

  int dur = 50;
  int life = 36;
  int cycle_time = 7;
  int refuel_time = 0;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, dur, life);
  sim.AddSource("enriched_u").Finalize();
  sim.AddSink("waste").Finalize();
  sim.AddRecipe("lwr_fresh", c_uox());
  sim.AddRecipe("lwr_spent", c_spentuox());
  int id = sim.Run();

  int ncore = 3;
  int nbatch = 1;

  // reactor should stop requesting new fresh fuel as it approaches retirement
  int nassem_recv =
      static_cast<int>(ceil(static_cast<double>(life) / 7.0)) * nbatch +
      (ncore - nbatch);

  std::vector<Cond> conds;
  conds.push_back(Cond("ReceiverId", "==", id));
  QueryResult qr = sim.db().Query("Transactions", &conds);
  EXPECT_EQ(nassem_recv, qr.rows.size())
      << "failed to stop ordering near retirement";

  // reactor should discharge all fuel before/by retirement
  conds.clear();
  conds.push_back(Cond("SenderId", "==", id));
  qr = sim.db().Query("Transactions", &conds);
  EXPECT_EQ(nassem_recv, qr.rows.size())
      << "failed to discharge all material by retirement time";

  // reactor should record power entry on the time step it retires if operating
  int time_online = life / (cycle_time + refuel_time) * cycle_time + std::min(life % (cycle_time + refuel_time), cycle_time);
  conds.clear();
  conds.push_back(Cond("AgentId", "==", id));
  conds.push_back(Cond("Value", ">", 0));
  qr = sim.db().Query("TimeSeriesPower", &conds);
  EXPECT_EQ(time_online, qr.rows.size())
      << "failed to generate power for the correct number of time steps";
}

TEST(ReactorTests, PositionInitialize) {
  std::string config =
     "  <fuel_inrecipes>  <val>lwr_fresh</val>  </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>lwr_spent</val>  </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>enriched_u</val> </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>      </fuel_outcommods>  "
     "  <fuel_prefs>      <val>1.0</val>        </fuel_prefs>  "
     ""
     "  <cycle_time>1</cycle_time>  "
     "  <refuel_time>0</refuel_time>  "
     "  <assem_size>300</assem_size>  "
     "  <n_assem_core>1</n_assem_core>  "
     "  <n_assem_batch>1</n_assem_batch>  ";

  int simdur = 50;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("enriched_u").Finalize();
  sim.AddRecipe("lwr_fresh", c_uox());
  sim.AddRecipe("lwr_spent", c_spentuox());
  int id = sim.Run();

  QueryResult qr = sim.db().Query("AgentPosition", NULL);
  EXPECT_EQ(qr.GetVal<double>("Latitude"), 0.0);
  EXPECT_EQ(qr.GetVal<double>("Longitude"), 0.0);
}

TEST(ReactorTests, PositionInitialize2) {
  std::string config =
     "  <fuel_inrecipes>  <val>lwr_fresh</val>  </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>lwr_spent</val>  </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>enriched_u</val> </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>      </fuel_outcommods>  "
     "  <fuel_prefs>      <val>1.0</val>        </fuel_prefs>  "
     ""
     "  <cycle_time>1</cycle_time>  "
     "  <refuel_time>0</refuel_time>  "
     "  <assem_size>300</assem_size>  "
     "  <n_assem_core>1</n_assem_core>  "
     "  <n_assem_batch>1</n_assem_batch>  "
     "  <longitude>30.0</longitude>  "
     "  <latitude>30.0</latitude>  ";

  int simdur = 50;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("enriched_u").Finalize();
  sim.AddRecipe("lwr_fresh", c_uox());
  sim.AddRecipe("lwr_spent", c_spentuox());
  int id = sim.Run();

  QueryResult qr = sim.db().Query("AgentPosition", NULL);
  EXPECT_EQ(qr.GetVal<double>("Latitude"), 30.0);
  EXPECT_EQ(qr.GetVal<double>("Longitude"), 30.0);
}

TEST(ReactorTests, ByProduct) {
  std::string config =
     "  <fuel_inrecipes>  <val>uox</val>      </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>spentuox</val> </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>uox</val>      </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>    </fuel_outcommods>  "
     ""
     "  <cycle_time>1</cycle_time>  "
     "  <refuel_time>1</refuel_time>  "
     "  <assem_size>1</assem_size>  "
     "  <n_assem_core>7</n_assem_core>  "
     "  <n_assem_batch>3</n_assem_batch>  "
     ""
     "  <side_products> <val>process_heat</val> </side_products>"
     "  <side_product_quantity> <val>10</val> </side_product_quantity>";

  int simdur = 10;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("uox").Finalize();
  sim.AddRecipe("uox", c_uox());
  sim.AddRecipe("spentuox", c_spentuox());
  int id = sim.Run();

  std::vector<Cond> conds;
  // test if it produces side products only when reactor is running
  int quantity = 10;
  conds.push_back(Cond("Value", "==", quantity));
  QueryResult qr = sim.db().Query("ReactorSideProducts", &conds);
  EXPECT_EQ(5, qr.rows.size());

  // test if it doesn't produce side products when reactor is refueling
  conds.clear();
  conds.push_back(Cond("Value", "==", 0));
  qr = sim.db().Query("ReactorSideProducts", &conds);
  EXPECT_EQ(5, qr.rows.size());
}

TEST(ReactorTests, MultipleByProduct) {
  std::string config =
     "  <fuel_inrecipes>  <val>uox</val>      </fuel_inrecipes>  "
     "  <fuel_outrecipes> <val>spentuox</val> </fuel_outrecipes>  "
     "  <fuel_incommods>  <val>uox</val>      </fuel_incommods>  "
     "  <fuel_outcommods> <val>waste</val>    </fuel_outcommods>  "
     ""
     "  <cycle_time>1</cycle_time>  "
     "  <refuel_time>1</refuel_time>  "
     "  <assem_size>1</assem_size>  "
     "  <n_assem_core>7</n_assem_core>  "
     "  <n_assem_batch>3</n_assem_batch>  "
     ""
     "  <side_products> <val>process_heat</val> <val>water</val> </side_products>"
     "  <side_product_quantity> <val>10</val> <val>100</val> </side_product_quantity>";

  int simdur = 10;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Reactor"), config, simdur);
  sim.AddSource("uox").Finalize();
  sim.AddRecipe("uox", c_uox());
  sim.AddRecipe("spentuox", c_spentuox());
  int id = sim.Run();


  std::vector<Cond> conds;
  // test if it produces heat when reactor is running
  int quantity = 10;
  conds.push_back(Cond("Product", "==", std::string("process_heat")));
  conds.push_back(Cond("Value", "==", quantity));
  QueryResult qr = sim.db().Query("ReactorSideProducts", &conds);
  EXPECT_EQ(5, qr.rows.size());

  // test if it produces water when reactor is running
  conds.clear();
  quantity = 100;
  conds.push_back(Cond("Product", "==", std::string("water")));
  conds.push_back(Cond("Value", "==", quantity));
  qr = sim.db().Query("ReactorSideProducts", &conds);
  EXPECT_EQ(5, qr.rows.size());

  conds.clear();
  conds.push_back(Cond("Value", "==", 0));
  qr = sim.db().Query("ReactorSideProducts", &conds);
  EXPECT_EQ(10, qr.rows.size());

}

} // namespace reactortests
} // namespace cycamore

