#ifndef CYCLUS_SRC_TOOLKIT_TIMESERIES_H_
#define CYCLUS_SRC_TOOLKIT_TIMESERIES_H_

#include <string>
#include <map>
#include <vector>
#include <functional>

#include "agent.h"
#include "context.h"

namespace cyclus {
namespace toolkit {

/// Time series types to be used in the RecordTimeSeries() functions.
/// These types have the following unit which *must* be adhered to strictly:
/// - POWER [MWe]
/// - ENRICH_SWU [kg SWU]
/// - ENRICH_FEED [kg]
enum TimeSeriesType {
  POWER,
  ENRICH_SWU,
  ENRICH_FEED,
};

/// Stores global information for the time series call backs.
typedef std::function<void(cyclus::Agent*, int, double)> time_series_listener_t;
extern std::map<TimeSeriesType, std::vector<time_series_listener_t> > TIME_SERIES_LISTENERS_DOUBLE;

/// Records a per-time step quantity for a given type
template <TimeSeriesType T>
void RecordTimeSeries(cyclus::Agent* agent, double value);

/// Records a per-time step quantity for a string
template <typename T>
void RecordTimeSeries(std::string tsname, cyclus::Agent* agent, T value) {
  std::string tblname = "TimeSeries" + tsname;
  agent->context()->NewDatum(tblname)
       ->AddVal("AgentId", agent->id())
       ->AddVal("Time", agent->context()->time())
       ->AddVal("Value", value)
       ->Record();
}

template <class Ret, class T>
time_series_listener_t BindListernerMemberFunction(Ret T::* pm){
  return std::mem_fn(pm);
}

void CallListenersDouble(TimeSeriesType tstype, cyclus::Agent* agent, double value);

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_TIMESERIES_H_
