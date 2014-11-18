#include "buy_policy.h"

#include "error.h"

#define LG(X) LOG(LEV_##X, "buypol")
#define LGH(X)                                               \
  LOG(LEV_##X, "buypol") << "policy " << name_ << " (agent " \
                         << manager()->id() << "): "

namespace cyclus {
namespace toolkit {

BuyPolicy::BuyPolicy() : Trader(NULL) {
  Warn<EXPERIMENTAL_WARNING>(
      "BuyPolicy is experimental and its API may be subject to change");
}

BuyPolicy::~BuyPolicy() { manager()->context()->UnregisterTrader(this); }

BuyPolicy& BuyPolicy::Init(Agent* manager, ResourceBuff* buf, std::string name,
                           double quantize) {
  manager_ = manager;
  buf_ = buf;
  quantize_ = quantize;
  name_ = name;
  return *this;
}

BuyPolicy& BuyPolicy::Set(std::string commod, Composition::Ptr c, double pref) {
  CommodDetail d;
  d.comp = c;
  d.pref = pref;
  commods_[commod] = d;
  return *this;
}

void BuyPolicy::Start() { manager()->context()->RegisterTrader(this); }

void BuyPolicy::Stop() { manager()->context()->UnregisterTrader(this); }

std::map<Material::Ptr, std::string> BuyPolicy::Commods() {
  return rsrc_commod_;
};

std::set<RequestPortfolio<Material>::Ptr> BuyPolicy::GetMatlRequests() {
  rsrc_commod_.clear();
  std::set<RequestPortfolio<Material>::Ptr> ports;
  double amt = buf_->space();
  if (amt < eps()) {
    return ports;
  }

  double quanta = quantize_;
  bool exclusive = true;
  if (quantize_ < 0) {
    exclusive = false;
    quanta = amt;
  }

  LGH(INFO3) << "requesting " << amt << " kg";

  RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());
  std::map<std::string, CommodDetail>::iterator it;

  std::map<int, std::vector<Request<Material>*> > grps;
  for (it = commods_.begin(); it != commods_.end(); ++it) {
    std::string commod = it->first;
    CommodDetail d = it->second;
    for (int i = 0; i < amt / quanta; i++) {
      LG(INFO4) << "  - one " << amt << " kg request of " << commod;
      Material::Ptr m = Material::CreateUntracked(quanta, d.comp);
      grps[i].push_back(port->AddRequest(m, this, commod, exclusive));
    }
  }

  std::map<int, std::vector<Request<Material>*> >::iterator grpit;
  for (grpit = grps.begin(); grpit != grps.end(); ++grpit) {
    port->AddMutualReqs(grpit->second);
  }
  ports.insert(port);

  return ports;
}

void BuyPolicy::AcceptMatlTrades(
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

void BuyPolicy::AdjustMatlPrefs(PrefMap<Material>::type& prefs) {
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
