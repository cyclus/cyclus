#include "predator.h"

namespace cyclus {

Predator::Predator(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      bufsize_(0),
      birth_freq_(0),
      age_(0),
      capture_prob_(0),
      for_sale_(0),
      capacity_(100){}

void Predator::InitInv(cyclus::Inventories& inv) {
  inventory_.PushAll(inv["inventory"]);
}

cyclus::Inventories Predator::SnapshotInv() {
  cyclus::Inventories invs;
  invs["inventory"] = inventory_.PopN(inventory_.count());
  return invs;
}

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
  Material::Ptr m;
  m = Material::Create(this, bufsize_, context()->GetRecipe(inrecipe_));
  inbuf_.Push(m);
}

void Predator::Decommission() {
  context()->UnregisterTrader(this);
  cyclus::Facility::Decommission();
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
    for (it = in_commods_.begin(); it != in_commods_.end(); ++it) {
      port->AddRequest(mat, this, *it);
    }

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
    for (it = in_commods_.begin(); it != in_commods_.end(); ++it) {
      std::string quality = "";  // not clear what this should be..
      Product::Ptr rsrc = Product::CreateUntracked(amt, quality);
      port->AddRequest(rsrc, this, *it);
    }

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
    for (vector<string>::iterator commod = in_commods_.begin();
         commod != in_commods_.end();
         commod++) {
      LOG(cyclus::LEV_INFO4, "Predator") << " will request " << requestAmt
          << " kg of " << *commod << ".";
    }
  }
  LOG(cyclus::LEV_INFO3, "Predator") << "}";
}

void Predator::Tock(int time) {
  LOG(cyclus::LEV_INFO3, "Predator") << prototype() << " is tocking {";

  // On the tock, the Predator facility doesn't really do much.
  // Maybe someday it will record things.
  // For now, lets just print out what we have at each timestep.
  LOG(cyclus::LEV_INFO4, "Predator") << "Predator " << this->id()
                                   << " is holding " << inventory_.quantity()
                                   << " units of material at the close of month "
                                   << time << ".";
  LOG(cyclus::LEV_INFO3, "Predator") << "}";
}

extern "C" cyclus::Agent* ConstructPredator(cyclus::Context* ctx) {
  return new Predator(ctx);
}

}  // namespace cyclus
