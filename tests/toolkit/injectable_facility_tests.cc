#include "injectable_facility.h"

#include <gtest/gtest.h>

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "infile_tree.h"
#include "mock_sim.h"
#include "pyhooks.h"
#include "sqlite_back.h"
#include "test_context.h"
#include "xml_parser.h"

namespace cyclus {
namespace toolkit {

// Extension points:
// 1. Include a new snippet in injectable_facility.h.
// 2. Add any required lifecycle calls in injectable_facility.cc.
// 3. Add expected names below and feature-specific behavior checks here.
//
// Scalar parameters automatically join the default, explicit-input, clone, and
// database round-trip checks. Add support for new types in Values/ExpectState;
// unsupported types fail explicitly. Explicit-input values test serialization
// and may not form a valid configuration for running a simulation. 
class InjectableFacilityTest : public ::testing::Test {
 protected:
  // Create the test database and context, and reset the snapshot counter.
  InjectableFacilityTest()
      : back_(":memory:"), ctx_(&timer_, &rec_), snapshot_time_(0) {}

  // Start Python, connect the recorder to SQLite, and initialize the context.
  virtual void SetUp() {
    cyclus::PyStart();
    rec_.RegisterBackend(&back_);
    ctx_.InitSim(cyclus::SimInfo(1));
  }

  // Balance the Python initialization performed in SetUp.
  virtual void TearDown() {
    cyclus::PyStop();
  }

  // Load XML configuration through the database into a new facility. The
  // fixture context owns the facility; loading does not call EnterNotify.
  InjectableFacility* Load(std::string config) {
    ctx_.time(0);
    InjectableFacility* a = new InjectableFacility(&ctx_);

    std::stringstream xml;
    xml << "<facility><name>injectable</name>"
        << "<config><InjectableFacility>"
        << config << "</InjectableFacility></config></facility>";
    cyclus::XMLParser parser;
    parser.Init(xml);
    
    // Take the XML above and step through the standard XML --> Database 
    // --> agent path we use to build cyclus from XML. 
    cyclus::InfileTree tree(parser);
    a->Agent::InfileToDb(&tree, cyclus::DbInit(a, true));
    a->InfileToDb(&tree, cyclus::DbInit(a));
    rec_.Flush();

    // Build queries to grab information from the database to build the agent
    std::vector<cyclus::Cond> conds;
    conds.push_back(cyclus::Cond("AgentId", "==", a->id()));
    conds.push_back(cyclus::Cond("SimTime", "==", 0));

    // Initialize the actual agent from the database queries
    cyclus::CondInjector filtered(&back_, conds);
    cyclus::PrefixInjector base(&filtered, "AgentState");
    cyclus::PrefixInjector state(&filtered, StateTable(a));
    a->Agent::InitFrom(&base);
    a->InitFrom(&state);

    return a;
  }

  // Configure a facility in MockSim and select it as the agent to run.
  InjectableFacility* Configure(cyclus::MockSim& sim, std::string config) {
    // Load generated configuration through a database, preserving the new
    // agent's simulation context (Agent::InitFrom(copy) copies its context).
    InjectableFacility* prototype = Load(config);
    Snapshot(prototype);

    std::vector<cyclus::Cond> conds = Conditions(prototype);
    
    cyclus::CondInjector filtered(&back_, conds);
    cyclus::PrefixInjector state(&filtered, StateTable(prototype));
    InjectableFacility* a = new InjectableFacility(sim.context());
    a->prototype("injectable");
    a->InitFrom(&state);
    sim.agent = a;
    
    return a;
  }

