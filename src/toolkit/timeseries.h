#ifndef CYCLUS_SRC_TOOLKIT_TIMESERIES_H_
#define CYCLUS_SRC_TOOLKIT_TIMESERIES_H_

#include <string>
#include <map>
#include <vector>
#include <functional>

#include "boost/variant.hpp"
#include "boost/variant/get.hpp"

#include "agent.h"
#include "context.h"
#include "any.hpp"

namespace cyclus {
namespace toolkit {

/// Time series types to be used in the RecordTimeSeries() functions.
/// These types have the following unit which *must* be adhered to strictly:
/// - POWER [MWe]
/// - ENRICH_SWU [kg SWU]
/// - ENRICH_FEED [kg]
enum TimeSeriesType : int{
  POWER,
  ENRICH_SWU,
  ENRICH_FEED,
};

/// Stores global information for the time series call backs.
typedef boost::variant<    
    std::function<void(cyclus::Agent*, int, bool, std::string)>,
    std::function<void(cyclus::Agent*, int, int, std::string)>,
    std::function<void(cyclus::Agent*, int, float, std::string)>,
    std::function<void(cyclus::Agent*, int, double, std::string)>,
    std::function<void(cyclus::Agent*, int, std::string, std::string)>
    > time_series_listener_t;

extern std::map<std::string, std::vector<time_series_listener_t> > TIME_SERIES_LISTENERS;

/// Records a per-time step quantity for a given type
template <TimeSeriesType T>
void RecordTimeSeries(cyclus::Agent* agent, double value);

/// Records a per-time step quantity for a string
template <typename T>
void RecordTimeSeries(std::string tsname, cyclus::Agent* agent, T value) {
  std::string tblname = "TimeSeries" + tsname;
  int time = agent->context()->time();
  agent->context()->NewDatum(tblname)
       ->AddVal("AgentId", agent->id())
       ->AddVal("Time", time)
       ->AddVal("Value", value)
       ->Record();
  std::vector<time_series_listener_t> vec = TIME_SERIES_LISTENERS[tsname];
  for (auto f=vec.begin(); f != vec.end(); ++f){
    std::function<void(cyclus::Agent*, int, T, std::string)> fn = boost::get<std::function<void(cyclus::Agent*, int, T, std::string)> >(*f);
    fn(agent, time, value, tsname); 
  }
  PyCallListeners(tsname, agent, agent->context(), time, value);
}

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_TIMESERIES_H_
