#ifndef CYCLUS_TESTS_TOOLKIT_COMMODITY_PRODUCER_TESTS_H_
#define CYCLUS_TESTS_TOOLKIT_COMMODITY_PRODUCER_TESTS_H_

#include <string>

#include <gtest/gtest.h>

#include "toolkit/commodity_producer.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CommodityProducerTests : public ::testing::Test {
 public:
  virtual void SetUp();
  virtual void TearDown();

 protected:
  std::string commodity_name_;
  Commodity commodity_;
  CommodityProducer producer_;
  double capacity_, cost_;
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_TESTS_TOOLKIT_COMMODITY_PRODUCER_TESTS_H_
