#include "matl_sell_policy.h"

#include "error.h"

#define LG(X) LOG(LEV_##X, "selpol")
#define LGH(X)                                               \
  LOG(LEV_##X, "selpol") << "policy " << name_ << " (agent " \
                         << Trader::manager()->id() << "): "
namespace cyclus {
namespace toolkit {

MatlSellPolicy::MatlSellPolicy() : Trader(NULL) {
  Warn<EXPERIMENTAL_WARNING>(
      "MatlSellPolicy is experimental and its API may be subject to change");
}

MatlSellPolicy::~MatlSellPolicy() {
  if (manager() != NULL)
    manager()->context()->UnregisterTrader(this);
}

MatlSellPolicy& MatlSellPolicy::Init(Agent* manager, ResourceBuff* buf,
                                     std::string name, double quantize,
                                     double throughput, bool ignore_comp) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  assert(quantize != 0);
  quantize_ = quantize;
  throughput_ = throughput;
  ignore_comp_ = ignore_comp;

  LGH(INFO3) << " configured with "
             << " quantize: " << quantize_
             << " throughput: " << throughput_
             << " ignore_comp: " << std::boolalpha << ignore_comp_;
  
  return *this;
}

MatlSellPolicy& MatlSellPolicy::Set(std::string commod) {
  commods_.insert(commod);
  return *this;
}

void MatlSellPolicy::Start() {
  if (manager() == NULL) {
    std::stringstream ss;
    ss << "No manager set on Sell Policy " << name_;
    throw ValueError(ss.str());
  }
  manager()->context()->RegisterTrader(this);
}

void MatlSellPolicy::Stop() {
  if (manager() == NULL) {
    std::stringstream ss;
    ss << "No manager set on Sell Policy " << name_;
    throw ValueError(ss.str());
  }
  manager()->context()->UnregisterTrader(this);
}

std::set<BidPortfolio<Material>::Ptr> MatlSellPolicy::GetMatlBids(
    CommodMap<Material>::type& commod_requests) {
  std::set<BidPortfolio<Material>::Ptr> ports;
  if (buf_->empty() || buf_->quantity() < eps())
    return ports;
  
  double bcap = buf_->quantity();
  bool excl = quantize_ > 0;
  double limit = excl ?                                                 \
                 bcap * static_cast<int>(std::floor(bcap / quantize_)) : bcap;
  limit = std::min(throughput_, limit);
  LGH(INFO3) << "bidding out " << limit << " kg";
  
  BidPortfolio<Material>::Ptr port(new BidPortfolio<Material>());
  CapacityConstraint<Material> cc(limit);
  port->AddConstraint(cc);
  ports.insert(port);
  
  std::string commod;
  Request<Material>* req;
  Material::Ptr m, offer;
  double qty;
  int nbids;
  std::set<std::string>::iterator sit;
  std::vector<Request<Material>*>::const_iterator rit;
  for (sit = commods_.begin(); sit != commods_.end(); ++sit) {
    commod = *sit;
    if (commod_requests.count(commod) < 1) {
      continue;
    }

    const std::vector<Request<Material>*>& requests =
        commod_requests.at(commod);
    for (rit = requests.begin(); rit != requests.end(); ++rit) {
      req = *rit;
      qty = std::min(req->target()->quantity(), buf_->quantity());
      nbids = excl ? static_cast<int>(std::floor(qty / quantize_)) : 1;
      qty = excl ? quantize_ : qty;
      for (int i = 0; i < nbids; i++) {
        m = buf_->Pop<Material>();
        buf_->Push(m);
        offer = Material::CreateUntracked(qty, m->comp());
        port->AddBid(req, offer, this, excl);
        LG(INFO4) << "  - bid " << qty << " kg on a request for " << commod;
      }
    }
  }
  return ports;
}

void MatlSellPolicy::GetMatlTrades(
    const std::vector<Trade<Material> >& trades,
    std::vector<std::pair<Trade<Material>, Material::Ptr> >& responses) {
  Composition::Ptr c;
  std::vector<Trade<Material> >::const_iterator it;
  for (it = trades.begin(); it != trades.end(); ++it) {
    double qty = it->amt;
    LGH(INFO4) << " sending " << qty << " kg of " << it->request->commodity();
    std::vector<Material::Ptr> man =
        ResCast<Material>(buf_->PopQty(qty, buf_->quantity() * 1e-12));
    if (ignore_comp_) {
      c = it->request->target()->comp();
      man[0]->Transmute(c);
      for (int i = 1; i < man.size(); ++i) {
        man[i]->Transmute(c);
      }
    }
    for (int i = 1; i < man.size(); ++i) {
      man[0]->Absorb(man[i]);
    }
    responses.push_back(std::make_pair(*it, man[0]));
  }
}

}  // namespace toolkit
}  // namespace cyclus
