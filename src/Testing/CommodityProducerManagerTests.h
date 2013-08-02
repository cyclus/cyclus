// CommodityProducerManagerTests.h
#include <gtest/gtest.h>
#include "CommodityTestHelper.h"

#include "CommodityProducerManager.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CommodityProducerManagerTests : public ::testing::Test {
 public:
  cyclus::SupplyDemand::CommodityProducerManager manager;
  CommodityTestHelper* helper;

 public:
  virtual void SetUp();
  virtual void TearDown();

  void addCommodity(cyclus::SupplyDemand::CommodityProducer* producer);
  void setCapacity(cyclus::SupplyDemand::CommodityProducer* producer);
  void setUpProducers();
  void registerProducer(cyclus::SupplyDemand::CommodityProducer* producer);
  void unRegisterProducer(cyclus::SupplyDemand::CommodityProducer* producer);
};
