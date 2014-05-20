// commodity_producer_manager_tests.h
#ifndef CYCLUS_TESTS_COMMODITY_PRODUCER_MANAGER_TESTS_H_
#define CYCLUS_TESTS_COMMODITY_PRODUCER_MANAGER_TESTS_H_

#include <gtest/gtest.h>

#include "toolkit/commodity_producer_manager.h"
#include "commodity_test_helper.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CommodityProducerManagerTests : public ::testing::Test {
 public:
  CommodityProducerManager manager;
  CommodityTestHelper helper;

 public:
  virtual void SetUp();
  virtual void TearDown();
};

} // namespace toolkit
} // namespace cyclus

#endif  // CYCLUS_TESTS_COMMODITY_PRODUCER_MANAGER_TESTS_H_
