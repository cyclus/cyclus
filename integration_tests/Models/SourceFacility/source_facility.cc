#include "source_facility.h"

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
SimpleSource::SimpleSource(cyclus::Context* ctx)
    : cyclus::FacilityModel(ctx),
      out_commod_(""),
      recipe_name_(""),
      commod_price_(0),
      capacity_(std::numeric_limits<double>::max()) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SimpleSource::~SimpleSource() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string SimpleSource::schema() {
  return
    "  <element name =\"output\">          \n"
    "    <ref name=\"outcommodity\"/>      \n"
    "    <optional>                        \n"
    "      <ref name=\"output_capacity\"/> \n"
    "    </optional>                       \n"
    "    <element name=\"recipe\">         \n"
    "      <data type=\"string\"/>         \n"
    "    </element>                        \n"
    "  </element>                          \n";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SimpleSource::InitFrom(cyclus::QueryEngine* qe) {
  cyclus::FacilityModel::InitFrom(qe);
  qe = qe->QueryElement("model/" + ModelImpl());

  using std::string;
  using std::numeric_limits;
  using boost::lexical_cast;
  cyclus::QueryEngine* output = qe->QueryElement("output");

  recipe(output->GetElementContent("recipe"));

  string data = output->GetElementContent("outcommodity");
  commodity(data);
  cyclus::Commodity commod(data);
  cyclus::CommodityProducer::AddCommodity(commod);

  double cap = cyclus::GetOptionalQuery<double>(output,
                                                "output_capacity",
                                                numeric_limits<double>::max());
  cyclus::CommodityProducer::SetCapacity(commod, cap);
  capacity(cap);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string SimpleSource::str() {
  std::stringstream ss;
  ss << cyclus::FacilityModel::str()
     << " supplies commodity '"
     << out_commod_ << "' with recipe '"
     << recipe_name_ << "' at a capacity of "
     << capacity_ << " kg per time step ";
  return ss.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* SimpleSource::Clone() {
  SimpleSource* m = new SimpleSource(context());
  m->InitFrom(this);
  return m;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SimpleSource::InitFrom(SimpleSource* m) {
  FacilityModel::InitFrom(m);
  commodity(m->commodity());
  capacity(m->capacity());
  recipe(m->recipe());
  CopyProducedCommoditiesFrom(m);
  current_capacity_ = capacity();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SimpleSource::Tick(int time) {
  LOG(cyclus::LEV_INFO3, "SrcFac") << FacName() << " is ticking {";
  LOG(cyclus::LEV_INFO4, "SrcFac") << "will offer " << capacity_
                                   << " kg of "
                                   << out_commod_ << ".";
  LOG(cyclus::LEV_INFO3, "SrcFac") << "}";
  current_capacity_ = capacity_; // reset capacity
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SimpleSource::Tock(int time) {
  LOG(cyclus::LEV_INFO3, "SrcFac") << FacName() << " is tocking {";
  LOG(cyclus::LEV_INFO3, "SrcFac") << "}";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Material::Ptr SimpleSource::GetOffer(
    const cyclus::Material::Ptr target) const {
  using cyclus::Material;
  double qty = std::min(target->quantity(), capacity_);
  return Material::CreateUntracked(qty, context()->GetRecipe(recipe_name_));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr>
SimpleSource::GetMatlBids(
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

    CapacityConstraint<Material> cc(capacity_);
    port->AddConstraint(cc);
    ports.insert(port);
  }
  return ports;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SimpleSource::GetMatlTrades(
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
                                     << " of " << out_commod_;
  }
  if (cyclus::IsNegative(current_capacity_)) {
    std::stringstream ss;
    ss << "is being asked to provide " << provided
       << " but its capacity is " << capacity_ << ".";
    throw cyclus::ValueError(Model::InformErrorMsg(ss.str()));
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructSimpleSource(cyclus::Context* ctx) {
  return new SimpleSource(ctx);
}

}  // namespace cyclus
