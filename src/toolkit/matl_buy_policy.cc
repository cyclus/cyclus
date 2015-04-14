#include "matl_buy_policy.h"

#include <sstream>

#include "error.h"

#define LG(X) LOG(LEV_##X, "buypol")
#define LGH(X)                                               \
  LOG(LEV_##X, "buypol") << "policy " << name_ << " (agent " \
                         << manager()->id() << "): "

namespace cyclus {
namespace toolkit {

MatlBuyPolicy::MatlBuyPolicy() : Trader(NULL) {
  Warn<EXPERIMENTAL_WARNING>(
      "MatlBuyPolicy is experimental and its API may be subject to change");
}

MatlBuyPolicy::~MatlBuyPolicy() {
  if (manager() != NULL) 
    manager()->context()->UnregisterTrader(this);
}

MatlBuyPolicy& MatlBuyPolicy::Init(Agent* manager, ResourceBuff* buf,
                                   std::string name, double quantize,
                                   double fill_to, double req_when_under) {
  Trader::manager_ = manager;
  buf_ = buf;
  if (fill_to > 1)
    fill_to /= buf_->capacity();
  assert(fill_to > 0 && fill_to <= 1.);
  fill_to_ = fill_to;
  if (req_when_under > 1)
    req_when_under /= buf_->capacity();
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
  commod_details_[commod] = d;
  return *this;
}

void MatlBuyPolicy::Start() {
  if (manager() == NULL) {
    std::stringstream ss;
    ss << "No manager set on Buy Policy " << name_;
    throw ValueError(ss.str());
  }
  manager()->context()->RegisterTrader(this);
}

void MatlBuyPolicy::Stop() {
  if (manager() == NULL) {
    std::stringstream ss;
    ss << "No manager set on Buy Policy " << name_;
    throw ValueError(ss.str());
  }
  manager()->context()->UnregisterTrader(this);
}

std::set<RequestPortfolio<Material>::Ptr> MatlBuyPolicy::GetMatlRequests() {
  rsrc_commods_.clear();
  std::set<RequestPortfolio<Material>::Ptr> ports;
  bool make_req = buf_->quantity() < req_when_under_ * buf_->capacity();
  double amt = TotalQty();
  if (!make_req || amt < eps()) {
    return ports;
  }

  bool excl = quantize_ > 0;
  double req_amt = ReqQty();
  int n_req = NReq();
  LGH(INFO3) << "requesting " << amt << " kg via " << n_req << " request(s)";

  // one portfolio for each request
  for (int i = 0; i != n_req; i++) {
    RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());
    std::map<int, std::vector<Request<Material>*> > grps;
    // one request for each commodity
    std::map<std::string, CommodDetail>::iterator it;
    for (it = commod_details_.begin(); it != commod_details_.end(); ++it) {
      std::string commod = it->first;
      CommodDetail d = it->second;
      LG(INFO4) << "  - one " << amt << " kg request of " << commod;
      Material::Ptr m = Material::CreateUntracked(req_amt, d.comp);
      grps[i].push_back(port->AddRequest(m, this, commod, d.pref, excl));
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
  rsrc_commods_.clear();
  for (it = resps.begin(); it != resps.end(); ++it) {
    rsrc_commods_[it->second] = it->first.request->commodity();
    LGH(INFO3) << "got " << it->second->quantity() << " kg of "
               << it->first.request->commodity();
    buf_->Push(it->second);
  }
}

}  // namespace toolkit
}  // namespace cyclus
