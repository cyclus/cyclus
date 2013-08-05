// CommodityProducerManagerTests.h
#include <gtest/gtest.h>
#include "CommodityTestHelper.h"

#include "CommodityProducerManager.h"

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