  // Generate a map of annotated variable names to expected test values.
  // With explicit_values=false, use declared defaults. With explicit_values
  // =true, offset numeric defaults, flip booleans, and choose categorical 
  // options or a placeholder for strings. The tests use these alternate values
  // to generate XML input and check that loading, cloning, and database reload
  // preserve the supplied configuration.
  Json::Value Values(InjectableFacility* a, bool explicit_values) {
    Json::Value vars = a->annotations()["vars"];
    Json::Value values(Json::objectValue);
    std::vector<std::string> names = vars.getMemberNames();

    for (int i = 0; i < names.size(); ++i) {
      const std::string& name = names[i];
      const Json::Value& info = vars[name];
    
      // cycpp can include alias pointers as well as variable descriptions.
      // We need to skip those and only look at the objects themselves.
      if (!info.isObject()) {
        continue;
      }

      // We require that injected variables have a default value. Check that.
      EXPECT_TRUE(info.isMember("default")) << name << " needs a default";
      
      // Either use the default vals, or generate new ones depending on type
      const Json::Value& def = info["default"];
      if (!explicit_values) {
        values[name] = def;
      } else if (info["type"] == "double") {
        values[name] = def.asDouble() + 0.25 + i * 0.01;
      } else if (info["type"] == "int") {
        values[name] = def.asInt() + 2 + i;
      } else if (info["type"] == "bool") {
        values[name] = !def.asBool();
      } else if (info["type"] == "std::string") {
        const Json::Value& choices = info["categorical"];
        values[name] = choices.isArray() && !choices.empty()
                           ? choices[choices.size() - 1]
                           : Json::Value("injected_value");
      } else {
        ADD_FAILURE() << "Add a test value for " << name << ": "
                      << info["type"];
      }
    }

    return values;
  }

  // Turn named scalar values into XML, escaping strings as needed.
  std::string Config(const Json::Value& values) {
    std::stringstream xml;
    xml << std::setprecision(17);

    std::vector<std::string> names = values.getMemberNames();
    for (int i = 0; i < names.size(); ++i) {
      const std::string& name = names[i];
      // Current snippets have scalar, unaliased XML fields.
      xml << "<" << name << ">";
      if (values[name].isString()) {
        const std::string value = values[name].asString();
        for (int j = 0; j < value.size(); ++j) {
          switch (value[j]) {
            case '&': xml << "&amp;"; break;
            case '<': xml << "&lt;"; break;
            case '>': xml << "&gt;"; break;
            default: xml << value[j];
          }
        }
      } else if (values[name].isBool()) {
        xml << (values[name].asBool() ? "true" : "false");
      } else if (values[name].isIntegral()) {
        xml << values[name].asInt();
      } else {
        xml << values[name].asDouble();
      }
      xml << "</" << name << ">";
    }

    return xml.str();
  }

  // Record base and injected state at a new test time, then return the
  // database row containing the injected state.
  cyclus::QueryResult Snapshot(InjectableFacility* a) {
    ctx_.time(++snapshot_time_);
    
    a->Agent::Snapshot(cyclus::DbInit(a, true));
    a->Snapshot(cyclus::DbInit(a));
    rec_.Flush();
    std::vector<cyclus::Cond> conds = Conditions(a);
  
    return back_.Query(StateTable(a) + "Info", &conds);
  }

  // Return the database table prefix Cyclus uses for this archetype's state.
  std::string StateTable(InjectableFacility* a) {
    return "AgentState" + cyclus::AgentSpec(a->spec()).Sanitize();
  }

  // Select this agent's state at the latest snapshot time.
  std::vector<cyclus::Cond> Conditions(InjectableFacility* a) {
    std::vector<cyclus::Cond> conds;
    conds.push_back(cyclus::Cond("AgentId", "==", a->id()));
    conds.push_back(cyclus::Cond("SimTime", "==", snapshot_time_));

    return conds;
  }

  // Compare recorded values with expectations using the annotated types.
  void ExpectState(cyclus::QueryResult state, const Json::Value& values,
                   const Json::Value& vars) {
    ASSERT_EQ(1, state.rows.size());
    std::vector<std::string> names = values.getMemberNames();
    for (int i = 0; i < names.size(); ++i) {
      const std::string& name = names[i];
      SCOPED_TRACE(name); // include the variable name in failure reports

      const Json::Value& type = vars[name]["type"];
      if (type == "double") {
        EXPECT_DOUBLE_EQ(values[name].asDouble(), state.GetVal<double>(name));
      } else if (type == "int") {
        EXPECT_EQ(values[name].asInt(), state.GetVal<int>(name));
      } else if (type == "bool") {
        EXPECT_EQ(values[name].asBool(), state.GetVal<bool>(name));
      } else if (type == "std::string") {
        EXPECT_EQ(values[name].asString(), state.GetVal<std::string>(name));
      } else {
        ADD_FAILURE() << "Add a state comparison for " << type;
      }
    }
  }

