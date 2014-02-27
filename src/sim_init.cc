
#include "sim_init.h"

#include <boost/lexical_cast.hpp>

#define SHOW(X) \
  std::cout << __FILE__ << ":" << __LINE__ << ": "#X" = " << X << "\n"

namespace cyclus {

SimInit::SimInit() {
  se_ = new SimEngine();
  se_->ti = new Timer();
}


SimEngine* SimInit::Init(QueryBackend* b, boost::uuids::uuid simid) {
  se_->rec = new Recorder(simid);
  b_ = b;
  simid_ = simid;
  t_ = 0;
  return InitBase();
}

SimEngine* SimInit::Restart(QueryBackend* b, boost::uuids::uuid simid, int t) {
  se_->rec = new Recorder();
  b_ = b;
  simid_ = simid;
  t_ = t;
  return InitBase();
}

SimEngine* SimInit::InitBase() {
  se_->ctx = new Context(se_->ti, se_->rec);

  LoadControlParams();
  LoadRecipes();
  LoadSolverInfo();
  LoadPrototypes();
  LoadInitialAgents();
  LoadInventories();

  // use rec.set_dump_count to reset all buffered datums that we don't want
  // to be eventually sent to backends that are to-be-added.
  se_->rec->set_dump_count(kDefaultDumpCount);

  return se_;
}

void SimInit::LoadControlParams() {
  std::vector<Cond> conds;
  conds.push_back(Cond("SimId", "==", simid_));
  QueryResult qr = b_->Query("Info", &conds);

  int dur = qr.GetVal<int>(0, "Duration");
  int dec = qr.GetVal<int>(0, "DecayInterval");
  int y0 = qr.GetVal<int>(0, "InitialYear");
  int m0 = qr.GetVal<int>(0, "InitialMonth");

  SHOW(dur);
  SHOW(dec);
  SHOW(y0);
  SHOW(m0);
  SHOW(se_->rec->sim_id());

  se_->ctx->InitTime(dur, dec, m0, y0);
}

void SimInit::LoadRecipes() {
}

void SimInit::LoadSolverInfo() {
}

void SimInit::LoadPrototypes() {
}

void SimInit::LoadInitialAgents() {
}

void SimInit::LoadInventories() {
}

} // namespace cyclus
