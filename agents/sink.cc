#include <boost/lexical_cast.hpp>

#include "sink.h"

namespace cyclus {

Sink::Sink(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      capacity_(100){}

void Sink::InitInv(cyclus::Inventories& inv) {
  inventory_.PushAll(inv["inventory"]);
}

cyclus::Inventories Sink::SnapshotInv() {
  cyclus::Inventories invs;
  invs["inventory"] = inventory_.PopN(inventory_.count());
  return invs;
}

std::string Sink::str() {
  // no info for now. Change later
  return Facility::str();
}

std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
Sink::GetMatlRequests() {
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
Sink::GetGenRsrcRequests() {
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

void Sink::AcceptMatlTrades(
    const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
    cyclus::Material::Ptr> >& responses) {
  std::vector< std::pair<cyclus::Trade<cyclus::Material>,
  cyclus::Material::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory_.Push(it->second);
  }
}

void Sink::AcceptGenRsrcTrades(
    const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
    cyclus::Product::Ptr> >& responses) {
  std::vector< std::pair<cyclus::Trade<cyclus::Product>,
  cyclus::Product::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory_.Push(it->second);
  }
}


void Sink::Tick(int time) {
  using std::string;
  using std::vector;
  LOG(cyclus::LEV_INFO3, "SnkFac") << prototype() << " is ticking {";

  double requestAmt = capacity();
  // inform the simulation about what the sink facility will be requesting
  if (requestAmt > cyclus::eps()) {
    for (vector<string>::iterator commod = in_commods_.begin();
         commod != in_commods_.end();
         commod++) {
      LOG(cyclus::LEV_INFO4, "SnkFac") << " will request " << requestAmt
          << " kg of " << *commod << ".";
    }
  }
  LOG(cyclus::LEV_INFO3, "SnkFac") << "}";
}

void Sink::Tock(int time) {
  LOG(cyclus::LEV_INFO3, "SnkFac") << prototype() << " is tocking {";

  // On the tock, the sink facility doesn't really do much.
  // Maybe someday it will record things.
  // For now, lets just print out what we have at each timestep.
  LOG(cyclus::LEV_INFO4, "SnkFac") << "Sink " << this->id()
                                   << " is holding " << inventory_.quantity()
                                   << " units of material at the close of month "
                                   << time << ".";
  LOG(cyclus::LEV_INFO3, "SnkFac") << "}";
}

extern "C" cyclus::Agent* ConstructSink(cyclus::Context* ctx) {
  return new Sink(ctx);
}

} // namespace cyclus
