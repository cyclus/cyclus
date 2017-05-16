#ifndef CYCLUS_TESTS_TEST_GISFAC_TEST_GISFAC_H_
#define CYCLUS_TESTS_TEST_GISFAC_TEST_GISFAC_H_

#include "cyclus.h"

namespace cyclus {

/// This is the simplest possible GISFac, for testing
class TestGISFac : public GISFac {
 public:
  static std::string proto_name() { return "test_gisfac_prototype"; }
  static std::string spec() { return "test_gisfac_impl"; }

  TestGISFac(Context* ctx) : GISFac(ctx) {
    GISFac::prototype(proto_name());
    GISFac::spec(spec());
  }
  virtual ~TestGISFac() {}

  virtual void Snapshot(DbInit di) {}
  virtual GISFac* Clone() { return new TestGISFac(context()); }
  virtual void InitInv(Inventories& inv) {}
  virtual Inventories SnapshotInv() { return Inventories(); }

  void Tick() {}
  void Tock() {}
};

}  // namespace cyclus

#endif  // CYCLUS_TESTS_TEST_GISFAC_TEST_GISFAC_H_
