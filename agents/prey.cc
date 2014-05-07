#include "prey.h"

#include <sstream>

namespace cyclus {

Prey::Prey(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      commod_(""),
      dead_(0),
      nchildren_(1),
      birth_freq_(1),
      age_(0) {}

std::string Prey::str() {
  std::stringstream ss;
  ss << cyclus::Facility::str()
     << " supplies commodity '"<< commod_;
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
  LOG(cyclus::LEV_INFO3, "Prey") << name() << " is ticking {";
  LOG(cyclus::LEV_INFO4, "Prey") << "will offer " << 1
                                   << " units of "
                                   << commod_ << ".";
  LOG(cyclus::LEV_INFO3, "Prey") << "}";
}

void Prey::GiveBirth() {
  bool policy = dead_ ? birth_and_death_ : true;
  if (age_ % birth_freq_ == 0 && policy) {
    LOG(cyclus::LEV_INFO3, "Prey") << name() << " is having children";
    for (int i = 0; i < nchildren_; ++i) {
      context()->SchedBuild(NULL, prototype());
    }
  }
}

void Prey::Tock(int time) {
  LOG(cyclus::LEV_INFO3, "Prey") << name() << " is tocking {";
    
  if (dead_) {
    LOG(cyclus::LEV_INFO3, "Prey") << name() << " got eaten";
    context()->SchedDecom(this);
    LOG(cyclus::LEV_INFO3, "Prey") << "}";
  }
  
  GiveBirth();
  
  age_++;  // getting older

  LOG(cyclus::LEV_INFO3, "Prey") << "}";
}

std::set<cyclus::BidPortfolio<cyclus::Product>::Ptr>
Prey::GetProductBids(
    const cyclus::CommodMap<cyclus::Product>::type& commod_requests) {
  using cyclus::Bid;
  using cyclus::BidPortfolio;
  using cyclus::CapacityConstraint;
  using cyclus::Product;
  using cyclus::Request;

  std::set<BidPortfolio<Product>::Ptr> ports;

  if (commod_requests.count(commod_) > 0) {
    BidPortfolio<Product>::Ptr port(new BidPortfolio<Product>());
    const std::vector<Request<Product>::Ptr>& requests =
        commod_requests.at(commod_);
    std::vector<Request<Product>::Ptr>::const_iterator it;
    for (it = requests.begin(); it != requests.end(); ++it) {
      // offer one wabbit
      port->AddBid(*it, Product::CreateUntracked(1, ""), this);
    }
    CapacityConstraint<Product> cc(1); // only 1 wabbit!
    port->AddConstraint(cc);
    ports.insert(port);
  }
  return ports;
}

void Prey::GetProductTrades(
    const std::vector< cyclus::Trade<cyclus::Product> >& trades,
    std::vector<std::pair<cyclus::Trade<cyclus::Product>,
                          cyclus::Product::Ptr> >& responses) {
  using cyclus::Product;
  using cyclus::Trade;

  double provided = 0;
  double current_capacity = 1; // only 1 wabbit!
  std::vector< cyclus::Trade<cyclus::Product> >::const_iterator it;
  for (it = trades.begin(); it != trades.end(); ++it) {
    double qty = it->amt;
    current_capacity -= qty;
    provided += qty;
    Product::Ptr response = Product::Create(this, qty, "");
    responses.push_back(std::make_pair(*it, response));
    LOG(cyclus::LEV_INFO5, "Prey") << name() << " just received an order"
                                     << " for " << qty
                                     << " of " << commod_;
  }
  if (cyclus::IsNegative(current_capacity)) {
    std::stringstream ss;
    ss << "is being asked to provide " << provided
       << " but its capacity is " << 1<< ".";
    throw cyclus::ValueError(Agent::InformErrorMsg(ss.str()));
  }

  if (provided > 0) dead_ = 1; // They came from... behind! 
}

extern "C" cyclus::Agent* ConstructPrey(cyclus::Context* ctx) {
  return new Prey(ctx);
}

}  // namespace cyclus
