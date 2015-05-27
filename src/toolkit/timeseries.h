#ifndef CYCLUS_SRC_TOOLKIT_TIMESERIES_H_
#define CYCLUS_SRC_TOOLKIT_TIMESERIES_H_

#include <string>

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

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_TIMESERIES_H_
