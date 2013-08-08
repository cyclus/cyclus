// commodity_producer_manager_tests.h
#include <gtest/gtest.h>
#include "commodity_test_helper.h"

#include "commodity_producer_manager.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CommodityProducerManagerTests : public ::testing::Test {
 public:
  cyclus::supply_demand::CommodityProducerManager manager;
  CommodityTestHelper* helper;

 public:
  virtual void SetUp();
  virtual void TearDown();

  void AddCommodity(cyclus::supply_demand::CommodityProducer* producer);
  void SetCapacity(cyclus::supply_demand::CommodityProducer* producer);
  void SetUpProducers();
  void RegisterProducer(cyclus::supply_demand::CommodityProducer* producer);
  void UnRegisterProducer(cyclus::supply_demand::CommodityProducer* producer);
};
