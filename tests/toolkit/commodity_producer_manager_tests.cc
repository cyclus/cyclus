#include "commodity_producer_manager_tests.h"

#include "error.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityProducerManagerTests::SetUp() {
  helper.SetUpProducers();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityProducerManagerTests::TearDown() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerManagerTests, initialization) {
  EXPECT_EQ(manager.TotalCapacity(helper.commodity), 0.0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerManagerTests, registerunregister) {
  // 1 producer
  EXPECT_NO_THROW(manager.Register(helper.producer1));
  EXPECT_EQ(manager.TotalCapacity(helper.commodity),
            helper.capacity);

  // 2 producers
  EXPECT_NO_THROW(manager.Register(helper.producer2));
  EXPECT_EQ(manager.TotalCapacity(helper.commodity),
            helper.nproducers*helper.capacity);

  // 1 producer
  EXPECT_NO_THROW(manager.Unregister(helper.producer1));
  EXPECT_EQ(manager.TotalCapacity(helper.commodity),
            helper.capacity);

  // 0 producers
  EXPECT_NO_THROW(manager.Unregister(helper.producer2));
  EXPECT_EQ(manager.TotalCapacity(helper.commodity), 0.0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerManagerTests, differentcommodity) {
  EXPECT_NO_THROW(manager.Register(helper.producer1));

  Commodity differentcommodity("differentcommodity");
  EXPECT_EQ(manager.TotalCapacity(differentcommodity), 0.0);
}

}  // namespace toolkit
}  // namespace cyclus
