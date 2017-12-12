#include "supply_demand_manager.h"

#include "error.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double SupplyDemandManager::Supply(Commodity& commodity) {
  double value = 0.0;
  std::set<CommodityProducerManager*>::iterator it;
  for (it = managers_.begin(); it != managers_.end(); it++) {
    value += (*it)->TotalCapacity(commodity);
  }
  return value;
}

}  // namespace toolkit
}  // namespace cyclus
