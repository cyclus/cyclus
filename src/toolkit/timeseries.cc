#include "timeseries.h"

namespace cyclus {
namespace toolkit {

template <>
void RecordTimeSeries<POWER>(cyclus::Agent* agent, double value) {
  RecordTimeSeries<double>("Power", agent, value);
}


}  // namespace toolkit
}  // namespace cyclus
