
#include "sim_init.h"

#include "greedy_preconditioner.h"
#include "greedy_solver.h"

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

  se_->ctx->InitTime(dur, dec, m0, y0);
}

void SimInit::LoadRecipes() {
  std::vector<Cond> conds;
  conds.push_back(Cond("SimId", "==", simid_));
  QueryResult qr = b_->Query("Recipes", &conds);

  for (int i = 0; i < qr.rows.size(); ++i) {
    std::string recipe = qr.GetVal<std::string>(i, "Recipe");
    int stateid = qr.GetVal<int>(i, "StateId");

    std::vector<Cond> conds;
    conds.push_back(Cond("SimId", "==", simid_));
    conds.push_back(Cond("StateId", "==", stateid));
    QueryResult qr = b_->Query("Compositions", &conds);
    CompMap m;
    SHOW(recipe);
    for (int j = 0; j < qr.rows.size(); ++j) {
      int nuc = qr.GetVal<int>(j, "NucId");
      double frac = qr.GetVal<double>(j, "MassFrac");
      m[nuc] = frac;
      SHOW(nuc);
      SHOW(frac);
    }
    Composition::Ptr comp = Composition::CreateFromMass(m);
    se_->ctx->AddRecipe(recipe, comp);
  }
}

void SimInit::LoadSolverInfo() {
  std::vector<Cond> conds;
  conds.push_back(Cond("SimId", "==", simid_));
  QueryResult qr = b_->Query("CommodPriority", &conds);

  std::map<std::string, double> commod_order;
  for (int i = 0; i < qr.rows.size(); ++i) {
    std::string commod = qr.GetVal<std::string>(i, "Commodity");
    double order = qr.GetVal<double>(i, "SolutionOrder");
    commod_order[commod] = order;
  }

  // solver will delete conditioner
  GreedyPreconditioner* conditioner = new GreedyPreconditioner(
    commod_order,
    GreedyPreconditioner::REVERSE);

  // context will delete solver
  bool exclusive_orders = false;
  GreedySolver* solver = new GreedySolver(exclusive_orders, conditioner);

  se_->ctx->solver(solver);
}

void SimInit::LoadPrototypes() {
  std::vector<Cond> conds;
  conds.push_back(Cond("SimId", "==", simid_));
  QueryResult qr = b_->Query("Prototypes", &conds);
  for (int i = 0; i < qr.rows.size(); ++i) {
    std::string proto = qr.GetVal<std::string>(i, "Prototype");
    std::string impl = qr.GetVal<std::string>(i, "Implementation");

  }
}

void SimInit::LoadInitialAgents() {
}

void SimInit::LoadInventories() {
}

} // namespace cyclus
