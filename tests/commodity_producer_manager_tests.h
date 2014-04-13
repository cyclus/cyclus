// commodity_producer_manager_tests.h
#ifndef CYCLUS_TESTS_COMMODITY_PRODUCER_MANAGER_TESTS_H_
#define CYCLUS_TESTS_COMMODITY_PRODUCER_MANAGER_TESTS_H_

#include <gtest/gtest.h>

#include "commodity_producer_manager.h"
#include "commodity_test_helper.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CommodityProducerManagerTests : public ::testing::Test {
 public:
  cyclus::CommodityProducerManager manager;
  CommodityTestHelper* helper;

 public:
  virtual void SetUp();
  virtual void TearDown();

  void AddCommodity(cyclus::CommodityProducer* producer);
  void SetCapacity(cyclus::CommodityProducer* producer);
  void SetUpProducers();
  void RegisterProducer(cyclus::CommodityProducer* producer);
  void UnRegisterProducer(cyclus::CommodityProducer* producer);
};

#endif  // CYCLUS_TESTS_COMMODITY_PRODUCER_MANAGER_TESTS_H_
