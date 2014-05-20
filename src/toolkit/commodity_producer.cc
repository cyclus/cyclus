#include "commodity_producer.h"

namespace cyclus {
namespace toolkit {

CommodInfo::CommodInfo(double default_capacity,
                       double default_cost)
    : capacity(default_capacity),
      cost(default_cost) {}

CommodityProducer::CommodityProducer(double default_capacity,
                                     double default_cost)
    : default_capacity_(default_capacity),
      default_cost_(default_cost) {}

CommodityProducer::~CommodityProducer() {}

std::set<Commodity, CommodityCompare> CommodityProducer::ProducedCommodities() {
  std::set<Commodity, CommodityCompare> commodities;
  std::map<Commodity, CommodInfo, CommodityCompare>::iterator it;
  for (it = commodities_.begin(); it != commodities_.end(); it++) {
    commodities.insert(it->first);
  }
  return commodities;
}

void CommodityProducer::Copy(CommodityProducer* source) {
  std::set<Commodity, CommodityCompare> commodities =
      source->ProducedCommodities();
  std::set<Commodity, CommodityCompare>::iterator it;
  for (it = commodities.begin(); it != commodities.end(); it++) {
    Add(*it);
    SetCapacity(*it, source->Capacity(*it));
    SetCost(*it, source->Cost(*it));
  }
}

} // namespace toolkit
} // namespace cyclus
