#include "injectable_facility.h"

#include <gtest/gtest.h>

#include <iomanip>
#include <list>
#include <map>
#include <set>
#include <stdexcept>
#include <type_traits>
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

namespace {

// Extend this list when adding a snippet or variable.
const char* const kExpectedVariables[] = {
    "latitude", "longitude", "unit_cost", "unit_cost_mod", "unit_cost_mods",
    "sell_quantity",
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

// To make these tests as general as possible, we use the list of backend
// types provided by share/dbtypes.json and some helper functions and templates
// defined below. Some comments have been added to help guide readers through
// those functions and templates, since they all look a little "samey".

// 1. Value conversions (TestValue and SequenceValue).
// Read means JSON -> C++; Write means C++ -> JSON. Container rules call the
// same conversions for their children, so nested types reuse these rules.
// JSON is the common representation for test values. Sequences use arrays;
// pairs use two-element arrays; maps use arrays of [key, value] entries so
// non-string keys and nested containers retain their types.
// 1a. Fallback and scalar rules. The primary template reports unsupported
// types; the specializations below handle arithmetic values and strings.
template<class T, class Enable = void>
struct TestValue {
  static T Read(const Json::Value&) {
    throw std::runtime_error("Add test conversion rules for this leaf type");
  }
  static Json::Value Write(const T&) {
    throw std::runtime_error("Add test conversion rules for this leaf type");
  }
};

template<class T>
struct TestValue<T, typename std::enable_if<std::is_arithmetic<T>::value>::type> {
  static T Read(const Json::Value& v) {
    return static_cast<T>(v.asDouble());
  }
  static Json::Value Write(T v) { return Json::Value(v); }
};

template<>
struct TestValue<std::string> {
  static std::string Read(const Json::Value& v) { return v.asString(); }
  static Json::Value Write(const std::string& v) { return v; }
};

// 1b. Shared sequence rule: convert one element at a time. This supplies the
// implementation used by vectors, lists, and sets immediately below.
template<class C>
struct SequenceValue {
  typedef typename C::value_type Element;
  static C Read(const Json::Value& v) {
    if (!v.isArray()) throw std::runtime_error("Expected a sequence array");
    C result;
    for (Json::ArrayIndex i = 0; i < v.size(); ++i)
      result.insert(result.end(), TestValue<Element>::Read(v[i]));
    return result;
  }
  static Json::Value Write(const C& v) {
    Json::Value result(Json::arrayValue);
    for (typename C::const_iterator it = v.begin(); it != v.end(); ++it)
      result.append(TestValue<Element>::Write(*it));
    return result;
  }
};

// These three specializations select the shared sequence implementation for
// each container family; they do not duplicate its conversion logic.
template<class T>
struct TestValue<std::vector<T> > : SequenceValue<std::vector<T> > {};
template<class T>
struct TestValue<std::list<T> > : SequenceValue<std::list<T> > {};
template<class T>
struct TestValue<std::set<T> > : SequenceValue<std::set<T> > {};

// 1c. Pair rule: convert the two members using their own type rules. Maps
// below also reuse this rule to represent each key/value entry.
template<class K, class V>
struct TestValue<std::pair<K, V> > {
  static std::pair<K, V> Read(const Json::Value& v) {
    if (!v.isArray() || v.size() != 2)
      throw std::runtime_error("Expected a two-element pair array");
    return std::make_pair(TestValue<K>::Read(v[0]), TestValue<V>::Read(v[1]));
  }
  static Json::Value Write(const std::pair<K, V>& v) {
    Json::Value result(Json::arrayValue);
    result.append(TestValue<K>::Write(v.first));
    result.append(TestValue<V>::Write(v.second));
    return result;
  }
};

// 1d. Map rule: accept dictionary defaults or arrays of key/value entries,
// then write the common entry-array representation used by the other helpers.
template<class K, class V>
struct TestValue<std::map<K, V> > {
  static std::map<K, V> Read(const Json::Value& v) {
    std::map<K, V> result;
    if (v.isObject()) {
      // cycpp annotations encode dictionary defaults as JSON objects.
      std::vector<std::string> keys = v.getMemberNames();
      for (size_t i = 0; i < keys.size(); ++i) {
        Json::Value key(keys[i]);
        if (!std::is_same<K, std::string>::value) {
          Json::Reader reader;
          if (!reader.parse(keys[i], key))
            throw std::runtime_error("Cannot decode map default key");
        }
        result[TestValue<K>::Read(key)] = TestValue<V>::Read(v[keys[i]]);
      }
    } else {
      if (!v.isArray()) throw std::runtime_error("Expected map entries");
      for (Json::ArrayIndex i = 0; i < v.size(); ++i) {
        std::pair<K, V> entry = TestValue<std::pair<K, V> >::Read(v[i]);
        result.insert(entry);
      }
    }
    return result;
  }
  static Json::Value Write(const std::map<K, V>& v) {
    Json::Value result(Json::arrayValue);
    for (typename std::map<K, V>::const_iterator it = v.begin();
         it != v.end(); ++it)
      result.append(TestValue<std::pair<K, V> >::Write(*it));
    return result;
  }
};

// 2. Value comparison. Once both sides are JSON, a single recursive check
// handles every supported container and retains floating-point comparisons.
void ExpectValue(const Json::Value& expected, const Json::Value& actual) {
  if (expected.isArray()) {
    ASSERT_TRUE(actual.isArray());
    ASSERT_EQ(expected.size(), actual.size());
    for (Json::ArrayIndex i = 0; i < expected.size(); ++i) {
      SCOPED_TRACE(i);
      ExpectValue(expected[i], actual[i]);
    }
  } else if (expected.isDouble()) {
    EXPECT_DOUBLE_EQ(expected.asDouble(), actual.asDouble());
  } else {
    EXPECT_EQ(expected, actual);
  }
}

// 3. Type registry: bridge runtime annotations and compile-time C++ types.
// The tests discover a variable's type through annotations. TypeOps lets them
// select operations already instantiated for that concrete type by Register<T>.
struct TypeOps {
  Json::Value type;
  Json::Value (*normalize)(const Json::Value&);
  Json::Value (*fetch)(cyclus::QueryResult&, const std::string&);
  void (*record)(cyclus::Recorder&, const Json::Value&);
};

// Typed operations stored in TypeOps:
// - Normalize passes JSON through T so expected values have T's representation
//   (for example, sets become ordered and map defaults become entry arrays).
// - Fetch reads a database value as T and returns comparable JSON.
// - Record writes JSON as T for the standalone helper tests below.
template<class T>
Json::Value Normalize(const Json::Value& value) {
  return TestValue<T>::Write(TestValue<T>::Read(value));
}

template<class T>
Json::Value Fetch(cyclus::QueryResult& state, const std::string& name) {
  return TestValue<T>::Write(state.GetVal<T>(name));
}

template<class T>
void Record(cyclus::Recorder& recorder, const Json::Value& value) {
  recorder.NewDatum("GenericTypeState")
      ->AddVal("value", TestValue<T>::Read(value))->Record();
}

// Use structural annotation JSON as the key, matching cycpp's representation.
std::string TypeKey(const Json::Value& type) {
  return Json::FastWriter().write(type);
}

// Bind the operations for one concrete T to its annotation. The generated
// header supplies these calls, so this file needs no list of concrete types.
template<class T>
void Register(std::map<std::string, TypeOps>& registry, const char* annotation) {
  TypeOps ops;
  Json::Reader reader;
  if (!reader.parse(annotation, ops.type))
    throw std::runtime_error("Invalid generated type annotation");
  ops.normalize = &Normalize<T>;
  ops.fetch = &Fetch<T>;
  ops.record = &Record<T>;
  registry[TypeKey(ops.type)] = ops;
}

const std::map<std::string, TypeOps>& Types() {
  static const std::map<std::string, TypeOps> types = [] {
    std::map<std::string, TypeOps> registry;
// CMake generates Register<T>(...) calls here from share/dbtypes.json.
#include "injectable_dbtypes.h"
    return registry;
  }();
  return types;
}

// This lookup is the fixture's entry point into the generated registry.
const TypeOps& Operations(const Json::Value& type) {
  const auto& types = Types();
  auto it = types.find(TypeKey(type));
  if (it == types.end())
    throw std::runtime_error("Type is absent from current SQLite dbtypes: " +
                             TypeKey(type));
  return it->second;
}

// 4. Sample generation. Walk the annotation type tree to build explicit
// input, filling containers even when their declared defaults are empty.
// Shape metadata controls fixed lengths; otherwise containers get two entries.
// ShapeRank keeps nested children aligned with cycpp's shape dimensions.
int ShapeRank(const Json::Value& type) {
  if (type.isString()) return type == "std::string" ? 1 : 0;
  int rank = type[0] == "std::pair" ? 0 : 1;
  for (Json::ArrayIndex i = 1; i < type.size(); ++i)
    rank += ShapeRank(type[i]);
  return rank;
}

Json::Value Sample(const Json::Value& type, const Json::Value& def, int seed,
                   const Json::Value& shape, int dimension = 0) {
  if (type.isString()) {
    if (type == "double" || type == "float")
      return def.isNumeric() ? def.asDouble() + 0.25 + seed * 0.01
                             : 0.25 + seed;
    if (type == "int") return def.isInt() ? def.asInt() + 2 + seed : 2 + seed;
    if (type == "bool") return def.isBool() ? !def.asBool() : seed % 2 == 0;
    if (type == "std::string") {
      std::string value = std::to_string(seed) + "_injected";
      if (dimension < shape.size() && shape[dimension].asInt() > 0)
        value.resize(shape[dimension].asInt(), 'x');
      return value;
    }
    throw std::runtime_error("Add sample rules for leaf type " + TypeKey(type));
  }
  const std::string family = type[0].asString();
  Json::Value result(Json::arrayValue);
  if (family == "std::pair") {
    result.append(Sample(type[1], Json::Value(), seed, shape, dimension));
    result.append(Sample(type[2], Json::Value(), seed + 1, shape,
                         dimension + ShapeRank(type[1])));
    return result;
  }
  if (family != "std::vector" && family != "std::list" &&
      family != "std::set" && family != "std::map")
    throw std::runtime_error("Add sample rules for container " + family);
  int count = dimension < shape.size() && shape[dimension].asInt() > 0
                  ? shape[dimension].asInt() : 2;
  for (int i = 0; i < count; ++i) {
    Json::Value child = Sample(type[1], Json::Value(), seed + i, shape,
                               dimension + 1);
    if (family == "std::map") {
      Json::Value entry(Json::arrayValue);
      entry.append(child);
      entry.append(Sample(type[2], Json::Value(), seed + i + 1, shape,
                           dimension + 1 + ShapeRank(type[1])));
      result.append(entry);
    } else {
      result.append(child);
    }
  }
  return result;
}

// 5. XML generation. Walk the value and alias trees together so nested
// containers use the element names expected by cycpp's input reader.
std::string XmlName(const Json::Value& alias) {
  if (alias.isString()) return alias.asString();
  return XmlName(alias[0]);
}

// Follow cycpp's nested alias tree, including map item/key/value names.
void WriteXml(std::ostream& xml, const Json::Value& value,
              const Json::Value& type, const Json::Value& alias) {
  const std::string name = XmlName(alias);
  xml << "<" << name << ">";
  if (type.isString()) {
    if (value.isString()) {
      const std::string text = value.asString();
      for (size_t i = 0; i < text.size(); ++i) {
        switch (text[i]) {
          case '&': xml << "&amp;"; break;
          case '<': xml << "&lt;"; break;
          case '>': xml << "&gt;"; break;
          default: xml << text[i];
        }
      }
    } else if (value.isBool()) {
      xml << (value.asBool() ? "true" : "false");
    } else if (value.isIntegral()) {
      xml << value.asInt();
    } else {
      xml << value.asDouble();
    }
  } else if (type[0] == "std::pair") {
    WriteXml(xml, value[0], type[1], alias[1]);
    WriteXml(xml, value[1], type[2], alias[2]);
  } else {
    for (Json::ArrayIndex i = 0; i < value.size(); ++i) {
      if (type[0] == "std::map") {
        const std::string item = alias[0][1].asString();
        xml << "<" << item << ">";
        WriteXml(xml, value[i][0], type[1], alias[1]);
        WriteXml(xml, value[i][1], type[2], alias[2]);
        xml << "</" << item << ">";
      } else {
        WriteXml(xml, value[i], type[1], alias[1]);
      }
    }
  }
  xml << "</" << name << ">";
}

}  // namespace

// 6. Facility fixture: assemble the helpers into the test workflow.
// Values -> Config -> Load -> Snapshot -> ExpectState:
// choose expected values, generate XML, load a facility, save its state, and
// compare that state with expectations. Clone/restart tests add their own
// copy/reload step before checking state again. Defaults use Load("") directly.
//
// Extension points: include snippets and lifecycle calls in the test agent,
// then add expected names and behavior checks below. Concrete SQLite types are
// discovered from dbtypes.json at build time. New combinations of supported
// containers work automatically; new leaf/container families need rules above.
// Explicit-input values test serialization, not simulation configuration validity.
class InjectableFacilityTest : public ::testing::Test {
 protected:
  // Fixture setup and loading: own the test environment and construct agents.

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

