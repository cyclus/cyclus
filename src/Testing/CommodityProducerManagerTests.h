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

  void addCommodity(cyclus::supply_demand::CommodityProducer* producer);
  void setCapacity(cyclus::supply_demand::CommodityProducer* producer);
  void setUpProducers();
  void registerProducer(cyclus::supply_demand::CommodityProducer* producer);
  void unRegisterProducer(cyclus::supply_demand::CommodityProducer* producer);
};
