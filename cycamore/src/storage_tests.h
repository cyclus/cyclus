#ifndef STORAGE_TESTS_H_
#define STORAGE_TESTS_H_

#include <gtest/gtest.h>

#include "storage.h"

#include "context.h"
#include "facility_tests.h"
#include "agent_tests.h"

namespace storage {
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StorageTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  Storage* src_facility_;

  virtual void SetUp();
  virtual void TearDown();
  void InitParameters();
  void SetUpStorage();
  void TestInitState(storage::Storage* fac);
  void TestAddMat(storage::Storage* fac,
      cyclus::Material::Ptr mat);
  void TestBuffers(storage::Storage* fac, double inv, double
      proc, double ready, double stocks);
  void TestStocks(storage::Storage* fac, cyclus::CompMap v);
  void TestReadyTime(storage::Storage* fac, int t);
  void TestCurrentCap(storage::Storage* fac, double inv);

  std::vector<std::string> in_c1, out_c1;
  std::string in_r1;

  int residence_time;
  double throughput, max_inv_size;
  bool discrete_handling;
};
} // namespace storage
#endif // STORAGE_TESTS_H_

