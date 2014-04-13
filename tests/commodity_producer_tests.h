// commodity_producer_tests.h
#ifndef CYCLUS_TESTS_COMMODITY_PRODUCER_TESTS_H_
#define CYCLUS_TESTS_COMMODITY_PRODUCER_TESTS_H_

#include <string>

#include <gtest/gtest.h>

#include "commodity_producer.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

#endif  // CYCLUS_TESTS_COMMODITY_PRODUCER_TESTS_H_
