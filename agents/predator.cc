#include "predator.h"

#include <math.h>

namespace cyclus {

Predator::Predator(cyclus::Context* ctx) : cyclus::Facility(ctx) {}

void Predator::EnterNotify() {
  cyclus::Facility::EnterNotify();
  context()->RegisterTrader(this);
}

void Predator::Decommission() {
  context()->UnregisterTrader(this);
  cyclus::Facility::Decommission();
}

std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
Predator::GetProductRequests() {
  using cyclus::CapacityConstraint;
  using cyclus::Product;
  using cyclus::RequestPortfolio;
  using cyclus::Request;

  std::set<RequestPortfolio<Product>::Ptr> ports;
  RequestPortfolio<Product>::Ptr
      port(new RequestPortfolio<Product>());

  if (age_ % hunt_freq_  == 0) {
    port->AddRequest(Product::CreateUntracked(hunt_cap_, ""), this, commod_);
    port->AddDefaultConstraint();
    ports.insert(port);
  }

  return ports;
}

void Predator::AdjustProductPrefs(
    cyclus::PrefMap<cyclus::Product>::type& prefs) {
  if (prefs.size() == 0)
    return;
  Request<Product>* req = prefs.begin()->first;
  std::map<Bid<Product>::Ptr, double>::iterator it;
  std::vector<Bid<Product>::Ptr> bids;
  for (it = prefs[req].begin(); it != prefs[req].end(); ++it) {
    bids.push_back(it->first);
  }
  
  std::sort(bids.begin(), bids.end(), Predator::SortById);
  int nprey = context()->n_prototypes(prey_);
  int npred = context()->n_prototypes(prototype());
  double factor = (hunt_factor_ && nprey < npred) ? double(nprey) / npred : 1;
  int n_drop = std::floor(prefs[req].size() * (1 - success_ * factor));
  LOG(cyclus::LEV_INFO3, "Predator") << name()
                                     << " removing " << n_drop << " bids "
                                     << " out of " << prefs[req].size();
  for (int i = 0; i != n_drop; i++) {
    prefs[req][bids[i]] = -1;
  }
}

void Predator::AcceptProductTrades(
    const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
    cyclus::Product::Ptr> >& responses) {
  std::vector< std::pair<cyclus::Trade<cyclus::Product>,
  cyclus::Product::Ptr> >::const_iterator it;
  
  for (it = responses.begin(); it != responses.end(); ++it) {
    LOG(cyclus::LEV_INFO3, "Predator") << name() << " ate";
    consumed_ += it->second->quantity();
  }
  LOG(cyclus::LEV_INFO3, "Predator") << name() << " consumed "
                                     << consumed_;
}

void Predator::Tick(int time) {
  LOG(cyclus::LEV_INFO3, "Predator") << name() << " is ticking {";

  // inform the simulation about what the Predator facility will be requesting
  if (age_ % hunt_freq_ == 0) {
    LOG(cyclus::LEV_INFO4, "Predator")
        << " will request " << hunt_cap_
        << " units of " << commod_ << ".";
  }
  LOG(cyclus::LEV_INFO3, "Predator") << "}";
}

void Predator::GiveBirth() {
  bool policy = dead_ ? birth_and_death_ : true;
  if (consumed_ >= full_ && policy) {
    int nchildren = nchildren_;
    LOG(cyclus::LEV_INFO3, "Predator") << name() << " is having "
                                       << nchildren << " children";
    for (int i = 0; i < nchildren; ++i) {
      context()->SchedBuild(NULL, prototype());
    }
    consumed_ = 0;
  }
}

void Predator::Tock(int time) {
  LOG(cyclus::LEV_INFO3, "Predator") << name() << " is tocking {";

  if (age_ >= lifespan_) {
    LOG(cyclus::LEV_INFO3, "Predator") << name() << " is dying of old age";
    dead_ = 1;
    context()->SchedDecom(this);
  }
  
  GiveBirth();
  
  age_++;  // getting older

  LOG(cyclus::LEV_INFO3, "Predator") << "}";
}

extern "C" cyclus::Agent* ConstructPredator(cyclus::Context* ctx) {
  return new Predator(ctx);
}

}  // namespace cyclus
