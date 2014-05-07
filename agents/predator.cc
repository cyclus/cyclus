#include "predator.h"

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
  // Query the number of the prey
  // Determine the request

  //return success_ * context()->n_prototypes(prey_);
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

void Predator::Tock(int time) {
  LOG(cyclus::LEV_INFO3, "Predator") << name() << " is tocking {";

  // give birth if enough food is eaten
  if (consumed_ * birth_factor_ >= 1) {
    LOG(cyclus::LEV_INFO3, "Prey") << name() << " is having children";
    int nchildren = consumed_ * birth_factor_;
    for (int i = 0; i < nchildren; ++i) {
      context()->SchedBuild(this, prototype());
    }
    consumed_ = 0;
  }

  if (age_ >= lifespan_) {
    LOG(cyclus::LEV_INFO3, "Predator") << name() << "is dying of old age}";
    context()->SchedDecom(this);
    LOG(cyclus::LEV_INFO3, "Predator") << "}";
    return;
  }

  age_++;  // getting older

  LOG(cyclus::LEV_INFO3, "Predator") << "}";
}

extern "C" cyclus::Agent* ConstructPredator(cyclus::Context* ctx) {
  return new Predator(ctx);
}

}  // namespace cyclus
