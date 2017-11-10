#include "timeseries.h"

namespace cyclus {
namespace toolkit {

std::map<TimeSeriesType, std::vector<std::function<void(cyclus::Agent*, int, double)> > > TIME_SERIES_LISTENERS_DOUBLE;

void CallListenersDouble(TimeSeriesType tstype, cyclus::Agent* agent, double value){
  int time = agent->context()->time();
  std::vector<std::function<void(cyclus::Agent*, int, double)> > vec = TIME_SERIES_LISTENERS_DOUBLE[tstype];
  for(auto f=vec.begin(); f != vec.end(); ++f){
     (*f)(agent, time, value);
  }
}

template <>
void RecordTimeSeries<POWER>(cyclus::Agent* agent, double value) {
  RecordTimeSeries<double>("Power", agent, value);
  CallListenersDouble(POWER, agent, value);
}

template <>
void RecordTimeSeries<ENRICH_SWU>(cyclus::Agent* agent, double value) {
  RecordTimeSeries<double>("EnrichmentSWU", agent, value);
  CallListenersDouble(ENRICH_SWU, agent, value);
}

template <>
void RecordTimeSeries<ENRICH_FEED>(cyclus::Agent* agent, double value) {
  RecordTimeSeries<double>("EnrichmentFeed", agent, value);
  CallListenersDouble(ENRICH_FEED, agent, value);
}

}  // namespace toolkit
}  // namespace cyclus
