#include "sink_facility.h"

using cyclus::SinkFacility;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SinkFacility::SinkFacility(cyclus::Context* ctx)
    : cyclus::FacilityModel(ctx) {};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SinkFacility::~SinkFacility() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string SinkFacility::schema() {
  return
      "  <element name =\"input\">               \n"
      "    <element name =\"incommodity\"/>      \n"
      "    <element name =\"input_capacity\">    \n"
      "      <data type=\"double\"/> \n"
      "    </element>                            \n"
      "  </element>                              \n";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkFacility::InitFrom(cyclus::QueryEngine* qe) {
  cyclus::FacilityModel::InitFrom(qe);
  qe = qe->QueryElement("model/" + ModelImpl());

  cyclus::QueryEngine* input = qe->QueryElement("input");
  incommodity_ = input->GetElementContent("incommodity", 0);
  capacity_ = input->GetElementContent<double>("input_capacity", 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* SinkFacility::Clone() {
  SinkFacility* m = new SinkFacility(context());
  m->InitFrom(this);
  return m;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// not expected to be cloned
void SinkFacility::InitFrom(SinkFacility* m) {
  cyclus::FacilityModel::InitFrom(m);
  // Initialize sinkfacility members for a cloned module here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string SinkFacility::str() {
  // no info for now. Change later
  return FacilityModel::str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
SinkFacility::GetMatlRequests() {
  using cyclus::CapacityConstraint;
  using cyclus::Material;
  using cyclus::RequestPortfolio;
  using cyclus::Request;

  std::set<RequestPortfolio<Material>::Ptr> ports;
  RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());
  double amt = RequestAmt();
  Material::Ptr mat = Material::CreateBlank(amt);

  if (amt > cyclus::eps()) {
    CapacityConstraint<Material> cc(amt);
    port->AddConstraint(cc);

    std::vector<std::string>::const_iterator it;
    for (it = in_commods_.begin(); it != in_commods_.end(); ++it) {
      port->AddRequest(mat, this, *it);
    }

    ports.insert(port);
  } // if amt > eps

  return ports;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<cyclus::RequestPortfolio<cyclus::GenericResource>::Ptr>
SinkFacility::GetGenRsrcRequests() {
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
      std::string quality = ""; // not clear what this should be..
      std::string units = ""; // not clear what this should be..
      GenericResource::Ptr rsrc = GenericResource::CreateUntracked(amt,
                                                                   quality,
                                                                   units);
      port->AddRequest(rsrc, this, *it);
    }

    ports.insert(port);
  } // if amt > eps

  return ports;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkFacility::AcceptMatlTrades(
    const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
    cyclus::Material::Ptr> >& responses) {
  std::vector< std::pair<cyclus::Trade<cyclus::Material>,
  cyclus::Material::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory_.Push(it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkFacility::AcceptGenRsrcTrades(
    const std::vector< std::pair<cyclus::Trade<cyclus::GenericResource>,
    cyclus::GenericResource::Ptr> >& responses) {
  std::vector< std::pair<cyclus::Trade<cyclus::GenericResource>,
  cyclus::GenericResource::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory_.Push(it->second);
  }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkFacility::Tick(int time) {
  using std::string;
  using std::vector:

      LOG(cyclus::LEV_INFO3, "SnkFac") << FacName() << " is ticking {";

  double requestAmt = request_;
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkFacility::Tock(int time) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructSinkFacility(cyclus::Context* ctx) {
  return new SinkFacility(ctx);
}
