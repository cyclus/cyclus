#include "gisfacisa.h"

namespace cyclus {

GISFacisa::GISFacisa(cyclus::Context* ctx)
    : cyclus::Facility(ctx), capacity(100), cyclus::GIS(0, 0) {}

GISFacisa::GISFacisa(cyclus::Context* ctx, float decimal_lat, float decimal_lon)
    : cyclus::Facility(ctx),
      capacity(100),
      cyclus::GIS(decimal_lat, decimal_lon) {}

std::string GISFacisa::str() {
  // No info for now. Change later
  return Facility::str();
}

std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
GISFacisa::GetMatlRequests() {
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
GISFacisa::GetProductRequests() {
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

void GISFacisa::AcceptMatlTrades(
    const std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                                cyclus::Material::Ptr> >& responses) {
  std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                        cyclus::Material::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory.Push(it->second);
  }
}

void GISFacisa::AcceptProductTrades(
    const std::vector<std::pair<cyclus::Trade<cyclus::Product>,
                                cyclus::Product::Ptr> >& responses) {
  std::vector<std::pair<cyclus::Trade<cyclus::Product>,
                        cyclus::Product::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory.Push(it->second);
  }
}

void GISFacisa::Tick() {
  using std::string;
  using std::vector;
  double request_amt = Capacity();
  // Inform the simulation about what the gisfacisa facility will be requesting

  if (request_amt > cyclus::eps()) {
    for (vector<string>::iterator commod = in_commods.begin();
         commod != in_commods.end(); commod++) {
      LOG(cyclus::LEV_INFO3, "GISFacisa") << prototype() << " will request "
                                          << request_amt << " kg of " << *commod
                                          << ".";
    }
  }
}

void GISFacisa::Tock() {
  // On the tock, the gisfacisa facility doesn't really do much.

  // Maybe someday it will record things.
  // For now, lets just print out what we have at each timestep.
  LOG(cyclus::LEV_INFO3, "GISFacisa")
      << prototype() << " is holding " << inventory.quantity()
      << " units of material at the close of month " << context()->time()
      << ".";
}

extern "C" cyclus::Agent* ConstructGISFacisa(cyclus::Context* ctx) {
  return new GISFacisa(ctx);
}

}  // namespace cyclus
