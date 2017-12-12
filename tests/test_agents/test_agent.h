#ifndef CYCLUS_TESTS_TEST_AGENTS_TEST_AGENT_H_
#define CYCLUS_TESTS_TEST_AGENTS_TEST_AGENT_H_

#include "cyclus.h"

namespace cyclus {

/// This is the simplest possible Agent, for testing
class TestAgent: public Agent {
 public:
  static std::string proto_name() { return "test_agent_prototype"; }
  static std::string spec() { return "test_agent_impl"; }

  TestAgent(Context* ctx) : Agent(ctx) {
    Agent::prototype(proto_name());
    Agent::spec(spec());
  }
  virtual ~TestAgent() {}
  
  virtual void Snapshot(DbInit di) {}
  virtual Agent* Clone() { return new TestAgent(context()); }
  virtual void InitInv(Inventories& inv) {}
  virtual Inventories SnapshotInv() { return Inventories(); }

  void Tick() {}
  void Tock() {}
};

} // namespace cyclus
  
#endif  // CYCLUS_TESTS_TEST_AGENTS_TEST_AGENT_H_
