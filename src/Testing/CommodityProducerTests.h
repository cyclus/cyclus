// CommodityProducerTests.h
#include <gtest/gtest.h>

#include "CommodityProducer.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CommodityProducerTests : public ::testing::Test {
 protected:
  std::string commodity_name_;
  Commodity commodity_;
  SupplyDemand::CommodityProducer producer_;
  double capacity_;

 public:
  virtual void SetUp();
  virtual void TearDown();
  double defaultCapacity();
  void addCommodity();
  void setCapacity();
};
