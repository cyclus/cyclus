#include "reactor.h"

using cyclus::Material;
using cyclus::Composition;
using cyclus::toolkit::ResBuf;
using cyclus::toolkit::MatVec;
using cyclus::KeyError;
using cyclus::ValueError;
using cyclus::Request;

namespace cycamore {

Reactor::Reactor(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      n_assem_batch(0),
      assem_size(0),
      n_assem_core(0),
      n_assem_spent(0),
      n_assem_fresh(0),
      cycle_time(0),
      refuel_time(0),
      cycle_step(0),
      power_cap(0),
      power_name("power"),
      discharged(false),
      latitude(0.0),
      longitude(0.0),
      coordinates(latitude, longitude) {}


#pragma cyclus def clone cycamore::Reactor

#pragma cyclus def schema cycamore::Reactor

#pragma cyclus def annotations cycamore::Reactor

#pragma cyclus def infiletodb cycamore::Reactor

#pragma cyclus def snapshot cycamore::Reactor

#pragma cyclus def snapshotinv cycamore::Reactor

#pragma cyclus def initinv cycamore::Reactor

void Reactor::InitFrom(Reactor* m) {
  #pragma cyclus impl initfromcopy cycamore::Reactor
  cyclus::toolkit::CommodityProducer::Copy(m);
}

void Reactor::InitFrom(cyclus::QueryableBackend* b) {
  #pragma cyclus impl initfromdb cycamore::Reactor

  namespace tk = cyclus::toolkit;
  tk::CommodityProducer::Add(tk::Commodity(power_name),
                             tk::CommodInfo(power_cap, power_cap));

  for (int i = 0; i < side_products.size(); i++) {
    tk::CommodityProducer::Add(tk::Commodity(side_products[i]),
                               tk::CommodInfo(side_product_quantity[i],
                                              side_product_quantity[i]));
  }
}

void Reactor::EnterNotify() {
  cyclus::Facility::EnterNotify();

  // If the user ommitted fuel_prefs, we set it to zeros for each fuel
  // type.  Without this segfaults could occur - yuck.
  if (fuel_prefs.size() == 0) {
    for (int i = 0; i < fuel_outcommods.size(); i++) {
      fuel_prefs.push_back(cyclus::kDefaultPref);
    }
  }

  // Test if any side products have been defined.
  if (side_products.size() == 0){
    hybrid_ = false;
  }

  // input consistency checking:
  int n = recipe_change_times.size();
  std::stringstream ss;
  if (recipe_change_commods.size() != n) {
    ss << "prototype '" << prototype() << "' has "
       << recipe_change_commods.size()
       << " recipe_change_commods vals, expected " << n << "\n";
  }
  if (recipe_change_in.size() != n) {
    ss << "prototype '" << prototype() << "' has " << recipe_change_in.size()
       << " recipe_change_in vals, expected " << n << "\n";
  }
  if (recipe_change_out.size() != n) {
    ss << "prototype '" << prototype() << "' has " << recipe_change_out.size()
       << " recipe_change_out vals, expected " << n << "\n";
  }

  n = pref_change_times.size();
  if (pref_change_commods.size() != n) {
    ss << "prototype '" << prototype() << "' has " << pref_change_commods.size()
       << " pref_change_commods vals, expected " << n << "\n";
  }
  if (pref_change_values.size() != n) {
    ss << "prototype '" << prototype() << "' has " << pref_change_values.size()
       << " pref_change_values vals, expected " << n << "\n";
  }

  if (ss.str().size() > 0) {
    throw cyclus::ValueError(ss.str());
  }
  RecordPosition();
}

bool Reactor::CheckDecommissionCondition() {
  return core.count() == 0 && spent.count() == 0;
}

void Reactor::Tick() {
  // The following code must go in the Tick so they fire on the time step
  // following the cycle_step update - allowing for the all reactor events to
  // occur and be recorded on the "beginning" of a time step.  Another reason
  // they
  // can't go at the beginnin of the Tock is so that resource exchange has a
  // chance to occur after the discharge on this same time step.

  if (retired()) {
    Record("RETIRED", "");

    if (context()->time() == exit_time() + 1) { // only need to transmute once
      if (decom_transmute_all == true) {
        Transmute(ceil(static_cast<double>(n_assem_core)));
      }
      else {
        Transmute(ceil(static_cast<double>(n_assem_core) / 2.0));
      }
    }
    while (core.count() > 0) {
      if (!Discharge()) {
        break;
      }
    }
    // in case a cycle lands exactly on our last time step, we will need to
    // burn a batch from fresh inventory on this time step.  When retired,
    // this batch also needs to be discharged to spent fuel inventory.
    while (fresh.count() > 0 && spent.space() >= assem_size) {
      spent.Push(fresh.Pop());
    }
    if(CheckDecommissionCondition()) {
      Decommission();    
    }
    return;
  }

  if (cycle_step == cycle_time) {
    Transmute();
    Record("CYCLE_END", "");
  }

  if (cycle_step >= cycle_time && !discharged) {
    discharged = Discharge();
  }
  if (cycle_step >= cycle_time) {
    Load();
  }

  int t = context()->time();

  // update preferences
  for (int i = 0; i < pref_change_times.size(); i++) {
    int change_t = pref_change_times[i];
    if (t != change_t) {
      continue;
    }

    std::string incommod = pref_change_commods[i];
    for (int j = 0; j < fuel_incommods.size(); j++) {
      if (fuel_incommods[j] == incommod) {
        fuel_prefs[j] = pref_change_values[i];
        break;
      }
    }
  }

  // update recipes
  for (int i = 0; i < recipe_change_times.size(); i++) {
    int change_t = recipe_change_times[i];
    if (t != change_t) {
      continue;
    }

    std::string incommod = recipe_change_commods[i];
    for (int j = 0; j < fuel_incommods.size(); j++) {
      if (fuel_incommods[j] == incommod) {
        fuel_inrecipes[j] = recipe_change_in[i];
        fuel_outrecipes[j] = recipe_change_out[i];
        break;
      }
    }
  }
}

std::set<cyclus::RequestPortfolio<Material>::Ptr> Reactor::GetMatlRequests() {
  using cyclus::RequestPortfolio;

  std::set<RequestPortfolio<Material>::Ptr> ports;
  Material::Ptr m;

  // second min expression reduces assembles to amount needed until
  // retirement if it is near.
  int n_assem_order = n_assem_core - core.count() + n_assem_fresh - fresh.count();

  if (exit_time() != -1) {
    // the +1 accounts for the fact that the reactor is alive and gets to
    // operate during its exit_time time step.
    int t_left = exit_time() - context()->time() + 1;
    int t_left_cycle = cycle_time + refuel_time - cycle_step;
    double n_cycles_left = static_cast<double>(t_left - t_left_cycle) /
                         static_cast<double>(cycle_time + refuel_time);
    n_cycles_left = ceil(n_cycles_left);
    int n_need = std::max(0.0, n_cycles_left * n_assem_batch - n_assem_fresh + n_assem_core - core.count());
    n_assem_order = std::min(n_assem_order, n_need);
  }

  if (n_assem_order == 0) {
    return ports;
  } else if (retired()) {
    return ports;
  }

  for (int i = 0; i < n_assem_order; i++) {
    RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());
    std::vector<Request<Material>*> mreqs;
    for (int j = 0; j < fuel_incommods.size(); j++) {
      std::string commod = fuel_incommods[j];
      double pref = fuel_prefs[j];
      Composition::Ptr recipe = context()->GetRecipe(fuel_inrecipes[j]);
      m = Material::CreateUntracked(assem_size, recipe);

      Request<Material>* r = port->AddRequest(m, this, commod, pref, true);
      mreqs.push_back(r);
    }

    std::vector<double>::iterator result;
    result = std::max_element(fuel_prefs.begin(), fuel_prefs.end());
    int max_index = std::distance(fuel_prefs.begin(), result);

    cyclus::toolkit::RecordTimeSeries<double>("demand"+fuel_incommods[max_index], this,
                                          assem_size * n_assem_order) ;

    port->AddMutualReqs(mreqs);
    ports.insert(port);
  }

  return ports;
}

void Reactor::GetMatlTrades(
    const std::vector<cyclus::Trade<Material> >& trades,
    std::vector<std::pair<cyclus::Trade<Material>, Material::Ptr> >&
        responses) {
  using cyclus::Trade;

  std::map<std::string, MatVec> mats = PopSpent();
  for (int i = 0; i < trades.size(); i++) {
    std::string commod = trades[i].request->commodity();
    Material::Ptr m = mats[commod].back();
    mats[commod].pop_back();
    responses.push_back(std::make_pair(trades[i], m));
    res_indexes.erase(m->obj_id());
  }
  PushSpent(mats);  // return leftovers back to spent buffer
}

void Reactor::AcceptMatlTrades(const std::vector<
    std::pair<cyclus::Trade<Material>, Material::Ptr> >& responses) {
  std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                        cyclus::Material::Ptr> >::const_iterator trade;

  std::stringstream ss;
  int nload = std::min((int)responses.size(), n_assem_core - core.count());
  if (nload > 0) {
    ss << nload << " assemblies";
    Record("LOAD", ss.str());
  }

  for (trade = responses.begin(); trade != responses.end(); ++trade) {
    std::string commod = trade->first.request->commodity();
    Material::Ptr m = trade->second;
    index_res(m, commod);

    if (core.count() < n_assem_core) {
      core.Push(m);
    } else {
      fresh.Push(m);
    }
  }
}

std::set<cyclus::BidPortfolio<Material>::Ptr> Reactor::GetMatlBids(
    cyclus::CommodMap<Material>::type& commod_requests) {
  using cyclus::BidPortfolio;
  std::set<BidPortfolio<Material>::Ptr> ports;

  bool gotmats = false;
  std::map<std::string, MatVec> all_mats;

  if (uniq_outcommods_.empty()) {
    for (int i = 0; i < fuel_outcommods.size(); i++) {
      uniq_outcommods_.insert(fuel_outcommods[i]);
    }
  }

  std::set<std::string>::iterator it;
  for (it = uniq_outcommods_.begin(); it != uniq_outcommods_.end(); ++it) {
    std::string commod = *it;
    std::vector<Request<Material>*>& reqs = commod_requests[commod];
    if (reqs.size() == 0) {
      continue;
    } else if (!gotmats) {
      all_mats = PeekSpent();
    }

    MatVec mats = all_mats[commod];
    if (mats.size() == 0) {
      continue;
    }

    BidPortfolio<Material>::Ptr port(new BidPortfolio<Material>());

    for (int j = 0; j < reqs.size(); j++) {
      Request<Material>* req = reqs[j];
      double tot_bid = 0;
      for (int k = 0; k < mats.size(); k++) {
        Material::Ptr m = mats[k];
        tot_bid += m->quantity();
        port->AddBid(req, m, this, true);
        if (tot_bid >= req->target()->quantity()) {
          break;
        }
      }
    }

    double tot_qty = 0;
    for (int j = 0; j < mats.size(); j++) {
      tot_qty += mats[j]->quantity();
    }

    cyclus::CapacityConstraint<Material> cc(tot_qty);
    port->AddConstraint(cc);
    ports.insert(port);
  }

  return ports;
}

void Reactor::Tock() {
  if (retired()) {
    return;
  }
  
  // Check that irradiation and refueling periods are over, that 
  // the core is full and that fuel was successfully discharged in this refueling time.
  // If this is the case, then a new cycle will be initiated.
  if (cycle_step >= cycle_time + refuel_time && core.count() == n_assem_core && discharged == true) {
    discharged = false;
    cycle_step = 0;
  }

  if (cycle_step == 0 && core.count() == n_assem_core) {
    Record("CYCLE_START", "");
  }

  if (cycle_step >= 0 && cycle_step < cycle_time &&
      core.count() == n_assem_core) {
    cyclus::toolkit::RecordTimeSeries<cyclus::toolkit::POWER>(this, power_cap);
    cyclus::toolkit::RecordTimeSeries<double>("supplyPOWER", this, power_cap);
    RecordSideProduct(true);
  } else {
    cyclus::toolkit::RecordTimeSeries<cyclus::toolkit::POWER>(this, 0);
    cyclus::toolkit::RecordTimeSeries<double>("supplyPOWER", this, 0);
    RecordSideProduct(false);
  }

  // "if" prevents starting cycle after initial deployment until core is full
  // even though cycle_step is its initial zero.
  if (cycle_step > 0 || core.count() == n_assem_core) {
    cycle_step++;
  }
}

void Reactor::Transmute() { Transmute(n_assem_batch); }

void Reactor::Transmute(int n_assem) {
  MatVec old = core.PopN(std::min(n_assem, core.count()));
  core.Push(old);
  if (core.count() > old.size()) {
    // rotate untransmuted mats back to back of buffer
    core.Push(core.PopN(core.count() - old.size()));
  }

  std::stringstream ss;
  ss << old.size() << " assemblies";
  Record("TRANSMUTE", ss.str());

  for (int i = 0; i < old.size(); i++) {
    old[i]->Transmute(context()->GetRecipe(fuel_outrecipe(old[i])));
  }
}

std::map<std::string, MatVec> Reactor::PeekSpent() {
  std::map<std::string, MatVec> mapped;
  MatVec mats = spent.PopN(spent.count());
  spent.Push(mats);
  for (int i = 0; i < mats.size(); i++) {
    std::string commod = fuel_outcommod(mats[i]);
    mapped[commod].push_back(mats[i]);
  }
  return mapped;
}

bool Reactor::Discharge() {
  int npop = std::min(n_assem_batch, core.count());
  if (n_assem_spent - spent.count() < npop) {
    Record("DISCHARGE", "failed");
    return false;  // not enough room in spent buffer
  }

  std::stringstream ss;
  ss << npop << " assemblies";
  Record("DISCHARGE", ss.str());
  spent.Push(core.PopN(npop));

  std::map<std::string, MatVec> spent_mats;
  for (int i = 0; i < fuel_outcommods.size(); i++) {
    spent_mats = PeekSpent();
    MatVec mats = spent_mats[fuel_outcommods[i]];
    double tot_spent = 0;
    for (int j = 0; j<mats.size(); j++){
      Material::Ptr m = mats[j];
      tot_spent += m->quantity();
    }
    cyclus::toolkit::RecordTimeSeries<double>("supply"+fuel_outcommods[i], this, tot_spent);
  }

  return true;
}

void Reactor::Load() {
  int n = std::min(n_assem_core - core.count(), fresh.count());
  if (n == 0) {
    return;
  }

  std::stringstream ss;
  ss << n << " assemblies";
  Record("LOAD", ss.str());
  core.Push(fresh.PopN(n));
}

std::string Reactor::fuel_incommod(Material::Ptr m) {
  int i = res_indexes[m->obj_id()];
  if (i >= fuel_incommods.size()) {
    throw KeyError("cycamore::Reactor - no incommod for material object");
  }
  return fuel_incommods[i];
}

std::string Reactor::fuel_outcommod(Material::Ptr m) {
  int i = res_indexes[m->obj_id()];
  if (i >= fuel_outcommods.size()) {
    throw KeyError("cycamore::Reactor - no outcommod for material object");
  }
  return fuel_outcommods[i];
}

std::string Reactor::fuel_inrecipe(Material::Ptr m) {
  int i = res_indexes[m->obj_id()];
  if (i >= fuel_inrecipes.size()) {
    throw KeyError("cycamore::Reactor - no inrecipe for material object");
  }
  return fuel_inrecipes[i];
}

std::string Reactor::fuel_outrecipe(Material::Ptr m) {
  int i = res_indexes[m->obj_id()];
  if (i >= fuel_outrecipes.size()) {
    throw KeyError("cycamore::Reactor - no outrecipe for material object");
  }
  return fuel_outrecipes[i];
}

double Reactor::fuel_pref(Material::Ptr m) {
  int i = res_indexes[m->obj_id()];
  if (i >= fuel_prefs.size()) {
    return 0;
  }
  return fuel_prefs[i];
}

void Reactor::index_res(cyclus::Resource::Ptr m, std::string incommod) {
  for (int i = 0; i < fuel_incommods.size(); i++) {
    if (fuel_incommods[i] == incommod) {
      res_indexes[m->obj_id()] = i;
      return;
    }
  }
  throw ValueError(
      "cycamore::Reactor - received unsupported incommod material");
}

std::map<std::string, MatVec> Reactor::PopSpent() {
  MatVec mats = spent.PopN(spent.count());
  std::map<std::string, MatVec> mapped;
  for (int i = 0; i < mats.size(); i++) {
    std::string commod = fuel_outcommod(mats[i]);
    mapped[commod].push_back(mats[i]);
  }

  // needed so we trade away oldest assemblies first
  std::map<std::string, MatVec>::iterator it;
  for (it = mapped.begin(); it != mapped.end(); ++it) {
    std::reverse(it->second.begin(), it->second.end());
  }

  return mapped;
}

void Reactor::PushSpent(std::map<std::string, MatVec> leftover) {
  std::map<std::string, MatVec>::iterator it;
  for (it = leftover.begin(); it != leftover.end(); ++it) {
    // undo reverse in PopSpent to make sure oldest assemblies come out first
    std::reverse(it->second.begin(), it->second.end());
    spent.Push(it->second);
  }
}

void Reactor::RecordSideProduct(bool produce){
  if (hybrid_){
    double value;
    for (int i = 0; i < side_products.size(); i++) {
      if (produce){
          value = side_product_quantity[i];
      }
      else {
          value = 0;
      }

      context()
          ->NewDatum("ReactorSideProducts")
          ->AddVal("AgentId", id())
          ->AddVal("Time", context()->time())
          ->AddVal("Product", side_products[i])
          ->AddVal("Value", value)
          ->Record();
    }
  }
}

void Reactor::Record(std::string name, std::string val) {
  context()
      ->NewDatum("ReactorEvents")
      ->AddVal("AgentId", id())
      ->AddVal("Time", context()->time())
      ->AddVal("Event", name)
      ->AddVal("Value", val)
      ->Record();
}

void Reactor::RecordPosition() {
  std::string specification = this->spec();
  context()
      ->NewDatum("AgentPosition")
      ->AddVal("Spec", specification)
      ->AddVal("Prototype", this->prototype())
      ->AddVal("AgentId", id())
      ->AddVal("Latitude", latitude)
      ->AddVal("Longitude", longitude)
      ->Record();
}

extern "C" cyclus::Agent* ConstructReactor(cyclus::Context* ctx) {
  return new Reactor(ctx);
}

}  // namespace cycamore
