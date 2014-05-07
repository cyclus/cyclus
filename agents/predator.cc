#include "predator.h"

#include <math.h>

namespace cyclus {

Predator::Predator(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      birth_factor_(0.1),
      age_(0),
      lifespan_(2),
      success_(0.1),
      capacity_(1) {}

std::string Predator::str() {
  // no info for now. Change later
  return Facility::str();
}

void Predator::DoRegistration() {
  cyclus::Facility::DoRegistration();
  context()->RegisterTrader(this);
}

void Predator::Build(cyclus::Agent* parent) {
  cyclus::Facility::Build();
}

void Predator::Decommission() {
  context()->UnregisterTrader(this);
  cyclus::Facility::Decommission();
}

double Predator::capacity() {
  return capacity_;
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
  double amt = capacity();

  if (amt > cyclus::eps()) {
    port->AddRequest(Product::CreateUntracked(amt, ""), this, commod_);
    port->AddDefaultConstraint();
    ports.insert(port);
  }

  return ports;
}

void Predator::AdjustProductPrefs(
    cyclus::PrefMap<cyclus::Product>::type& prefs) {
  Request<Product>::Ptr req = prefs.begin()->first;
  std::map<Bid<Product>::Ptr, double>::iterator it;
  std::vector<Bid<Product>::Ptr> bids;
  for (it = prefs[req].begin(); it != prefs[req].end(); ++it) {
    bids.push_back(it->first);
  }
  
  std::sort(bids.begin(), bids.end(), Predator::SortById);
  int n_drop = std::floor(prefs[req].size() * (1 - success_));
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

  double amt = capacity();
  // inform the simulation about what the Predator facility will be requesting
  if (amt > cyclus::eps()) {
    LOG(cyclus::LEV_INFO4, "Predator") << " will request " << amt
        << " units of " << commod_ << ".";
  }
  LOG(cyclus::LEV_INFO3, "Predator") << "}";
}

void Predator::GiveBirth() {
  bool policy = dead_ ? birth_and_death_ : true;
  if (consumed_ >= capacity() && policy) {
    int nchildren = std::floor(consumed_ * birth_factor_);
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