  // Test input: choose expected values and serialize explicit configuration.

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
      
      SCOPED_TRACE(name);
      const TypeOps& ops = Operations(info["type"]);
      Json::Value value = explicit_values
          ? Sample(info["type"], info["default"], i, info["shape"])
          : info["default"];
      const Json::Value& choices = info["categorical"];
      if (explicit_values && info["type"] == "std::string" &&
          choices.isArray() && !choices.empty())
        value = choices[choices.size() - 1];
      values[name] = ops.normalize(value);
    }

    return values;
  }

  // Serialize normalized test values using the same aliases as cycpp.
  std::string Config(const Json::Value& values, const Json::Value& vars) {
    std::stringstream xml;
    xml << std::setprecision(17);
    std::vector<std::string> names = values.getMemberNames();
    for (size_t i = 0; i < names.size(); ++i) {
      const std::string& name = names[i];
      const Json::Value& info = vars[name];
      WriteXml(xml, values[name], info["type"],
               info.isMember("alias") ? info["alias"] : Json::Value(name));
    }
    return xml.str();
  }

  // State verification: capture database state and compare it with expectations.

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

      const TypeOps& ops = Operations(vars[name]["type"]);
      ExpectValue(ops.normalize(values[name]), ops.fetch(state, name));
    }
  }

  // Simulation behavior helpers: inspect transactions and recorded positions.

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

  // 7. Included getter/setter helpers to pass friendship along to the TESTS
  void SetUnitCost(InjectableFacility* a, double value) {
    a->set_unit_cost(value);
  }

  double GetUnitCost(InjectableFacility* a) {
    return a->get_unit_cost();
  }

  void SetUnitCostMod(InjectableFacility* a, double value) {
    a->set_unit_cost_mod(value);
  }

  double GetUnitCostMod(InjectableFacility* a) {
    return a->get_unit_cost_mod();
  }

  void SetUnitCostMods(InjectableFacility* a, std::vector<double> value) {
    a->set_unit_cost_mods(value);
  }

  std::vector<double> GetUnitCostMods(InjectableFacility* a) {
    return a->get_unit_cost_mods();
  }

  // Keep the recorder and backend alive until context cleanup finishes.
  cyclus::SqliteBack back_;
  cyclus::Recorder rec_;
  cyclus::Timer timer_;
  cyclus::FakeContext ctx_;
  int snapshot_time_;
};



