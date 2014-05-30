#include "source.h"

#include <sstream>

namespace cyclus {

Source::Source(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      commod(""),
      recipe_name(""),
      capacity(100) {}

std::string Source::str() {
  std::stringstream ss;
  ss << cyclus::Facility::str()
     << " supplies commodity '"
     << commod << "' with recipe '"
     << recipe_name << "' at a capacity of "
     << capacity << " kg per time step ";
  return ss.str();
}

void Source::Tick() {
  LOG(cyclus::LEV_INFO3, "SrcFac") << prototype() << " is ticking";
  LOG(cyclus::LEV_INFO4, "SrcFac") << "will offer " << capacity
                                   << " kg of "
                                   << commod << ".";
}

void Source::Tock() {
  LOG(cyclus::LEV_INFO3, "SrcFac") << prototype() << " is tocking";
}

cyclus::Material::Ptr Source::GetOffer(
    const cyclus::Material::Ptr target) const {
  using cyclus::Material;
  double qty = std::min(target->quantity(), capacity);
  return Material::CreateUntracked(qty, context()->GetRecipe(recipe_name));
}

std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr>
Source::GetMatlBids(
    cyclus::CommodMap<cyclus::Material>::type& commod_requests) {
  using cyclus::Bid;
  using cyclus::BidPortfolio;
  using cyclus::CapacityConstraint;
  using cyclus::Material;
  using cyclus::Request;

  std::set<BidPortfolio<Material>::Ptr> ports;

  if (commod_requests.count(commod) > 0) {
    BidPortfolio<Material>::Ptr port(new BidPortfolio<Material>());

    std::vector<Request<Material>*>& requests = commod_requests[commod];

    std::vector<Request<Material>*>::iterator it;
    for (it = requests.begin(); it != requests.end(); ++it) {
      Request<Material>* req = *it;
      Material::Ptr offer = GetOffer(req->target());
      port->AddBid(req, offer, this);
    }

    CapacityConstraint<Material> cc(capacity);
    port->AddConstraint(cc);
    ports.insert(port);
  }
  return ports;
}

void Source::GetMatlTrades(
    const std::vector< cyclus::Trade<cyclus::Material> >& trades,
    std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                          cyclus::Material::Ptr> >& responses) {
  using cyclus::Material;
  using cyclus::Trade;

  double provided = 0;
  double current_capacity = capacity;
  std::vector< cyclus::Trade<cyclus::Material> >::const_iterator it;
  for (it = trades.begin(); it != trades.end(); ++it) {
    double qty = it->amt;
    current_capacity -= qty;
    provided += qty;
    // @TODO we need a policy on negatives..
    Material::Ptr response = Material::Create(this, qty,
                                              context()->GetRecipe(recipe_name));
    responses.push_back(std::make_pair(*it, response));
    LOG(cyclus::LEV_INFO5, "SrcFac") << prototype() << " just received an order"
                                     << " for " << qty
                                     << " of " << commod;
  }
  if (cyclus::IsNegative(current_capacity)) {
    std::stringstream ss;
    ss << "is being asked to provide " << provided
       << " but its capacity is " << capacity << ".";
    throw cyclus::ValueError(Agent::InformErrorMsg(ss.str()));
  }
}

extern "C" cyclus::Agent* ConstructSource(cyclus::Context* ctx) {
  return new Source(ctx);
}

}  // namespace cyclus
