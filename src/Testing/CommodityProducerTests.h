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
  double capacity_, cost_;

 public:
  virtual void SetUp();
  virtual void TearDown();
  double defaultCapacity();
  double defaultCost();
  void addCommodity();
  void setCapacity();
  void setCost();
};
