#include "predator.h"

namespace cyclus {

Predator::Predator(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      birth_factor_(0.1),
      age_(0),
      lifespan_(12),
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
  return success_ * context()->n_prototypes(prey_);
}

std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
Predator::GetMatlRequests() {
  using cyclus::CapacityConstraint;
  using cyclus::Material;
  using cyclus::RequestPortfolio;
  using cyclus::Request;

  std::set<RequestPortfolio<Material>::Ptr> ports;
  RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());
  double amt = capacity();
  Material::Ptr mat = cyclus::NewBlankMaterial(amt);

  if (amt > cyclus::eps()) {
    CapacityConstraint<Material> cc(amt);
    port->AddConstraint(cc);

    std::vector<std::string>::const_iterator it;
    port->AddRequest(mat, this, commod_);

    ports.insert(port);
  }  // if amt > eps

  return ports;
}

std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
Predator::GetGenRsrcRequests() {
  using cyclus::CapacityConstraint;
  using cyclus::Product;
  using cyclus::RequestPortfolio;
  using cyclus::Request;

  std::set<RequestPortfolio<Product>::Ptr> ports;
  RequestPortfolio<Product>::Ptr
      port(new RequestPortfolio<Product>());
  double amt = capacity();

  if (amt > cyclus::eps()) {
    CapacityConstraint<Product> cc(amt);
    port->AddConstraint(cc);

    std::vector<std::string>::const_iterator it;
    std::string quality = "";  // not clear what this should be..
    Product::Ptr rsrc = Product::CreateUntracked(amt, quality);
    port->AddRequest(rsrc, this, commod_);

    ports.insert(port);
  }  // if amt > eps

  return ports;
}

void Predator::AcceptMatlTrades(
    const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
    cyclus::Material::Ptr> >& responses) {
  std::vector< std::pair<cyclus::Trade<cyclus::Material>,
  cyclus::Material::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory_.Push(it->second);
  }
}

void Predator::AcceptGenRsrcTrades(
    const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
    cyclus::Product::Ptr> >& responses) {
  std::vector< std::pair<cyclus::Trade<cyclus::Product>,
  cyclus::Product::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory_.Push(it->second);
  }
}


void Predator::Tick(int time) {
  using std::string;
  using std::vector;
  LOG(cyclus::LEV_INFO3, "Predator") << prototype() << " is ticking {";

  double requestAmt = capacity();
  // inform the simulation about what the Predator facility will be requesting
  if (requestAmt > cyclus::eps()) {
    LOG(cyclus::LEV_INFO4, "Predator") << " will request " << requestAmt
        << " units of " << commod_ << ".";
  }
  LOG(cyclus::LEV_INFO3, "Predator") << "}";
}

void Predator::Tock(int time) {
  LOG(cyclus::LEV_INFO3, "Predator") << prototype() << " is tocking {";

  LOG(cyclus::LEV_INFO4, "Predator") << "Predator " << this->id()
                                   << " is holding " << inventory_.quantity()
                                   << " units of material at the close of month "
                                   << time << ".";
  assert(age_ >= 0);
  assert(lifespan_ > 0);
  if (age_ >= lifespan_) {
    LOG(cyclus::LEV_INFO3, "Predator") << prototype() << "is dying of old age}";
    context()->SchedDecom(this);
    LOG(cyclus::LEV_INFO3, "Predator") << "}";
    return;
  }

  LOG(cyclus::LEV_INFO3, "Predator") << prototype() << " ate";

  // give birth if enough food is eaten
  assert(birth_factor_ > 0);
  if (inventory_.quantity() * birth_factor_ > 1) {
    LOG(cyclus::LEV_INFO3, "Prey") << prototype() << " is having children";
    int nchildren = inventory_.quantity() * birth_factor_;
    inventory_.PopQty(nchildren);
    for (int i = 0; i < nchildren; ++i) {
      context()->SchedBuild(this, prototype());
    }
  }

  age_++;  // getting older

  LOG(cyclus::LEV_INFO3, "Predator") << "}";
}

extern "C" cyclus::Agent* ConstructPredator(cyclus::Context* ctx) {
  return new Predator(ctx);
}

}  // namespace cyclus
