#include "separations.h"

#include <gtest/gtest.h>
#include <sstream>
#include "cyclus.h"

using pyne::nucname::id;
using cyclus::Composition;
using cyclus::CompMap;
using cyclus::Material;
using cyclus::QueryResult;
using cyclus::Cond;
using cyclus::toolkit::MatQuery;

namespace cycamore {

TEST(SeparationsTests, SepMaterial) {
  CompMap comp;
  comp[id("U235")] = 10;
  comp[id("U238")] = 90;
  comp[id("Pu239")] = 1;
  comp[id("Pu240")] = 2;
  comp[id("Am241")] = 3;
  comp[id("Am242")] = 2.8;
  double qty = 100;
  Composition::Ptr c = Composition::CreateFromMass(comp);
  Material::Ptr mat = Material::CreateUntracked(qty, c);

  std::map<int, double> effs;
  effs[id("U")] = .7;
  effs[id("Pu")] = .4;
  effs[id("Am241")] = .4;

  Material::Ptr sep = SepMaterial(effs, mat);
  MatQuery mqorig(mat);
  MatQuery mqsep(sep);

  EXPECT_DOUBLE_EQ(effs[id("U")] * mqorig.mass("U235"), mqsep.mass("U235"));
  EXPECT_DOUBLE_EQ(effs[id("U")] * mqorig.mass("U238"), mqsep.mass("U238"));
  EXPECT_DOUBLE_EQ(effs[id("Pu")] * mqorig.mass("Pu239"), mqsep.mass("Pu239"));
  EXPECT_DOUBLE_EQ(effs[id("Pu")] * mqorig.mass("Pu240"), mqsep.mass("Pu240"));
  EXPECT_DOUBLE_EQ(effs[id("Am241")] * mqorig.mass("Am241"), mqsep.mass("Am241"));
  EXPECT_DOUBLE_EQ(0, mqsep.mass("Am242"));
}


// Check that cumulative separations efficiency for a single nuclide of less than or equal to one does not trigger an error.
TEST(SeparationsTests, SeparationEfficiency) {

  int simdur = 2;
  std::string config =
      "<streams>"
      "    <item>"
      "        <commod>stream1</commod>"
      "        <info>"
      "            <buf_size>-1</buf_size>"
      "            <efficiencies>"
      "                <item><comp>Pu239</comp> <eff>.2</eff></item>"
      "            </efficiencies>"
      "        </info>"
      "    </item>"
      "    <item>"
      "        <commod>stream1</commod>"
      "        <info>"
      "            <buf_size>-1</buf_size>"
      "            <efficiencies>"
      "                <item><comp>Pu239</comp> <eff>.1</eff></item>"
      "            </efficiencies>"
      "        </info>"
      "    </item>"
      "    <item>"
      "        <commod>stream1</commod>"
      "        <info>"
      "            <buf_size>-1</buf_size>"
      "            <efficiencies>"
      "                <item><comp>Pu239</comp> <eff>.5</eff></item>"
      "            </efficiencies>"
      "        </info>"
      "    </item>"
      "</streams>"
      ""
      "<leftover_commod>waste</leftover_commod>"
      "<throughput>100</throughput>"
      "<feedbuf_size>100</feedbuf_size>"
      "<feed_commods> <val>feed</val> </feed_commods>"
  ;


  cyclus::MockSim sim1(cyclus::AgentSpec(":cycamore:Separations"), config, simdur);

  EXPECT_NO_THROW(sim1.Run()) << "Cumulative separation efficiency smaler than 1 is throwing an error but should not.";


  config =
      "<streams>"
      "    <item>"
      "        <commod>stream1</commod>"
      "        <info>"
      "            <buf_size>-1</buf_size>"
      "            <efficiencies>"
      "                <item><comp>U</comp> <eff>0.1</eff></item>"
      "                <item><comp>Pu239</comp> <eff>.2</eff></item>"
      "            </efficiencies>"
      "        </info>"
      "    </item>"
      "    <item>"
      "        <commod>stream1</commod>"
      "        <info>"
      "            <buf_size>-1</buf_size>"
      "            <efficiencies>"
      "                <item><comp>U</comp> <eff>0.2</eff></item>"
      "                <item><comp>Pu239</comp> <eff>.1</eff></item>"
      "            </efficiencies>"
      "        </info>"
      "    </item>"
      "    <item>"
      "        <commod>stream1</commod>"
      "        <info>"
      "            <buf_size>-1</buf_size>"
      "            <efficiencies>"
      "                <item><comp>U</comp> <eff>0.7</eff></item>"
      "                <item><comp>Pu239</comp> <eff>.5</eff></item>"
      "            </efficiencies>"
      "        </info>"
      "    </item>"
      "</streams>"
      ""
      "<leftover_commod>waste</leftover_commod>"
      "<throughput>100</throughput>"
      "<feedbuf_size>100</feedbuf_size>"
      "<feed_commods> <val>feed</val> </feed_commods>"
      ;


  cyclus::MockSim sim2(cyclus::AgentSpec(":cycamore:Separations"), config, simdur);

  EXPECT_NO_THROW(sim2.Run()) << "Cumulative separation efficiency of 1 is throwing an error but should not.";
}

// Check that an error is correctly thrown when separations efficiency of greater than one.
TEST(SeparationsTests, SeparationEfficiencyThrowing) {
  int simdur = 2;

  // Check that single separations efficiency for a single nuclide of greater than one does not trigger an error.
  std::string config =
      "<streams>"
      "    <item>"
      "        <commod>stream1</commod>"
      "        <info>"
      "            <buf_size>-1</buf_size>"
      "            <efficiencies>"
      "                <item><comp>U</comp> <eff>1.6</eff></item>"
      "                <item><comp>Pu239</comp> <eff>.7</eff></item>"
      "            </efficiencies>"
      "        </info>"
      "    </item>"
      "</streams>"
      ""
      "<leftover_commod>waste</leftover_commod>"
      "<throughput>100</throughput>"
      "<feedbuf_size>100</feedbuf_size>"
      "<feed_commods> <val>feed</val> </feed_commods>"
      ;

  cyclus::MockSim sim1(cyclus::AgentSpec(":cycamore:Separations"), config, simdur);

  EXPECT_THROW(sim1.Run(), cyclus::ValueError) << "Direct separation efficiency greater than 1 is not throwing an error but should.";

// Check if a cumulative separation efficiency greater than 1 for a unique nuclide throw an error as expected.
  config =
    "<streams>"
    "    <item>"
    "        <commod>stream1</commod>"
    "        <info>"
    "            <buf_size>-1</buf_size>"
    "            <efficiencies>"
    "                <item><comp>U</comp> <eff>0.6</eff></item>"
    "                <item><comp>Pu239</comp> <eff>.7</eff></item>"
    "            </efficiencies>"
    "        </info>"
    "    </item>"
    "    <item>"
    "        <commod>stream2</commod>"
    "        <info>"
    "            <buf_size>-1</buf_size>"
    "            <efficiencies>"
    "                <item><comp>U</comp> <eff>0.1</eff></item>"
    "                <item><comp>Pu239</comp> <eff>.7</eff></item>"
    "            </efficiencies>"
    "        </info>"
    "    </item>"
    "</streams>"
    ""
    "<leftover_commod>waste</leftover_commod>"
    "<throughput>100</throughput>"
    "<feedbuf_size>100</feedbuf_size>"
    "<feed_commods> <val>feed</val> </feed_commods>"
    ;

  cyclus::MockSim sim2(cyclus::AgentSpec(":cycamore:Separations"), config, simdur);

  EXPECT_THROW(sim2.Run(), cyclus::ValueError) << "Single cumulative separation efficiency greater than 1 is not throwing an error but should.";
// Check if a cumulative separation efficiency greater than 1 for multiple nuclides throw an error as expected.
  config =
      "<streams>"
      "    <item>"
      "        <commod>stream1</commod>"
      "        <info>"
      "            <buf_size>-1</buf_size>"
      "            <efficiencies>"
      "                <item><comp>U</comp> <eff>0.6</eff></item>"
      "                <item><comp>Pu239</comp> <eff>.7</eff></item>"
      "            </efficiencies>"
      "        </info>"
      "    </item>"
      "    <item>"
      "        <commod>stream2</commod>"
      "        <info>"
      "            <buf_size>-1</buf_size>"
      "            <efficiencies>"
      "                <item><comp>U</comp> <eff>0.6</eff></item>"
      "                <item><comp>Pu239</comp> <eff>.7</eff></item>"
      "            </efficiencies>"
      "        </info>"
      "    </item>"
      "</streams>"
      ""
      "<leftover_commod>waste</leftover_commod>"
      "<throughput>100</throughput>"
      "<feedbuf_size>100</feedbuf_size>"
      "<feed_commods> <val>feed</val> </feed_commods>"
      ;

  cyclus::MockSim sim3(cyclus::AgentSpec(":cycamore:Separations"), config, simdur);

  EXPECT_THROW(sim3.Run(), cyclus::ValueError) << "Multiple cumulative separation efficiencies greater than 1 are not throwing an error but should.";
}

TEST(SeparationsTests, SepMixElemAndNuclide) {
  std::string config =
      "<streams>"
      "    <item>"
      "        <commod>stream1</commod>"
      "        <info>"
      "            <buf_size>-1</buf_size>"
      "            <efficiencies>"
      "                <item><comp>U</comp> <eff>0.6</eff></item>"
      "                <item><comp>Pu239</comp> <eff>.7</eff></item>"
      "            </efficiencies>"
      "        </info>"
      "    </item>"
      "</streams>"
      ""
      "<leftover_commod>waste</leftover_commod>"
      "<throughput>100</throughput>"
      "<feedbuf_size>100</feedbuf_size>"
      "<feed_commods> <val>feed</val> </feed_commods>"
     ;

  CompMap m;
  m[id("u235")] = 0.08;
  m[id("u238")] = 0.9;
  m[id("Pu239")] = .01;
  m[id("Pu240")] = .01;
  Composition::Ptr c = Composition::CreateFromMass(m);

  int simdur = 2;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Separations"), config, simdur);
  sim.AddSource("feed").recipe("recipe1").Finalize();
  sim.AddSink("stream1").capacity(100).Finalize();
  sim.AddRecipe("recipe1", c);
  int id = sim.Run();

  std::vector<Cond> conds;
  conds.push_back(Cond("SenderId", "==", id));
  int resid = sim.db().Query("Transactions", &conds).GetVal<int>("ResourceId");
  MatQuery mq (sim.GetMaterial(resid));
  EXPECT_DOUBLE_EQ(m[922350000]*0.6*100, mq.mass("U235"));
  EXPECT_DOUBLE_EQ(m[922380000]*0.6*100, mq.mass("U238"));
  EXPECT_DOUBLE_EQ(m[942390000]*0.7*100, mq.mass("Pu239"));
  EXPECT_DOUBLE_EQ(0, mq.mass("Pu240"));
}

TEST(SeparationsTests, Retire) {
  std::string config =
      "<streams>"
      "    <item>"
      "        <commod>stream1</commod>"
      "        <info>"
      "            <buf_size>-1</buf_size>"
      "            <efficiencies>"
      "                <item><comp>U235</comp> <eff>1.0</eff></item>"
      "            </efficiencies>"
      "        </info>"
      "    </item>"
      "</streams>"
      ""
      "<leftover_commod>waste</leftover_commod>"
      "<throughput>100</throughput>"
      "<feedbuf_size>100</feedbuf_size>"
      "<feed_commods> <val>feed</val> </feed_commods>"
     ;

  CompMap m;
  m[id("u235")] = 0.1;
  m[id("u238")] = 0.9;
  Composition::Ptr c = Composition::CreateFromMass(m);

  int simdur = 5;
  int life = 2;

  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Separations"),
          config, simdur, life);
  sim.AddSource("feed").recipe("recipe1").Finalize();
  sim.AddSink("stream1").capacity(100).Finalize();
  sim.AddSink("waste").capacity(70).Finalize();
  sim.AddRecipe("recipe1", c);
  int id = sim.Run();