// 8. Helper tests: check generic type handling independently of the facility.

// Exercise the generated dispatch and recursive conversions independently of
// which types happen to be present in the currently included snippets.
TEST(InjectableTypes, PopulatedDatabaseValues) {
  for (const auto& entry : Types()) {
    const TypeOps& ops = entry.second;
    SCOPED_TRACE(entry.first);
    // These backend-only leaves have no test XML/value rules. A facility that
    // introduces one still fails explicitly in Values(), rather than skipping it.
    if (ops.type == "cyclus::Blob" || ops.type == "boost::uuids::uuid") {
      EXPECT_THROW(Sample(ops.type, Json::Value(), 0, Json::Value()),
                   std::runtime_error);
      continue;
    }
    Json::Value value = ops.normalize(
        Sample(ops.type, Json::Value(), 0, Json::Value()));
    cyclus::SqliteBack backend(":memory:");
    cyclus::Recorder recorder;
    recorder.RegisterBackend(&backend);
    ops.record(recorder, value);
    recorder.Flush();
    cyclus::QueryResult state = backend.Query("GenericTypeState", nullptr);
    ASSERT_EQ(1, state.rows.size());
    ExpectValue(value, ops.fetch(state, "value"));
  }
}

TEST(InjectableTypes, NestedAliasesAndEscaping) {
  Json::Reader reader;
  Json::Value type, alias, value;
  ASSERT_TRUE(reader.parse(
      R"json(["std::map", "std::string", ["std::pair", "double",
              ["std::vector", "std::string"]]])json", type));
  ASSERT_TRUE(reader.parse(
      R"json([["settings", "entry"], "label",
              ["details", "cost", ["names", "name"]]])json", alias));
  ASSERT_TRUE(reader.parse(R"json([["a&b", [1.25, ["x<y", "z"]]]])json", value));
  std::stringstream xml;
  WriteXml(xml, value, type, alias);
  EXPECT_EQ("<settings><entry><label>a&amp;b</label><details><cost>1.25</cost>"
            "<names><name>x&lt;y</name><name>z</name></names></details>"
            "</entry></settings>", xml.str());
  cyclus::XMLParser parser;
  EXPECT_NO_THROW(parser.Init(xml));
}

