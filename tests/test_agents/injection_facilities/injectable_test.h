#ifndef CYCLUS_TESTS_TEST_AGENTS_INJECTION_FACILITIES_INJECTABLE_TEST_H_
#define CYCLUS_TESTS_TEST_AGENTS_INJECTION_FACILITIES_INJECTABLE_TEST_H_

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include "infile_tree.h"
#include "pyhooks.h"
#include "sqlite_back.h"
#include "test_context.h"
#include "xml_parser.h"

namespace cyclus {
namespace toolkit {

/// Shared XML, database, and persistence support for focused Cycpp host tests.
/// Injectable-specific configuration and assertions belong in derived fixtures.
template <class FacilityT>
class InjectableTest : public ::testing::Test {
 protected:
  InjectableTest()
      : back_(":memory:"), ctx_(&timer_, &rec_), snapshot_time_(0) {}

  virtual void SetUp() {
    PyStart();
    rec_.RegisterBackend(&back_);
    ctx_.InitSim(SimInfo(1));
  }

  virtual void TearDown() { PyStop(); }

  FacilityT* Load(const std::string& archetype, const std::string& config) {
    ctx_.time(0);
    FacilityT* facility = new FacilityT(&ctx_);

    std::stringstream xml;
    xml << "<facility><name>injectable-test</name><config><" << archetype
        << ">" << config << "</" << archetype
        << "></config></facility>";
    XMLParser parser;
    parser.Init(xml);
    InfileTree tree(parser);
    facility->Agent::InfileToDb(&tree, DbInit(facility, true));
    facility->InfileToDb(&tree, DbInit(facility));
    rec_.Flush();

    std::vector<Cond> conds;
    conds.push_back(Cond("AgentId", "==", facility->id()));
    conds.push_back(Cond("SimTime", "==", 0));
    CondInjector filtered(&back_, conds);
    PrefixInjector base(&filtered, "AgentState");
    PrefixInjector state(&filtered, StateTable(facility));
    facility->Agent::InitFrom(&base);
    facility->InitFrom(&state);
    return facility;
  }

  QueryResult Snapshot(FacilityT* facility) {
    ctx_.time(++snapshot_time_);
    facility->Agent::Snapshot(DbInit(facility, true));
    facility->Snapshot(DbInit(facility));
    rec_.Flush();
    std::vector<Cond> conds = Conditions(facility);
    return back_.Query(StateTable(facility) + "Info", &conds);
  }

  FacilityT* Restore(FacilityT* facility) {
    std::vector<Cond> conds = Conditions(facility);
    CondInjector filtered(&back_, conds);
    PrefixInjector base(&filtered, "AgentState");
    PrefixInjector state(&filtered, StateTable(facility));
    FacilityT* restored = new FacilityT(&ctx_);
    restored->Agent::InitFrom(&base);
    restored->InitFrom(&state);
    return restored;
  }

  std::string StateTable(Agent* facility) const {
    return "AgentState" + AgentSpec(facility->spec()).Sanitize();
  }

  std::vector<Cond> Conditions(Agent* facility) const {
    std::vector<Cond> conds;
    conds.push_back(Cond("AgentId", "==", facility->id()));
    conds.push_back(Cond("SimTime", "==", snapshot_time_));
    return conds;
  }

  SqliteBack back_;
  Recorder rec_;
  Timer timer_;
  FakeContext ctx_;
  int snapshot_time_;
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_TESTS_TEST_AGENTS_INJECTION_FACILITIES_INJECTABLE_TEST_H_
