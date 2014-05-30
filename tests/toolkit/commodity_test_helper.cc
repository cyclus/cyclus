#include "commodity_test_helper.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CommodityTestHelper::CommodityTestHelper()
    : commodity_name("commod"),
      commodity(commodity_name),
      capacity(5.0),
      nproducers(2) {
  producer1 = new CommodityProducer();
  producer2 = new CommodityProducer();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CommodityTestHelper::~CommodityTestHelper() {
  delete producer1;
  delete producer2;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityTestHelper::SetUpProducerManager() {
  SetUpProducers();
  manager.Register(producer1);
  manager.Register(producer2);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityTestHelper::SetUpProducers() {
  producer1->Add(commodity);
  producer1->SetCapacity(commodity, capacity);
  producer2->Add(commodity);
  producer2->SetCapacity(commodity, capacity);
}

} // namespace toolkit
} // namespace cyclus
