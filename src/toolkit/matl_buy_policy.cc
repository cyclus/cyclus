#include "matl_buy_policy.h"

#include "error.h"

#define LG(X) LOG(LEV_##X, "buypol")
#define LGH(X)                                               \
  LOG(LEV_##X, "buypol") << "policy " << name_ << " (agent " \
                         << manager_->id() << "): "

namespace cyclus {
namespace toolkit {

MatlBuyPolicy::MatlBuyPolicy() : Trader(NULL) {
  Warn<EXPERIMENTAL_WARNING>(
      "MatlBuyPolicy is experimental and its API may be subject to change");
}

MatlBuyPolicy::~MatlBuyPolicy() {
  if (manager_)
    manager_->context()->UnregisterTrader(this);
}

MatlBuyPolicy& MatlBuyPolicy::Init(Agent* manager, ResourceBuff* buf,
                                   std::string name, double quantize,
                                   double fill_to, double req_when_under) {
  manager_ = manager;
  buf_ = buf;
  assert(fill_to > 0 && fill_to <= 1.);
  fill_to_ = fill_to;
  assert(req_when_under > 0 && req_when_under <= 1.);
  req_when_under_ = req_when_under;
  assert(quantize != 0);
  quantize_ = quantize;
  name_ = name;
  return *this;
}

MatlBuyPolicy& MatlBuyPolicy::Set(std::string commod, Composition::Ptr c,
                                  double pref) {
  CommodDetail d;
  d.comp = c;
  d.pref = pref;
  commods_[commod] = d;
  return *this;
}

void MatlBuyPolicy::Start() { manager_->context()->RegisterTrader(this); }

void MatlBuyPolicy::Stop() { manager_->context()->UnregisterTrader(this); }

std::map<Material::Ptr, std::string> MatlBuyPolicy::Commods() {
  return rsrc_commod_;
};

std::set<RequestPortfolio<Material>::Ptr> MatlBuyPolicy::GetMatlRequests() {
  rsrc_commod_.clear();
  std::set<RequestPortfolio<Material>::Ptr> ports;
  bool make_req = req_when_under_ * buf_->capacity() < buf_->quantity();
  double amt = TotalQty();
  if (!make_req || amt < eps()) {
    return ports;
  }

  bool excl = quantize_ > 0;
  double req_amt = ReqQty();
  double n_req = NReq();
  LGH(INFO3) << "requesting " << amt << " kg via " << n_req << " request(s)";

  // one portfolio for each request
  for (int i = 0; i < n_req; i++) {
    RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());
    std::map<int, std::vector<Request<Material>*> > grps;
    // one request for each commodity
    std::map<std::string, CommodDetail>::iterator it;
    for (it = commods_.begin(); it != commods_.end(); ++it) {
      std::string commod = it->first;
      CommodDetail d = it->second;
      LG(INFO4) << "  - one " << amt << " kg request of " << commod;
      Material::Ptr m = Material::CreateUntracked(req_amt, d.comp);
      grps[i].push_back(port->AddRequest(m, this, commod, excl));
    }

    // if there's more than one commodity, then make them mutal
    if (grps.size() > 1) {
      std::map<int, std::vector<Request<Material>*> >::iterator grpit;
      for (grpit = grps.begin(); grpit != grps.end(); ++grpit) {
        port->AddMutualReqs(grpit->second);
      }
    }
    ports.insert(port);
  }
  
  return ports;
}

void MatlBuyPolicy::AcceptMatlTrades(
    const std::vector<std::pair<Trade<Material>, Material::Ptr> >& resps) {
  std::vector<std::pair<Trade<Material>, Material::Ptr> >::const_iterator it;
  rsrc_commod_.clear();
  for (it = resps.begin(); it != resps.end(); ++it) {
    rsrc_commod_[it->second] = it->first.request->commodity();
    LGH(INFO3) << "got " << it->second->quantity() << " kg of "
               << it->first.request->commodity();
    buf_->Push(it->second);
  }
}

void MatlBuyPolicy::AdjustMatlPrefs(PrefMap<Material>::type& prefs) {
  PrefMap<Material>::type::iterator it;
  for (it = prefs.begin(); it != prefs.end(); ++it) {
    Request<Material>* r = it->first;
    double pref = commods_[r->commodity()].pref;
    LGH(INFO5) << "setting prefs for " << r->target()->quantity()
               << " kg bid for " << r->commodity() << " to " << pref;
    std::map<Bid<Material>*, double>::iterator it2;
    std::map<Bid<Material>*, double> bids = it->second;
    for (it2 = bids.begin(); it2 != bids.end(); ++it2) {
      Bid<Material>* b = it2->first;
      prefs[r][b] = pref;
    }
  }
}

}  // namespace toolkit
}  // namespace cyclus