TEST(InjectableTypes, ShapesAndDictionaryDefaults) {
  Json::Reader reader;
  Json::Value type, shape, defaults;
  ASSERT_TRUE(reader.parse(R"json(["std::vector", "std::string"])json", type));
  ASSERT_TRUE(reader.parse("[3, 5]", shape));
  Json::Value sample = Sample(type, Json::Value(), 0, shape);
  ASSERT_EQ(3, sample.size());
  for (Json::ArrayIndex i = 0; i < sample.size(); ++i)
    EXPECT_EQ(5, sample[i].asString().size());
  ASSERT_TRUE(reader.parse(R"json(["std::map", "int", "double"])json", type));
  ASSERT_TRUE(reader.parse(R"json({"2": 1.25, "3": -0.5})json", defaults));
  Json::Value entries = Operations(type).normalize(defaults);
  ASSERT_EQ(2, entries.size());
  EXPECT_EQ(2, entries[0][0].asInt());
  EXPECT_DOUBLE_EQ(1.25, entries[0][1].asDouble());
  EXPECT_EQ(3, entries[1][0].asInt());
  EXPECT_DOUBLE_EQ(-0.5, entries[1][1].asDouble());
  EXPECT_THROW(Operations(Json::Value("unknown_type")), std::runtime_error);
}

