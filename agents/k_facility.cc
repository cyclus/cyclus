#include "k_facility.h"

#include <limits>
#include <sstream>

#include <boost/lexical_cast.hpp>

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KFacility::KFacility(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      in_commod_(""),
      out_commod_(""),
      recipe_name_(""),
      k_factor_in_(1),
      k_factor_out_(1),
      in_capacity_(100),
      out_capacity_(100) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KFacility::~KFacility() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string KFacility::str() {
  std::stringstream ss;
  ss << cyclus::Facility::str()
     << " supplies commodity '"
     << out_commod_ << "' with recipe '"
     << recipe_name_ << "' at a capacity of "
     << out_capacity_ << " kg per time step "
     << ", changing per step by a factor "
     << k_factor_out_ << "\n"
     << "This facility also requests '"
     << in_commod_ << "' at a capacity of "
     << in_capacity_ << " kg per time step "
     << ", changing per step by a factor "
     << k_factor_in_;
  return ss.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void KFacility::Tick(int time) {
  using std::string;
  using std::vector;
  LOG(cyclus::LEV_INFO3, "SrcFac") << prototype() << " is ticking {";
  LOG(cyclus::LEV_INFO4, "SrcFac") << "will offer " << out_capacity_
                                   << " kg of "
                                   << out_commod_ << ".";
  LOG(cyclus::LEV_INFO3, "SrcFac") << "}";
  current_capacity_ = out_capacity_;  // reset capacity

  LOG(cyclus::LEV_INFO3, "SnkFac") << prototype() << " is ticking {";

  double requestAmt = RequestAmt();
  // inform the simulation about what the sink facility will be requesting
  if (requestAmt > cyclus::eps()) {
    LOG(cyclus::LEV_INFO4, "SnkFac") << " will request " << requestAmt
        << " kg of " << in_commod_ << ".";
  }
  LOG(cyclus::LEV_INFO3, "SnkFac") << "}";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void KFacility::Tock(int time) {
  LOG(cyclus::LEV_INFO3, "SrcFac") << prototype() << " is tocking {";
  LOG(cyclus::LEV_INFO4, "SrcFac") << "KFacility " << this->id()
                                   << " is holding " << inventory_.quantity()
                                   << " units of material at the close of month "
                                   << time << ".";
  LOG(cyclus::LEV_INFO3, "SrcFac") << "}";
  // Update capacity for the next step
  in_capacity_ = in_capacity_ * k_factor_in_;
  out_capacity_ = out_capacity_ * k_factor_out_;
  current_capacity_ = out_capacity_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Material::Ptr KFacility::GetOffer(
    const cyclus::Material::Ptr target) const {
  using cyclus::Material;
  double qty = std::min(target->quantity(), out_capacity_);
  return Material::CreateUntracked(qty, context()->GetRecipe(recipe_name_));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr>
KFacility::GetMatlBids(
    const cyclus::CommodMap<cyclus::Material>::type& commod_requests) {
  using cyclus::Bid;
  using cyclus::BidPortfolio;
  using cyclus::CapacityConstraint;
  using cyclus::Material;
  using cyclus::Request;

  std::set<BidPortfolio<Material>::Ptr> ports;

  if (commod_requests.count(out_commod_) > 0) {
    BidPortfolio<Material>::Ptr port(new BidPortfolio<Material>());

    const std::vector<Request<Material>::Ptr>& requests = commod_requests.at(
        out_commod_);

    std::vector<Request<Material>::Ptr>::const_iterator it;
    for (it = requests.begin(); it != requests.end(); ++it) {
      const Request<Material>::Ptr req = *it;
      Material::Ptr offer = GetOffer(req->target());
      port->AddBid(req, offer, this);
    }

    CapacityConstraint<Material> cc(out_capacity_);
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
    current_capacity_ -= qty;
    provided += qty;
    // @TODO we need a policy on negatives..
    Material::Ptr response = Material::Create(this,
                                              qty,
                                              context()->GetRecipe(recipe_name_));
    responses.push_back(std::make_pair(*it, response));
    LOG(cyclus::LEV_INFO5, "SrcFac") << prototype() << " just received an order"
                                     << " for " << qty
                                     << " of " << out_commod_;
  }
  if (cyclus::IsNegative(current_capacity_)) {
    std::stringstream ss;
    ss << "is being asked to provide " << provided
       << " but its capacity is " <<out_capacity_ << ".";
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

    port->AddRequest(mat, this, in_commod_);

    ports.insert(port);
  }  // if amt > eps

  return ports;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
KFacility::GetGenRsrcRequests() {
  using cyclus::CapacityConstraint;
  using cyclus::Product;
  using cyclus::RequestPortfolio;
  using cyclus::Request;

  std::set<RequestPortfolio<Product>::Ptr> ports;
  RequestPortfolio<Product>::Ptr
      port(new RequestPortfolio<Product>());
  double amt = RequestAmt();

  if (amt > cyclus::eps()) {
    CapacityConstraint<Product> cc(amt);
    port->AddConstraint(cc);

    std::string quality = "";  // not clear what this should be..
    Product::Ptr rsrc = Product::CreateUntracked(amt, quality);
    port->AddRequest(rsrc, this, in_commod_);

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
    inventory_.Push(it->second);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void KFacility::AcceptGenRsrcTrades(
    const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
    cyclus::Product::Ptr> >& responses) {
  std::vector< std::pair<cyclus::Trade<cyclus::Product>,
  cyclus::Product::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory_.Push(it->second);
  }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Agent* ConstructKFacility(cyclus::Context* ctx) {
  return new KFacility(ctx);
}

}  // namespace cyclus
