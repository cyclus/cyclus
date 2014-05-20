#include "commodity_producer_manager.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double CommodityProducerManager::TotalCapacity(Commodity& commodity) {
  double total = 0.0;
  std::set<CommodityProducer*>::iterator it;
  CommodityProducer* p;
  for (it = producers_.begin(); it != producers_.end(); ++it) {
    p = *it;
    if (p->Produces(commodity))
      total += p->Capacity(commodity);
  }
  return total;
}

} // namespace toolkit
} // namespace cyclus

