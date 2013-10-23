// commodity_producer_tests.h
#include <gtest/gtest.h>

#include "commodity_producer.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CommodityProducerTests : public ::testing::Test {
 protected:
  std::string commodity_name_;
  cyclus::Commodity commodity_;
  cyclus::CommodityProducer producer_;
  double capacity_, cost_;

 public:
  virtual void SetUp();
  virtual void TearDown();
  double DefaultCapacity();
  double DefaultCost();
  void AddCommodity();
  void SetCapacity();
  void SetCost();
};
