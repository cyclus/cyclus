#include "gisfac.h"

namespace cyclus {

GISFac::GISFac(cyclus::Context* ctx) : cyclus::Facility(ctx), capacity(100) {}

std::string GISFac::str() {
  // No info for now. Change later
  return Facility::str();
}

std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
GISFac::GetMatlRequests() {
  using cyclus::CapacityConstraint;
  using cyclus::Material;
  using cyclus::RequestPortfolio;
  using cyclus::Request;

  std::set<RequestPortfolio<Material>::Ptr> ports;
  RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());
  double amt = Capacity();

  Material::Ptr mat = cyclus::NewBlankMaterial(amt);
  if (!recipe_name.empty()) {
    Composition::Ptr c = context()->GetRecipe(recipe_name);
    mat = Material::CreateUntracked(amt, c);
  }

  if (amt > cyclus::eps()) {
    CapacityConstraint<Material> cc(amt);
    port->AddConstraint(cc);

    std::vector<std::string>::const_iterator it;
    for (it = in_commods.begin(); it != in_commods.end(); ++it) {
      port->AddRequest(mat, this, *it);
    }

    ports.insert(port);
  }  // if amt > eps

  return ports;
}

std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
GISFac::GetProductRequests() {
  using cyclus::CapacityConstraint;
  using cyclus::Product;
  using cyclus::RequestPortfolio;
  using cyclus::Request;

  std::set<RequestPortfolio<Product>::Ptr> ports;
  RequestPortfolio<Product>::Ptr port(new RequestPortfolio<Product>());
  double amt = Capacity();

  if (amt > cyclus::eps()) {
    CapacityConstraint<Product> cc(amt);
    port->AddConstraint(cc);

    std::vector<std::string>::const_iterator it;
    for (it = in_commods.begin(); it != in_commods.end(); ++it) {
      std::string quality = "";  // not clear what this should be..
      Product::Ptr rsrc = Product::CreateUntracked(amt, quality);
      port->AddRequest(rsrc, this, *it);
    }

    ports.insert(port);
  }  // if amt > eps

  return ports;
}

void GISFac::AcceptMatlTrades(
    const std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                                cyclus::Material::Ptr> >& responses) {
  std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                        cyclus::Material::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory.Push(it->second);
  }
}

void GISFac::AcceptProductTrades(
    const std::vector<std::pair<cyclus::Trade<cyclus::Product>,
                                cyclus::Product::Ptr> >& responses) {
  std::vector<std::pair<cyclus::Trade<cyclus::Product>,
                        cyclus::Product::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory.Push(it->second);
  }
}

void GISFac::Tick() {
  using std::string;
  using std::vector;
  double request_amt = Capacity();
  // Inform the simulation about what the gisfac facility will be requesting
  if (request_amt > cyclus::eps()) {
    for (vector<string>::iterator commod = in_commods.begin();
         commod != in_commods.end(); commod++) {
      LOG(cyclus::LEV_INFO3, "GISFac") << prototype() << " will request "
                                       << request_amt << " kg of " << *commod
                                       << ".";
    }
  }
}

void GISFac::Tock() {
  // On the tock, the gisfac facility doesn't really do much.
  // Maybe someday it will record things.
  // For now, lets just print out what we have at each timestep.
  LOG(cyclus::LEV_INFO3, "GISFac")
      << prototype() << " is holding " << inventory.quantity()
      << " units of material at the close of month " << context()->time()
      << ".";
}

extern "C" cyclus::Agent* ConstructGISFac(cyclus::Context* ctx) {
  return new GISFac(ctx);
}

}  // namespace cyclus
