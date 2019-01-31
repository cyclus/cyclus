#include "timeseries.h"

namespace cyclus {
namespace toolkit {

std::map<std::string, std::vector<time_series_listener_t> > TIME_SERIES_LISTENERS;

template <>
void RecordTimeSeries<POWER>(cyclus::Agent* agent, double value, std::string unit) {
  RecordTimeSeries<double>("Power", agent, value, unit);
}

template <>
void RecordTimeSeries<ENRICH_SWU>(cyclus::Agent* agent, double value, std::string unit) {
  RecordTimeSeries<double>("EnrichmentSWU", agent, value, unit);
}

template <>
void RecordTimeSeries<ENRICH_FEED>(cyclus::Agent* agent, double value, std::string unit) {
  RecordTimeSeries<double>("EnrichmentFeed", agent, value, unit);
}

}  // namespace toolkit
}  // namespace cyclus