  // Separations should stop requesting material at its lifetime
  // (it is smart enough to not request material on its last timestep because
  //  it knows it won't be able to process it)
  std::vector<Cond> conds;
  conds.push_back(Cond("ReceiverId", "==", id));
  QueryResult qr = sim.db().Query("Transactions", &conds);
  EXPECT_EQ(life - 1, qr.rows.size())
      << "failed to stop ordering near retirement";

  // Separations should discharge all material before decomissioning
  conds.clear();
  conds.push_back(Cond("SenderId", "==", id));
  qr = sim.db().Query("Transactions", &conds);
  double tot_mat = 0;
  for (int i = 0; i < qr.rows.size(); i++) {
    cyclus::Material::Ptr m = sim.GetMaterial(qr.GetVal<int>("ResourceId", i));
    tot_mat += m->quantity();
  }
  EXPECT_EQ(100, tot_mat)
    << "total material traded away does not equal total material separated";
  EXPECT_EQ(3.0, qr.rows.size())
      << "failed to discharge all material before decomissioning";
 }

 TEST(SeparationsTests, PositionInitialize) {
  std::string config =
      "<streams>"
      "    <item>"
      "        <commod>stream1</commod>"
      "        <info>"
      "            <buf_size>-1</buf_size>"
      "            <efficiencies>"
      "                <item><comp>U</comp> <eff>0.6</eff></item>"
      "                <item><comp>Pu239</comp> <eff>.7</eff></item>"
      "            </efficiencies>"
      "        </info>"
      "    </item>"
      "</streams>"
      ""
      "<leftover_commod>waste</leftover_commod>"
      "<throughput>100</throughput>"
      "<feedbuf_size>100</feedbuf_size>"
      "<feed_commods> <val>feed</val> </feed_commods>";
  CompMap m;
  m[id("u235")] = 0.08;
  m[id("u238")] = 0.9;
  m[id("Pu239")] = .01;
  m[id("Pu240")] = .01;
  Composition::Ptr c = Composition::CreateFromMass(m);

  int simdur = 2;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Separations"), config, simdur);
  sim.AddSource("feed").recipe("recipe1").Finalize();
  sim.AddSink("stream1").capacity(100).Finalize();
  sim.AddRecipe("recipe1", c);
  int id = sim.Run();

  QueryResult qr = sim.db().Query("AgentPosition", NULL);
  EXPECT_EQ(qr.GetVal<double>("Latitude"), 0.0);
  EXPECT_EQ(qr.GetVal<double>("Longitude"), 0.0);
 }

  TEST(SeparationsTests, PositionInitialize2) {
  std::string config =
      "<streams>"
      "    <item>"
      "        <commod>stream1</commod>"
      "        <info>"
      "            <buf_size>-1</buf_size>"
      "            <efficiencies>"
      "                <item><comp>U</comp> <eff>0.6</eff></item>"
      "                <item><comp>Pu239</comp> <eff>.7</eff></item>"
      "            </efficiencies>"
      "        </info>"
      "    </item>"
      "</streams>"
      ""
      "<leftover_commod>waste</leftover_commod>"
      "<throughput>100</throughput>"
      "<feedbuf_size>100</feedbuf_size>"
      "<feed_commods> <val>feed</val> </feed_commods>"
      "<latitude>10.0</latitude> "
      "<longitude>15.0</longitude> ";
  CompMap m;
  m[id("u235")] = 0.08;
  m[id("u238")] = 0.9;
  m[id("Pu239")] = .01;
  m[id("Pu240")] = .01;
  Composition::Ptr c = Composition::CreateFromMass(m);

  int simdur = 2;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Separations"), config, simdur);
  sim.AddSource("feed").recipe("recipe1").Finalize();
  sim.AddSink("stream1").capacity(100).Finalize();
  sim.AddRecipe("recipe1", c);
  int id = sim.Run();

  QueryResult qr = sim.db().Query("AgentPosition", NULL);
  EXPECT_EQ(qr.GetVal<double>("Latitude"), 10.0);
  EXPECT_EQ(qr.GetVal<double>("Longitude"), 15.0);
 }
} // namespace cycamore

