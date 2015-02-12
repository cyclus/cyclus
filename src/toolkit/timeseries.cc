namespace cyclus {
namespace toolkit {

void RecordTimeSeries<POWER>(cyclus::Agent* agent, double value) {
  RecordTimeSeries<double>("Power", agent, value);
}


}  // namespace toolkit
}  // namespace cyclus
