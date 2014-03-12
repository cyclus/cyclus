#include "k_facility.h"

#include <sstream>
#include <limits>

#include <boost/lexical_cast.hpp>

#include "capacity_constraint.h"
#include "cyc_limits.h"
#include "context.h"
#include "error.h"
#include "logger.h"
#include "request.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KFacility::KFacility(cyclus::Context* ctx)
    : cyclus::FacilityModel(ctx),
      commod_(""),
      recipe_name_(""),
      commod_price_(0),
      k_factor_(1),
      capacity_(100) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KFacility::~KFacility() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string KFacility::schema() {
  return
    "  <element name =\"setup\">              \n"
    "    <element name =\"commodity\">        \n"
    "      <text/>                            \n"
    "    </element>                           \n"
    "    <element name=\"recipe\">            \n"
    "      <data type=\"string\"/>            \n"
    "    </element>                           \n"
    "    <element name =\"init_capacity\">    \n"
    "      <data type=\"double\"/>            \n"
    "    </element>                           \n"
    "    <element name =\"k_factor\">         \n"
    "      <data type=\"double\"/>            \n"
    "    </element>                           \n"
    "  </element>                             \n";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void KFacility::InitFrom(cyclus::QueryEngine* qe) {
  cyclus::FacilityModel::InitFrom(qe);
  qe = qe->QueryElement("model/" + ModelImpl());

  using std::string;
  using std::numeric_limits;
  using boost::lexical_cast;
  cyclus::QueryEngine* setup = qe->QueryElement("setup");

  recipe(setup->GetElementContent("recipe"));

  string data = setup->GetElementContent("commodity");
  commodity(data);
  cyclus::Commodity commod(data);
  cyclus::CommodityProducer::AddCommodity(commod);
  KFacility::AddCommodity(data);

  double cap = lexical_cast<double>(setup->GetElementContent("init_capacity"));
  cyclus::CommodityProducer::SetCapacity(commod, cap);
  capacity(cap);

  double k = lexical_cast<double>(setup->GetElementContent("k_factor"));
  k_factor(k);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string KFacility::str() {
  std::stringstream ss;
  ss << cyclus::FacilityModel::str()
     << " supplies and requests commodity '"
     << commod_ << "' with recipe '"
     << recipe_name_ << "' at a capacity of "
     << capacity_ << " kg per time step "
     << ", changing per step by a factor "
     << k_factor_;
  return ss.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* KFacility::Clone() {
  KFacility* m = new KFacility(context());
  m->InitFrom(this);
  return m;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void KFacility::InitFrom(KFacility* m) {
  FacilityModel::InitFrom(m);
  commodity(m->commodity());
  capacity(m->capacity());
  recipe(m->recipe());
  k_factor(m->k_factor());
  CopyProducedCommoditiesFrom(m);
  current_capacity_ = capacity();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void KFacility::Tick(int time) {
  using std::string;
  using std::vector;
  LOG(cyclus::LEV_INFO3, "SrcFac") << FacName() << " is ticking {";
  LOG(cyclus::LEV_INFO4, "SrcFac") << "will offer " << capacity_
                                   << " kg of "
                                   << commod_ << ".";
  LOG(cyclus::LEV_INFO3, "SrcFac") << "}";
  current_capacity_ = capacity_; // reset capacity

  LOG(cyclus::LEV_INFO3, "SnkFac") << FacName() << " is ticking {";

  double requestAmt = RequestAmt();
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void KFacility::Tock(int time) {
  LOG(cyclus::LEV_INFO3, "SrcFac") << FacName() << " is tocking {";
  LOG(cyclus::LEV_INFO4, "SrcFac") << "KFacility " << this->id()
                                   << " is holding " << inventory_.quantity()
                                   << " units of material at the close of month "
                                   << time << ".";
  LOG(cyclus::LEV_INFO3, "SrcFac") << "}";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Material::Ptr KFacility::GetOffer(
    const cyclus::Material::Ptr target) const {
  using cyclus::Material;
  double qty = std::min(target->quantity(), capacity_);
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
    LOG(cyclus::LEV_INFO5, "SrcFac") << name() << " just received an order"
                                     << " for " << qty
                                     << " of " << commod_;
  }
  if (cyclus::IsNegative(current_capacity_)) {
    std::stringstream ss;
    ss << "is being asked to provide " << provided
       << " but its capacity is " << capacity_ << ".";
    throw cyclus::ValueError(Model::InformErrorMsg(ss.str()));
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
  std::cout<< amt << std::endl;
  Material::Ptr mat = Material::CreateBlank(amt);

  if (amt > cyclus::eps()) {
    CapacityConstraint<Material> cc(amt);
    port->AddConstraint(cc);

    std::vector<std::string>::const_iterator it;
    for (it = in_commods_.begin(); it != in_commods_.end(); ++it) {
      std::cout<< *it << std::endl;
      port->AddRequest(mat, this, *it);
    }

    ports.insert(port);
  }  // if amt > eps

  return ports;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<cyclus::RequestPortfolio<cyclus::GenericResource>::Ptr>
KFacility::GetGenRsrcRequests() {
  using cyclus::CapacityConstraint;
  using cyclus::GenericResource;
  using cyclus::RequestPortfolio;
  using cyclus::Request;

  std::set<RequestPortfolio<GenericResource>::Ptr> ports;
  RequestPortfolio<GenericResource>::Ptr
      port(new RequestPortfolio<GenericResource>());
  double amt = RequestAmt();

  if (amt > cyclus::eps()) {
    CapacityConstraint<GenericResource> cc(amt);
    port->AddConstraint(cc);

    std::vector<std::string>::const_iterator it;
    for (it = in_commods_.begin(); it != in_commods_.end(); ++it) {
      std::string quality = "";  // not clear what this should be..
      std::string units = "";  // not clear what this should be..
      GenericResource::Ptr rsrc = GenericResource::CreateUntracked(amt,
                                                                   quality,
                                                                   units);
      port->AddRequest(rsrc, this, *it);
    }

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
    const std::vector< std::pair<cyclus::Trade<cyclus::GenericResource>,
    cyclus::GenericResource::Ptr> >& responses) {
  std::vector< std::pair<cyclus::Trade<cyclus::GenericResource>,
  cyclus::GenericResource::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory_.Push(it->second);
  }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructKFacility(cyclus::Context* ctx) {
  return new KFacility(ctx);
}

}  // namespace cyclus
