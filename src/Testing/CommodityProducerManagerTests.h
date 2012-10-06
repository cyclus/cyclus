// CommodityProducerManagerTests.h
#include <gtest/gtest.h>
#include "CommodityTestHelper.h"

#include "CommodityProducerManager.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CommodityProducerManagerTests : public ::testing::Test {
 public:
  SupplyDemand::CommodityProducerManager manager;
  CommodityTestHelper* helper;

 public:
  virtual void SetUp();
  virtual void TearDown();

  void addCommodity(SupplyDemand::CommodityProducer* producer);
  void setCapacity(SupplyDemand::CommodityProducer* producer);
  void setUpProducers();
  void registerProducer(SupplyDemand::CommodityProducer* producer);
  void unRegisterProducer(SupplyDemand::CommodityProducer* producer);
};