  // Read a resource's recorded quantity without rebuilding its composition.
  double Quantity(cyclus::MockSim& sim, int resource_id) {
    std::vector<cyclus::Cond> conds;
    conds.push_back(cyclus::Cond("ResourceId", "==", resource_id));
    cyclus::QueryResult qr = sim.db().Query("Resources", &conds);
    EXPECT_EQ(1, qr.rows.size()); // Very cheap, more tests good

    return qr.GetVal<double>("Quantity");
  }

  // Check for one recorded position matching the expected coordinates.
  void ExpectPosition(cyclus::MockSim& sim, int id, double lat, double lon) {
    std::vector<cyclus::Cond> conds;
    conds.push_back(cyclus::Cond("AgentId", "==", id));
    cyclus::QueryResult qr = sim.db().Query("AgentPosition", &conds);
    ASSERT_EQ(1, qr.rows.size());
    EXPECT_NEAR(lat, qr.GetVal<double>("Latitude"), 1e-6);
    EXPECT_NEAR(lon, qr.GetVal<double>("Longitude"), 1e-6);
  }

  // Keep the recorder and backend alive until context cleanup finishes.
  cyclus::SqliteBack back_;
  cyclus::Recorder rec_;
  cyclus::Timer timer_;
  cyclus::FakeContext ctx_;
  int snapshot_time_;
};

TEST_F(InjectableFacilityTest, ExpectedVariables) {
  InjectableFacility* a = Load("");
  Json::Value vars = a->annotations()["vars"];
  // Independent expectations catch a missing pragma/include that discovery
  // alone would overlook. Extend this list when adding a snippet or variable.
  const char* expected[] = {
      "latitude", "longitude", "unit_cost", "unit_cost_mod", "sell_quantity",
      "active_buying_frequency_type", "active_buying_val", "active_buying_min",
      "active_buying_max", "active_buying_mean", "active_buying_stddev",
      "active_buying_end_probability", "active_buying_disruption_probability",
      "active_buying_disruption", "dormant_buying_frequency_type",
      "dormant_buying_val", "dormant_buying_min", "dormant_buying_max",
      "dormant_buying_mean", "dormant_buying_stddev",
      "dormant_buying_end_probability", "dormant_buying_disruption_probability",
      "dormant_buying_disruption", "buying_size_type", "buying_size_val",
      "buying_size_min", "buying_size_max", "buying_size_mean",
      "buying_size_stddev", "reorder_point", "reorder_quantity",
      "cumulative_cap"};
  for (int i = 0; i < sizeof(expected) / sizeof(expected[0]); ++i) {
    EXPECT_TRUE(vars[expected[i]].isObject()) << expected[i];
  }
}

TEST_F(InjectableFacilityTest, Defaults) {
  InjectableFacility* a = Load("");
  ExpectState(Snapshot(a), Values(a, false), a->annotations()["vars"]);
}

TEST_F(InjectableFacilityTest, ExplicitInput) {
  InjectableFacility* defaults = Load("");
  Json::Value values = Values(defaults, true);
  InjectableFacility* a = Load(Config(values));
  ExpectState(Snapshot(a), values, a->annotations()["vars"]);
}

TEST_F(InjectableFacilityTest, Clone) {
  InjectableFacility* defaults = Load("");
  Json::Value values = Values(defaults, true);
  InjectableFacility* a = Load(Config(values));
  InjectableFacility* clone = dynamic_cast<InjectableFacility*>(a->Clone());
  ASSERT_NE(nullptr, clone);
  ExpectState(Snapshot(clone), values, a->annotations()["vars"]);
}

// This test makes sure that we can save these to a database and then restore
// like if we were to use the restart feature.
TEST_F(InjectableFacilityTest, DatabaseRoundTrip) {
  InjectableFacility* defaults = Load("");
  Json::Value values = Values(defaults, true);
  InjectableFacility* a = Load(Config(values));
  ExpectState(Snapshot(a), values, a->annotations()["vars"]);
  std::vector<cyclus::Cond> conds = Conditions(a);
  cyclus::CondInjector filtered(&back_, conds);
  cyclus::PrefixInjector base(&filtered, "AgentState");
  cyclus::PrefixInjector state(&filtered, StateTable(a));
  InjectableFacility* restored = new InjectableFacility(&ctx_);
  restored->Agent::InitFrom(&base);
  restored->InitFrom(&state);
  ExpectState(Snapshot(restored), values, restored->annotations()["vars"]);
}

TEST_F(InjectableFacilityTest, Schema) {
  InjectableFacility* a = Load("");
  std::stringstream schema;
  schema << "<grammar xmlns:a=\'http://relaxng.org/ns/annotation/1.0\' "
         << "datatypeLibrary=\'http://www.w3.org/2001/XMLSchema-datatypes\' "
         << "xmlns='http://relaxng.org/ns/structure/1.0'>"
         << "<start><element name='InjectableFacility'>" << a->schema()
         << "</element></start></grammar>";
  cyclus::XMLParser parser;
  parser.Init("<InjectableFacility/>");
  EXPECT_NO_THROW(parser.Validate(schema));
  parser.Init("<InjectableFacility>" + Config(Values(a, true)) +
              "</InjectableFacility>");
  EXPECT_NO_THROW(parser.Validate(schema));
}

TEST_F(InjectableFacilityTest, RunsWithDefaults) {
  cyclus::MockSim sim(2);
  Configure(sim, "");
  int id = sim.Run();
  ExpectPosition(sim, id, 0, 0);
}

TEST_F(InjectableFacilityTest, ConfiguredRun) {
  cyclus::MockSim sim(5);
  std::stringstream xml;
  xml << "<latitude>41.25</latitude><longitude>-87.5</longitude>"
      << "<active_buying_val>2</active_buying_val>"
      << "<dormant_buying_val>1</dormant_buying_val>"
      << "<buying_size_val>0.5</buying_size_val>"
      << "<sell_quantity>3</sell_quantity>"
      << "<unit_cost>2.5</unit_cost><unit_cost_mod>-0.25</unit_cost_mod>";
  Configure(sim, xml.str());
  sim.AddSource("injectable_in").Finalize();
  sim.AddSink("injectable_out").Finalize();
  int id = sim.Run();
  ExpectPosition(sim, id, 41.25, -87.5);

  std::vector<cyclus::Cond> conds;
  conds.push_back(cyclus::Cond("ReceiverId", "==", id));
  cyclus::QueryResult incoming = sim.db().Query("Transactions", &conds);
  ASSERT_EQ(4, incoming.rows.size());
  const int expected_times[] = {0, 1, 3, 4};
  for (int i = 0; i < incoming.rows.size(); ++i) {
    EXPECT_EQ(expected_times[i], incoming.GetVal<int>("Time", i));
    EXPECT_DOUBLE_EQ(-0.25, incoming.GetVal<double>("UnitCostMod", i));
    EXPECT_DOUBLE_EQ(2, Quantity(sim, incoming.GetVal<int>("ResourceId", i)));
  }

  conds.clear();
  conds.push_back(cyclus::Cond("SenderId", "==", id));
  cyclus::QueryResult outgoing = sim.db().Query("Transactions", &conds);
  ASSERT_FALSE(outgoing.rows.empty());
  for (int i = 0; i < outgoing.rows.size(); ++i) {
    EXPECT_DOUBLE_EQ(3, Quantity(sim, outgoing.GetVal<int>("ResourceId", i)));
    EXPECT_DOUBLE_EQ(incoming.GetVal<double>("UnitCost", 0) + 2.5,
                     outgoing.GetVal<double>("UnitCost", i));
  }
}

TEST_F(InjectableFacilityTest, InvalidBuyPolicyParameters) {
  InjectableFacility* a = Load("<active_buying_min>3</active_buying_min>"
                             "<active_buying_max>2</active_buying_max>");
  // Build invokes EnterNotify with normally initialized input state.
  EXPECT_THROW(a->Build(nullptr), cyclus::ValueError);
}

}  // namespace toolkit
}  // namespace cyclus
