#include "sell_policy.h"

#include "error.h"

#define LG(X) LOG(LEV_##X, "selpol")
#define LGH(X)                                               \
  LOG(LEV_##X, "selpol") << "policy " << name_ << " (agent " \
                         << manager()->id() << "): "
namespace cyclus {
namespace toolkit {

SellPolicy::SellPolicy() : Trader(NULL) {
  Warn<EXPERIMENTAL_WARNING>(
      "SellPolicy is experimental and its API may be subject to change");
}

SellPolicy::~SellPolicy() { manager()->context()->UnregisterTrader(this); }

SellPolicy& SellPolicy::Init(Agent* manager, ResourceBuff* buf,
                             std::string name, double quantize) {
  manager_ = manager;
  quantize_ = quantize;
  buf_ = buf;
  name_ = name;
  return *this;
}

SellPolicy& SellPolicy::Set(std::string commod) {
  commods_.insert(commod);
  return *this;
}

void SellPolicy::Start() { manager()->context()->RegisterTrader(this); }

void SellPolicy::Stop() { manager()->context()->UnregisterTrader(this); }

std::set<BidPortfolio<Material>::Ptr> SellPolicy::GetMatlBids(
    CommodMap<Material>::type& commod_requests) {
  std::set<BidPortfolio<Material>::Ptr> ports;
  if (buf_->empty()) {
    return ports;
  } else if (buf_->quantity() < eps()) {
    return ports;
  }

  BidPortfolio<Material>::Ptr port(new BidPortfolio<Material>());

  LGH(INFO3) << "bidding out " << buf_->quantity() << " kg";

  bool exclusive = true;
  if (quantize_ < 0) {
    exclusive = false;
  }

  std::set<std::string>::iterator it;
  for (it = commods_.begin(); it != commods_.end(); ++it) {
    std::string commod = *it;
    if (commod_requests.count(commod) < 1) {
      continue;
    }

    const std::vector<Request<Material>*>& requests =
        commod_requests.at(commod);

    std::vector<Request<Material>*>::const_iterator it;
    for (it = requests.begin(); it != requests.end(); ++it) {
      Request<Material>* req = *it;
      double qty = req->target()->quantity();
      qty = std::min(qty, buf_->quantity());

      if (quantize_ < 0) {
        Material::Ptr m = buf_->Pop<Material>();
        buf_->Push(m);
        Material::Ptr offer = Material::CreateUntracked(qty, m->comp());
        port->AddBid(req, offer, this, exclusive);
        LG(INFO4) << "  - bid " << qty << " kg on a request for " << commod;
      } else {
        for (int i = 0; i < (int)(qty / quantize_); i++) {
          Material::Ptr m = buf_->Pop<Material>();
          buf_->Push(m);
          Material::Ptr offer = Material::CreateUntracked(quantize_, m->comp());
          port->AddBid(req, offer, this, exclusive);
          LG(INFO4) << "  - bid " << quantize_ << " kg on a request for "
                    << commod;
        }
      }
    }
  }

  CapacityConstraint<Material> cc(buf_->quantity());
  port->AddConstraint(cc);
  ports.insert(port);
  return ports;
}

void SellPolicy::GetMatlTrades(
    const std::vector<Trade<Material> >& trades,
    std::vector<std::pair<Trade<Material>, Material::Ptr> >& responses) {
  std::vector<Trade<Material> >::const_iterator it;
  for (it = trades.begin(); it != trades.end(); ++it) {
    double qty = it->amt;
    LGH(INFO4) << " sending " << qty << " kg of " << it->request->commodity();
    std::vector<Material::Ptr> man =
        ResCast<Material>(buf_->PopQty(qty, buf_->quantity() * 1e-12));
    for (int i = 1; i < man.size(); ++i) {
      man[0]->Absorb(man[i]);
    }
    responses.push_back(std::make_pair(*it, man[0]));
  }
}

}  // namespace toolkit
}  // namespace cyclus
