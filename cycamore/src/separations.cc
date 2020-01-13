#include "separations.h"

using cyclus::Material;
using cyclus::Composition;
using cyclus::toolkit::ResBuf;
using cyclus::toolkit::MatVec;
using cyclus::KeyError;
using cyclus::ValueError;
using cyclus::Request;
using cyclus::CompMap;

namespace cycamore {

Separations::Separations(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      latitude(0.0),
      longitude(0.0),
      coordinates(latitude, longitude) {}

cyclus::Inventories Separations::SnapshotInv() {
  cyclus::Inventories invs;

  // these inventory names are intentionally convoluted so as to not clash
  // with the user-specified stream commods that are used as the separations
  // streams inventory names.
  invs["leftover-inv-name"] = leftover.PopNRes(leftover.count());
  leftover.Push(invs["leftover-inv-name"]);
  invs["feed-inv-name"] = feed.PopNRes(feed.count());
  feed.Push(invs["feed-inv-name"]);

  std::map<std::string, ResBuf<Material> >::iterator it;
  for (it = streambufs.begin(); it != streambufs.end(); ++it) {
    invs[it->first] = it->second.PopNRes(it->second.count());
    it->second.Push(invs[it->first]);
  }

  return invs;
}

void Separations::InitInv(cyclus::Inventories& inv) {
  leftover.Push(inv["leftover-inv-name"]);
  feed.Push(inv["feed-inv-name"]);

  cyclus::Inventories::iterator it;
  for (it = inv.begin(); it != inv.end(); ++it) {
    streambufs[it->first].Push(it->second);
  }
}

typedef std::pair<double, std::map<int, double> > Stream;
typedef std::map<std::string, Stream> StreamSet;

void Separations::EnterNotify() {
  cyclus::Facility::EnterNotify();
  std::map<int, double> efficiency_;

  StreamSet::iterator it;
  std::map<int, double>::iterator it2;

  for (it = streams_.begin(); it != streams_.end(); ++it) {
    std::string name = it->first;
    Stream stream = it->second;
    double cap = stream.first;
    if (cap >= 0) {
      streambufs[name].capacity(cap);
    }

    for (it2 = stream.second.begin(); it2 != stream.second.end(); it2++) {
      efficiency_[it2->first] += it2->second;
    }
    RecordPosition();
  }

  std::vector<int> eff_pb_;
  for (it2 = efficiency_.begin(); it2 != efficiency_.end(); it2++) {
    if (it2->second > 1) {
      eff_pb_.push_back(it2->first);
    }
  }

  if (eff_pb_.size() > 0) {
    std::stringstream ss;
    ss << "In " << prototype() << ", ";
    ss << "the following nuclide(s) have a cumulative separation efficiency "
          "greater than 1:";
    for (int i = 0; i < eff_pb_.size(); i++) {
      ss << "\n    " << eff_pb_[i];
      if (i < eff_pb_.size() - 1) {
        ss << ",";
      } else {
        ss << ".";
      }
    }

    throw cyclus::ValueError(ss.str());
  }

  if (feed_commod_prefs.size() == 0) {
    for (int i = 0; i < feed_commods.size(); i++) {
      feed_commod_prefs.push_back(cyclus::kDefaultPref);
    }
  }
}

void Separations::Tick() {
  using cyclus::toolkit::RecordTimeSeries;
  if (feed.count() == 0) {
    return;
  }
  double pop_qty = std::min(throughput, feed.quantity());
  Material::Ptr mat = feed.Pop(pop_qty, cyclus::eps_rsrc());
  double orig_qty = mat->quantity();

  StreamSet::iterator it;
  double maxfrac = 1;
  std::map<std::string, Material::Ptr> stagedsep;
  Record("Separating", orig_qty, "feed");
  for (it = streams_.begin(); it != streams_.end(); ++it) {
    Stream info = it->second;
    std::string name = it->first;
    stagedsep[name] = SepMaterial(info.second, mat);
    double frac = streambufs[name].space() / stagedsep[name]->quantity();
    if (frac < maxfrac) {
      maxfrac = frac;
    }
  }

  std::map<std::string, Material::Ptr>::iterator itf;
  for (itf = stagedsep.begin(); itf != stagedsep.end(); ++itf) {
    std::string name = itf->first;
    Material::Ptr m = itf->second;
    if (m->quantity() > 0) {
      double qty = m->quantity();
      if (m->quantity() > mat->quantity()) {
        qty = mat->quantity();
      }
      streambufs[name].Push(
          mat->ExtractComp(qty * maxfrac, m->comp()));
      Record("Separated", qty * maxfrac, name);
    }
    cyclus::toolkit::RecordTimeSeries<double>("supply"+name, this,
                                              streambufs[name].quantity());
  }

  if (maxfrac == 1) {
    if (mat->quantity() > 0) {
      // unspecified separations fractions go to leftovers
      leftover.Push(mat);
    }
  } else {  // maxfrac is < 1
    // push back any leftover feed due to separated stream inv size constraints

    feed.Push(mat->ExtractQty((1 - maxfrac) * orig_qty));
    if (mat->quantity() > 0) {
      // unspecified separations fractions go to leftovers
      leftover.Push(mat);
    }
  }
  cyclus::toolkit::RecordTimeSeries<double>("supply"+leftover_commod, this,
                                            leftover.quantity());

}

// Note that this returns an untracked material that should just be used for
// its composition and qty - not in any real inventories, etc.
Material::Ptr SepMaterial(std::map<int, double> effs, Material::Ptr mat) {
  CompMap cm = mat->comp()->mass();
  cyclus::compmath::Normalize(&cm, mat->quantity());
  double tot_qty = 0;
  CompMap sepcomp;

  CompMap::iterator it;
  for (it = cm.begin(); it != cm.end(); ++it) {
    int nuc = it->first;
    int elem = (nuc / 10000000) * 10000000;
    double eff = 0;
    if (effs.count(nuc) > 0) {
      eff = effs[nuc];
    } else if (effs.count(elem) > 0) {
      eff = effs[elem];
    } else {
      continue;
    }

    double qty = it->second;
    double sepqty = qty * eff;
    sepcomp[nuc] = sepqty;
    tot_qty += sepqty;
  }

  Composition::Ptr c = Composition::CreateFromMass(sepcomp);
  return Material::CreateUntracked(tot_qty, c);
};

std::set<cyclus::RequestPortfolio<Material>::Ptr>
Separations::GetMatlRequests() {
  using cyclus::RequestPortfolio;
  using cyclus::toolkit::RecordTimeSeries;
  std::set<RequestPortfolio<Material>::Ptr> ports;

  int t = context()->time();
  int t_exit = exit_time();

  // record demand of highest-preferred commodity
  std::vector<double>::iterator result;
  result = std::max_element(feed_commod_prefs.begin(), feed_commod_prefs.end());
  int maxindx = std::distance(feed_commod_prefs.begin(), result);
  cyclus::toolkit::RecordTimeSeries<double>("demand"+feed_commods[maxindx],
                                            this, feed.space());
  if (t_exit >= 0 && (feed.quantity() >= (t_exit - t) * throughput)) {
    return ports;  // already have enough feed for remainder of life
  } else if (feed.space() < cyclus::eps_rsrc()) {
    return ports;
  }

  bool exclusive = false;
  RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());

  Material::Ptr m = cyclus::NewBlankMaterial(feed.space());
  if (!feed_recipe.empty()) {
    Composition::Ptr c = context()->GetRecipe(feed_recipe);
    m = Material::CreateUntracked(feed.space(), c);
  }

  std::vector<cyclus::Request<Material>*> reqs;
  for (int i = 0; i < feed_commods.size(); i++) {
    std::string commod = feed_commods[i];
    double pref = feed_commod_prefs[i];
    reqs.push_back(port->AddRequest(m, this, commod, pref, exclusive));
  }
  port->AddMutualReqs(reqs);
  ports.insert(port);

  return ports;
}

void Separations::GetMatlTrades(
    const std::vector<cyclus::Trade<Material> >& trades,
    std::vector<std::pair<cyclus::Trade<Material>, Material::Ptr> >&
        responses) {
  using cyclus::Trade;

  std::vector<cyclus::Trade<cyclus::Material> >::const_iterator it;
  for (int i = 0; i < trades.size(); i++) {
    std::string commod = trades[i].request->commodity();
    if (commod == leftover_commod) {
      double amt = std::min(leftover.quantity(), trades[i].amt);
      Material::Ptr m = leftover.Pop(amt, cyclus::eps_rsrc());
      responses.push_back(std::make_pair(trades[i], m));
    } else if (streambufs.count(commod) > 0) {
      double amt = std::min(streambufs[commod].quantity(), trades[i].amt);
      Material::Ptr m = streambufs[commod].Pop(amt, cyclus::eps_rsrc());
      responses.push_back(std::make_pair(trades[i], m));
    } else {
      throw ValueError("invalid commodity " + commod +
                       " on trade matched to prototype " + prototype());
    }
  }
}

void Separations::AcceptMatlTrades(
    const std::vector<std::pair<cyclus::Trade<Material>, Material::Ptr> >&
        responses) {
  std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                        cyclus::Material::Ptr> >::const_iterator trade;

  for (trade = responses.begin(); trade != responses.end(); ++trade) {
    feed.Push(trade->second);
  }
}

std::set<cyclus::BidPortfolio<Material>::Ptr> Separations::GetMatlBids(
    cyclus::CommodMap<Material>::type& commod_requests) {
  using cyclus::BidPortfolio;
  bool exclusive = false;
  std::set<BidPortfolio<Material>::Ptr> ports;

  // bid streams
  std::map<std::string, ResBuf<Material> >::iterator it;
  for (it = streambufs.begin(); it != streambufs.end(); ++it) {
    std::string commod = it->first;
    std::vector<Request<Material>*>& reqs = commod_requests[commod];
    if (reqs.size() == 0) {
      continue;
    } else if (streambufs[commod].quantity() < cyclus::eps_rsrc()) {
      continue;
    }

    MatVec mats = streambufs[commod].PopN(streambufs[commod].count());
    streambufs[commod].Push(mats);

    BidPortfolio<Material>::Ptr port(new BidPortfolio<Material>());

    for (int j = 0; j < reqs.size(); j++) {
      Request<Material>* req = reqs[j];
      double tot_bid = 0;
      for (int k = 0; k < mats.size(); k++) {
        Material::Ptr m = mats[k];
        tot_bid += m->quantity();

        // this fix the problem of the cyclus exchange manager which crashes
        // when a bid with a quantity <=0 is offered.
        if (m->quantity() > cyclus::eps_rsrc()) {
          port->AddBid(req, m, this, exclusive);
        }

        if (tot_bid >= req->target()->quantity()) {
          break;
        }
      }
    }

    double tot_qty = streambufs[commod].quantity();
    cyclus::CapacityConstraint<Material> cc(tot_qty);
    port->AddConstraint(cc);
    ports.insert(port);
  }

  // bid leftovers
  std::vector<Request<Material>*>& reqs = commod_requests[leftover_commod];
  if (reqs.size() > 0 && leftover.quantity() >= cyclus::eps_rsrc()) {
    MatVec mats = leftover.PopN(leftover.count());
    leftover.Push(mats);

    BidPortfolio<Material>::Ptr port(new BidPortfolio<Material>());

    for (int j = 0; j < reqs.size(); j++) {
      Request<Material>* req = reqs[j];
      double tot_bid = 0;
      for (int k = 0; k < mats.size(); k++) {
        Material::Ptr m = mats[k];
        tot_bid += m->quantity();

        // this fix the problem of the cyclus exchange manager which crashes
        // when a bid with a quantity <=0 is offered.
        if (m->quantity() > cyclus::eps_rsrc()) {
          port->AddBid(req, m, this, exclusive);
        }

        if (tot_bid >= req->target()->quantity()) {
          break;
        }
      }
    }

    cyclus::CapacityConstraint<Material> cc(leftover.quantity());
    port->AddConstraint(cc);
    ports.insert(port);
  }

  return ports;
}

void Separations::Tock() {}

bool Separations::CheckDecommissionCondition() {
  if (leftover.count() > 0) {
    return false;
  }

  std::map<std::string, ResBuf<Material> >::iterator it;
  for (it = streambufs.begin(); it != streambufs.end(); ++it) {
    if (it->second.count() > 0) {
      return false;
    }
  }

  return true;
}

void Separations::RecordPosition() {
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

void Separations::Record(std::string name, double val, std::string type) {
  context()
      ->NewDatum("SeparationEvents")
      ->AddVal("AgentId", id())
      ->AddVal("Time", context()->time())
      ->AddVal("Event", name)
      ->AddVal("Value", val)
      ->AddVal("Type", type)
      ->Record();
}

extern "C" cyclus::Agent* ConstructSeparations(cyclus::Context* ctx) {
  return new Separations(ctx);
}

}  // namespace cycamore
