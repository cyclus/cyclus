#include "prey.h"

#include <sstream>

namespace cyclus {

Prey::Prey(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      commod_(""),
      recipe_(""),
      capacity_(1),
      killed_(0),
      nchildren_(1),
      birth_freq_(1),
      lifespan_(4),
      age_(0) {}

std::string Prey::str() {
  std::stringstream ss;
  ss << cyclus::Facility::str()
     << " supplies commodity '"
     << commod_ << "' with recipe '"
     << recipe_ << "' at a capacity of "
     << capacity_ << " units per time step ";
  return ss.str();
}

void Prey::DoRegistration() {
  cyclus::Facility::DoRegistration();
  context()->RegisterTrader(this);
}

void Prey::Build(cyclus::Agent* parent) {
  cyclus::Facility::Build();
}

void Prey::Decommission() {
  context()->UnregisterTrader(this);
  cyclus::Facility::Decommission();
}

void Prey::Tick(int time) {
  LOG(cyclus::LEV_INFO3, "Prey") << prototype() << " is ticking {";
  LOG(cyclus::LEV_INFO4, "Prey") << "will offer " << capacity_
                                   << " units of "
                                   << commod_ << ".";
  LOG(cyclus::LEV_INFO3, "Prey") << "}";
}

void Prey::Tock(int time) {
  LOG(cyclus::LEV_INFO3, "Prey") << prototype() << " is tocking {";

  if (killed_) {
    LOG(cyclus::LEV_INFO3, "Prey") << prototype() << " got eaten";
    context()->SchedDecom(this);
    LOG(cyclus::LEV_INFO3, "Prey") << "}";
    return;
  }

  assert(age_ >= 0);
  assert(lifespan_ >= 0);
  if (age_ >= lifespan_) {
    LOG(cyclus::LEV_INFO3, "Prey") << prototype() << "is dying of old age";
    context()->SchedDecom(this);
    LOG(cyclus::LEV_INFO3, "Prey") << "}";
    return;
  }

  assert(birth_freq_ > 0);
  if (age_ % birth_freq_ == 0) {
    LOG(cyclus::LEV_INFO3, "Prey") << prototype() << " is having children";
    for (int i = 0; i < nchildren_; ++i) {
      context()->SchedBuild(this, prototype());
    }
  }

  age_++;  // getting older

  LOG(cyclus::LEV_INFO3, "Prey") << "}";
}

cyclus::Material::Ptr Prey::GetOffer(
    const cyclus::Material::Ptr target) const {
  using cyclus::Material;
  double qty = std::min(target->quantity(), capacity_);
  return Material::CreateUntracked(qty, context()->GetRecipe(recipe_));
}

std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr>
Prey::GetMatlBids(
    const cyclus::CommodMap<cyclus::Material>::type& commod_requests) {
  using cyclus::Bid;
  using cyclus::BidPortfolio;
  using cyclus::CapacityConstraint;
  using cyclus::Material;
  using cyclus::Request;

  std::set<BidPortfolio<Material>::Ptr> ports;

  if (commod_requests.count(commod_) > 0) {
    BidPortfolio<Material>::Ptr port(new BidPortfolio<Material>());

    const std::vector<Request<Material>::Ptr>& requests = commod_requests.at(
        commod_);

    std::vector<Request<Material>::Ptr>::const_iterator it;
    for (it = requests.begin(); it != requests.end(); ++it) {
      const Request<Material>::Ptr req = *it;
      Material::Ptr offer = GetOffer(req->target());
      port->AddBid(req, offer, this);
    }

    CapacityConstraint<Material> cc(capacity_);
    port->AddConstraint(cc);
    ports.insert(port);
  }
  return ports;
}

void Prey::GetMatlTrades(
    const std::vector< cyclus::Trade<cyclus::Material> >& trades,
    std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                          cyclus::Material::Ptr> >& responses) {
  using cyclus::Material;
  using cyclus::Trade;

  double provided = 0;
  double current_capacity = capacity_;
  std::vector< cyclus::Trade<cyclus::Material> >::const_iterator it;
  for (it = trades.begin(); it != trades.end(); ++it) {
    double qty = it->amt;
    current_capacity -= qty;
    provided += qty;
    // @TODO we need a policy on negatives..
    Material::Ptr response = Material::Create(this, qty,
                                              context()->GetRecipe(recipe_));
    responses.push_back(std::make_pair(*it, response));
    LOG(cyclus::LEV_INFO5, "Prey") << prototype() << " just received an order"
                                     << " for " << qty
                                     << " of " << commod_;
  }
  if (cyclus::IsNegative(current_capacity)) {
    std::stringstream ss;
    ss << "is being asked to provide " << provided
       << " but its capacity is " << capacity_ << ".";
    throw cyclus::ValueError(Agent::InformErrorMsg(ss.str()));
  }
  // indicate that this prey was hunted down
  if (capacity_ > current_capacity) killed_ = 1;
}

extern "C" cyclus::Agent* ConstructPrey(cyclus::Context* ctx) {
  return new Prey(ctx);
}

}  // namespace cyclus
