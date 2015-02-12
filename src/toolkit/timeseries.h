#ifndef CYCLUS_SRC_TOOLKIT_TIMESERIES_H_
#define CYCLUS_SRC_TOOLKIT_TIMESERIES_H_

#include <string>

namespace cyclus {
namespace toolkit {

/// Time series types to be used in the RecordTimeSeries() fucntions.
/// These types have the following unit which *must* be adhered to strictly:
/// - POWER [MWe]
enum TimeSeriesTypes {
  POWER,
};

/// Records a per-time step quantity for a given type
void RecordTimeSeries(TimeSeriesTypes tstype, cyclus::Agent* agent,
                      double value);

/// Records a per-time step quantity for a string
void RecordTimeSeries(std::string tstype, cyclus::Agent* agent,
                      double value);


}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_TIMESERIES_H_
