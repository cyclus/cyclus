// commodity_producer_manager_tests.h
#include <gtest/gtest.h>
#include "commodity_test_helper.h"

#include "commodity_producer_manager.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