// 8. Facility state tests: discover variables and check defaults, explicit
// input, cloning, database restoration, and the generated input schema.

TEST_F(InjectableFacilityTest, ExpectedVariables) {
  InjectableFacility* a = Load("");
  Json::Value vars = a->annotations()["vars"];
  for (const char* name : kExpectedVariables) {
    EXPECT_TRUE(vars[name].isObject()) << name;
  }
}

TEST_F(InjectableFacilityTest, Defaults) {
  InjectableFacility* a = Load("");
  ExpectState(Snapshot(a), Values(a, false), a->annotations()["vars"]);
}

TEST_F(InjectableFacilityTest, ExplicitInput) {
  InjectableFacility* defaults = Load("");
  Json::Value values = Values(defaults, true);
  InjectableFacility* a = Load(Config(values, defaults->annotations()["vars"]));
  ExpectState(Snapshot(a), values, a->annotations()["vars"]);
}

TEST_F(InjectableFacilityTest, Clone) {
  InjectableFacility* defaults = Load("");
  Json::Value values = Values(defaults, true);
  InjectableFacility* a = Load(Config(values, defaults->annotations()["vars"]));
  InjectableFacility* clone = dynamic_cast<InjectableFacility*>(a->Clone());
  ASSERT_NE(nullptr, clone);
  ExpectState(Snapshot(clone), values, a->annotations()["vars"]);
}

// This test makes sure that we can save these to a database and then restore
// like if we were to use the restart feature.
TEST_F(InjectableFacilityTest, DatabaseRoundTrip) {
  InjectableFacility* defaults = Load("");
  Json::Value values = Values(defaults, true);
  InjectableFacility* a = Load(Config(values, defaults->annotations()["vars"]));
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
  parser.Init("<InjectableFacility>" + Config(Values(a, true), a->annotations()["vars"]) +
              "</InjectableFacility>");
  EXPECT_NO_THROW(parser.Validate(schema));
}

// 9. Facility behavior tests: run simulations and check lifecycle behavior.

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

TEST_F(InjectableFacilityTest, UnitCostGettersAndSetters) {
  InjectableFacility* a = Load("");

  double fac_cost = 10.0;
  double fac_cost_mod = 1.0;
  std::vector<double> fac_cost_mods = {4.13, 5.14, 19.31};

  SetUnitCost(a, fac_cost);
  EXPECT_EQ(fac_cost, GetUnitCost(a));

  SetUnitCostMod(a, fac_cost_mod);
  EXPECT_EQ(fac_cost_mod, GetUnitCostMod(a));

  SetUnitCostMods(a, fac_cost_mods);
  EXPECT_EQ(fac_cost_mods, GetUnitCostMods(a));
}

}  // namespace toolkit
}  // namespace cyclus
