#include "timeseries.h"

namespace cyclus {
namespace toolkit {

std::map<std::string, std::vector<time_series_listener_t> > TIME_SERIES_LISTENERS;

template <>
void RecordTimeSeries<POWER>(cyclus::Agent* agent, double value) {
  RecordTimeSeries<double>("Power", agent, value);
}

template <>
void RecordTimeSeries<ENRICH_SWU>(cyclus::Agent* agent, double value) {
  RecordTimeSeries<double>("EnrichmentSWU", agent, value);
}

template <>
void RecordTimeSeries<ENRICH_FEED>(cyclus::Agent* agent, double value) {
  RecordTimeSeries<double>("EnrichmentFeed", agent, value);
}

}  // namespace toolkit
}  // namespace cyclus
