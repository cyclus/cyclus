// CommodityProducerManagerTests.h
#include <gtest/gtest.h>

#include "CommodityProducerManager.h"
/* #include "CommodityProducer.h" */
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CommodityProducerManagerTests : public ::testing::Test {
 protected:
  std::string commodity_name_;
  Commodity commodity_;
  SupplyDemand::CommodityProducer* producer1_;
  SupplyDemand::CommodityProducer* producer2_;
  double capacity_;
  int nproducers_;
  SupplyDemand::CommodityProducerManager manager_;

 public:
  virtual void SetUp();
  virtual void TearDown();
  void addCommodity(SupplyDemand::CommodityProducer* producer);
  void setCapacity(SupplyDemand::CommodityProducer* producer);
  void setUpProducers();
  void registerProducer(SupplyDemand::CommodityProducer* producer);
  void unRegisterProducer(SupplyDemand::CommodityProducer* producer);
};
