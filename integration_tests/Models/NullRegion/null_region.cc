#include "null_region.h"

using cyclus::NullRegion;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NullRegion::NullRegion(cyclus::Context* ctx)
    : cyclus::RegionModel(ctx),
      cyclus::Model(ctx)
    {};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NullRegion::~NullRegion() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void NullRegion::InitFrom(cyclus::QueryEngine* qe) {
  cyclus::RegionModel::InitFrom(qe);
  qe = qe->QueryElement(ModelImpl());

  // retrieve input data members here. For example :
  // string query = "incommodity";
  // incommodity_ = lexical_cast<double>(qe->getElementContent(query));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* NullRegion::Clone() {
  NullRegion* m = new NullRegion(context());
  m->InitFrom(this);
  return m;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void NullRegion::InitFrom(NullRegion* m) {
  cyclus::RegionModel::InitFrom(m);
  // Initialize nullregion members for a cloned module here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string NullRegion::str() {
  return RegionModel::str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructNullRegion(cyclus::Context* ctx) {
  return new NullRegion(ctx);
}
