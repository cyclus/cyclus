#include "k_facility.h"

#include <limits>
#include <sstream>

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KFacility::KFacility(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      in_commod(""),
      out_commod(""),
      recipe_name(""),
      k_factor_in(1),
      k_factor_out(1),
      in_capacity(100),
      out_capacity(100) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KFacility::~KFacility() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string KFacility::str() {
  std::stringstream ss;
  ss << cyclus::Facility::str()
     << " supplies commodity '"
     << out_commod << "' with recipe '"
     << recipe_name << "' at a capacity of "
     << out_capacity << " kg per time step "
     << ", changing per step by a factor "
     << k_factor_out << "\n"
     << "This facility also requests '"
     << in_commod << "' at a capacity of "
     << in_capacity << " kg per time step "
     << ", changing per step by a factor "
     << k_factor_in;
  return ss.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void KFacility::Tick(int time) {
  using std::string;
  using std::vector;
  LOG(cyclus::LEV_INFO3, "KFac") << prototype() << " is ticking";
  LOG(cyclus::LEV_INFO4, "KFac") << "will offer " << out_capacity
                                   << " kg of "
                                   << out_commod << ".";
  current_capacity = out_capacity;  // reset capacity

  double request_amt = RequestAmt();
  // inform the simulation about what the sink facility will be requesting
  if (request_amt > cyclus::eps()) {
    LOG(cyclus::LEV_INFO4, "KFac") << " will request " << request_amt
        << " kg of " << in_commod << ".";
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void KFacility::Tock(int time) {
  LOG(cyclus::LEV_INFO3, "KFac") << prototype() << " is tocking";
  LOG(cyclus::LEV_INFO4, "KFac") << "KFacility " << this->id()
                                   << " is holding " << inventory.quantity()
                                   << " units of material at the close of month "
                                   << time << ".";
  // Update capacity for the next step
  in_capacity = in_capacity * k_factor_in;
  out_capacity = out_capacity * k_factor_out;
  current_capacity = out_capacity;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Material::Ptr KFacility::GetOffer(
    const cyclus::Material::Ptr target) const {
  using cyclus::Material;
  double qty = std::min(target->quantity(), out_capacity);
  return Material::CreateUntracked(qty, context()->GetRecipe(recipe_name));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr>
KFacility::GetMatlBids(
    cyclus::CommodMap<cyclus::Material>::type& commod_requests) {
  using cyclus::Bid;
  using cyclus::BidPortfolio;
  using cyclus::CapacityConstraint;
  using cyclus::Material;
  using cyclus::Request;

  std::set<BidPortfolio<Material>::Ptr> ports;

  if (commod_requests.count(out_commod) > 0) {
    BidPortfolio<Material>::Ptr port(new BidPortfolio<Material>());

    std::vector<Request<Material>*>& requests = commod_requests.at(
        out_commod);

    std::vector<Request<Material>*>::iterator it;
    for (it = requests.begin(); it != requests.end(); ++it) {
      Request<Material>* req = *it;
      Material::Ptr offer = GetOffer(req->target());
      port->AddBid(req, offer, this);
    }

    CapacityConstraint<Material> cc(out_capacity);
    port->AddConstraint(cc);
    ports.insert(port);
  }
  return ports;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void KFacility::GetMatlTrades(
    const std::vector< cyclus::Trade<cyclus::Material> >& trades,
    std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                          cyclus::Material::Ptr> >& responses) {
  using cyclus::Material;
  using cyclus::Trade;

  double provided = 0;
  std::vector< cyclus::Trade<cyclus::Material> >::const_iterator it;
  for (it = trades.begin(); it != trades.end(); ++it) {
    double qty = it->amt;
    current_capacity -= qty;
    provided += qty;
    // @TODO we need a policy on negatives..
    Material::Ptr response = Material::Create(this, qty,
                                              context()->GetRecipe(recipe_name));
    responses.push_back(std::make_pair(*it, response));
    LOG(cyclus::LEV_INFO5, "KFac") << prototype() << " just received an order"
                                     << " for " << qty
                                     << " of " << out_commod;
  }
  if (cyclus::IsNegative(current_capacity)) {
    std::stringstream ss;
    ss << "is being asked to provide " << provided
       << " but its capacity is " <<out_capacity << ".";
    throw cyclus::ValueError(Agent::InformErrorMsg(ss.str()));
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
KFacility::GetMatlRequests() {
  using cyclus::CapacityConstraint;
  using cyclus::Material;
  using cyclus::RequestPortfolio;
  using cyclus::Request;

  std::set<RequestPortfolio<Material>::Ptr> ports;
  RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());
  double amt = RequestAmt();
  Material::Ptr mat = cyclus::NewBlankMaterial(amt);

  if (amt > cyclus::eps()) {
    CapacityConstraint<Material> cc(amt);
    port->AddConstraint(cc);

    port->AddRequest(mat, this, in_commod);

    ports.insert(port);
  }  // if amt > eps

  return ports;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
KFacility::GetProductRequests() {
  using cyclus::CapacityConstraint;
  using cyclus::Product;
  using cyclus::RequestPortfolio;
  using cyclus::Request;

  std::set<RequestPortfolio<Product>::Ptr> ports;
  RequestPortfolio<Product>::Ptr port(new RequestPortfolio<Product>());
  double amt = RequestAmt();

  if (amt > cyclus::eps()) {
    CapacityConstraint<Product> cc(amt);
    port->AddConstraint(cc);

    std::string quality = "";  // not clear what this should be..
    Product::Ptr rsrc = Product::CreateUntracked(amt, quality);
    port->AddRequest(rsrc, this, in_commod);

    ports.insert(port);
  }  // if amt > eps

  return ports;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void KFacility::AcceptMatlTrades(
    const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
    cyclus::Material::Ptr> >& responses) {
  std::vector< std::pair<cyclus::Trade<cyclus::Material>,
  cyclus::Material::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory.Push(it->second);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void KFacility::AcceptProductTrades(
    const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
    cyclus::Product::Ptr> >& responses) {
  std::vector< std::pair<cyclus::Trade<cyclus::Product>,
  cyclus::Product::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory.Push(it->second);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Agent* ConstructKFacility(cyclus::Context* ctx) {
  return new KFacility(ctx);
}

}  // namespace cyclus
