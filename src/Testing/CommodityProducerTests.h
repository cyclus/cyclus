// CommodityProducerTests.h
#include <gtest/gtest.h>

#include "CommodityProducer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CommodityProducerTests : public ::testing::Test {
 protected:
  string commodity_name_;
  Commodity commodity_;
  SupplyDemand::CommodityProducer producer_;
  double capacity_;

 public:
  virtual void SetUp();
  virtual void TearDown();
  double defaultCapacity();
};
