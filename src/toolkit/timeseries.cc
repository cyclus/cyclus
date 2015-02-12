#include "error.h"

namespace cyclus {
namespace toolkit {

void RecordTimeSeries(TimeSeriesTypes tstype, cyclus::Agent* agent,
                      double value) {
  using std::string;
  string strtype;
  switch (tstype) {
    case POWER : {
      strtype = "Power";
      break;
    }
    default: {
      throw cyclus::ValueError("TimeSeriesType not understood!");
    }
  }
  RecordTimeSeries(strtype, agent, value);
}

void RecordTimeSeries(std::string tstype, cyclus::Agent* agent, double value) {
  using std::string;
  std::string tblname = "TimeSeries" + tstype;
  agent->context()->NewDatum(tblname)
       ->AddValue("AgentId", agent->id())
       ->AddValue("Time", agent->context()->time())
       ->AddValue("Value", value)
       ->Record();
}

}  // namespace toolkit
}  // namespace cyclus
