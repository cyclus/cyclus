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
      commod_price_(0),
      k_factor_in_(1),
      k_factor_out_(1),
      in_capacity_(100),
      out_capacity_(100) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KFacility::~KFacility() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// std::string KFacility::schema() {
//   return
//     "  <element name =\"setup\">              \n"
//     "    <element name =\"incommodity\">      \n"
//     "      <text/>                            \n"
//     "    </element>                           \n"
//     "    <element name =\"in_capacity\">      \n"
//     "      <data type=\"double\"/>            \n"
//     "    </element>                           \n"
//     "    <element name =\"k_factor_in\">      \n"
//     "      <data type=\"double\"/>            \n"
//     "    </element>                           \n"
//     "    <element name =\"outcommodity\">     \n"
//     "      <text/>                            \n"
//     "    </element>                           \n"
//     "    <element name=\"recipe\">            \n"
//     "      <data type=\"string\"/>            \n"
//     "    </element>                           \n"
//     "    <element name =\"out_capacity\">     \n"
//     "      <data type=\"double\"/>            \n"
//     "    </element>                           \n"
//     "    <element name =\"k_factor_out\">     \n"
//     "      <data type=\"double\"/>            \n"
//     "    </element>                           \n"
//     "  </element>                             \n";
// }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// void KFacility::InfileToDb(cyclus::QueryEngine* qe, cyclus::DbInit di) {
//   cyclus::FacilityModel::InfileToDb(qe, di);
//   qe = qe->QueryElement("model/" + model_impl());
//
//   using std::string;
//   using boost::lexical_cast;
//
//   cyclus::QueryEngine* setup = qe->QueryElement("setup");
//
//   // Facility output configuraitons
//   recipe(setup->GetString("recipe"));
//
//   string data = setup->GetString("outcommodity");
//   commodity(data);
//   cyclus::Commodity commod(data);
//   cyclus::CommodityProducer::AddCommodity(commod);
//
//   double cap = lexical_cast<double>(setup->GetString("out_capacity"));
//   cyclus::CommodityProducer::SetCapacity(commod, cap);
//   capacity(cap);
//
//   double k = lexical_cast<double>(setup->GetString("k_factor_out"));
//   k_factor_out(k);
//
//   // Facility input configurations
//   AddCommodity(setup->GetString("incommodity"));
//   k_factor_in(lexical_cast<double>(setup->GetString("k_factor_in")));
//   in_capacity_ = lexical_cast<double>(setup->GetString("in_capacity"));
//
//   // di.NewDatum("Info")
//   //     ->AddVal("recipe", recipe)
//   //     ->AddVal("out_commod", out_commod)
//   //     ->AddVal("capacity", cap)
//   //     ->AddVal("curr_capacity", cap)
//   //     ->Record();
// }

// void KFacility::InitFrom(cyclus::QueryBackend* b) {
//   cyclus::FacilityModel::InitFrom(b);
//   // cyclus::QueryResult qr = b->Query("Info", NULL);
//   // recipe_name_ = qr.GetVal<std::string>("recipe");
//   // out_commod_ = qr.GetVal<std::string>("out_commod");
//   // capacity_ = qr.GetVal<double>("capacity");
//   // current_capacity_ = qr.GetVal<double>("curr_capacity");
//
//   // cyclus::Commodity commod(out_commod_);
//   // cyclus::CommodityProducer::AddCommodity(commod);
//   // cyclus::CommodityProducer::SetCapacity(commod, capacity_);
// }
//
// void KFacility::Snapshot(cyclus::DbInit di) {
//   cyclus::FacilityModel::Snapshot(di);
// //  di.NewDatum("Info")
// //      ->AddVal("recipe", recipe_name_)
// //      ->AddVal("out_commod", out_commod_)
// //      ->AddVal("capacity", capacity_)
// //      ->AddVal("curr_capacity", current_capacity_)
// //      ->Record();
// }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// void KFacility::InitFrom(cyclus::QueryEngine* qe) {
//   cyclus::FacilityModel::InitFrom(qe);
//   qe = qe->QueryElement("model/" + ModelImpl());
//
//   using std::string;
//   using std::numeric_limits;
//   using boost::lexical_cast;
//   cyclus::QueryEngine* setup = qe->QueryElement("setup");
//
//   // Facility output configuraitons
//   recipe(setup->GetElementContent("recipe"));
//
//   string data = setup->GetElementContent("outcommodity");
//   commodity(data);
//   cyclus::Commodity commod(data);
//   cyclus::CommodityProducer::AddCommodity(commod);
//
//
//   double cap = lexical_cast<double>(setup->GetElementContent("out_capacity"));
//   cyclus::CommodityProducer::SetCapacity(commod, cap);
//   capacity(cap);
//
//   double k = lexical_cast<double>(setup->GetElementContent("k_factor_out"));
//   k_factor_out(k);
//
//   // Facility input configurations
//   AddCommodity(setup->GetElementContent("incommodity"));
//   k_factor_in(lexical_cast<double>(setup->GetElementContent("k_factor_in")));
//   in_capacity_ = lexical_cast<double>(setup->GetElementContent("in_capacity"));
// }
//
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
// cyclus::Model* KFacility::Clone() {
//   KFacility* m = new KFacility(context());
//   m->InitFrom(this);
//   return m;
// }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// void KFacility::InitFrom(KFacility* m) {
//   FacilityModel::InitFrom(m);
//   commodity(m->commodity());
//   in_commod_ = m->in_commod_;
//   capacity(m->capacity());
//   in_capacity_ = m->in_capacity_;
//   recipe(m->recipe());
//   k_factor_in(m->k_factor_in());
//   k_factor_out(m->k_factor_out());
//   in_commods_ = m->in_commods_;
//   CopyProducedCommoditiesFrom(m);
//   current_capacity_ = capacity();
// }

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

    std::vector<std::string>::const_iterator it;
    for (it = in_commods_.begin(); it != in_commods_.end(); ++it) {
      port->AddRequest(mat, this, *it);
    }

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

    std::vector<std::string>::const_iterator it;
    for (it = in_commods_.begin(); it != in_commods_.end(); ++it) {
      std::string quality = "";  // not clear what this should be..
      std::string units = "";  // not clear what this should be..
      Product::Ptr rsrc = Product::CreateUntracked(amt,
                                                                   quality);
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
