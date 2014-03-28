#ifndef CYCLUS_TESTS_RESOURCE_HELPERS_H_
#define CYCLUS_TESTS_RESOURCE_HELPERS_H_

#include "composition.h"
#include "material.h"
#include "product.h"
#include "test_context.h"
#include "test_modules/test_facility.h"
#include "request.h"
#include "bid.h"

using cyclus::Arc;
using cyclus::Bid;
using cyclus::Request;
using cyclus::CompMap;
using cyclus::Composition;
using cyclus::Converter;
using cyclus::Material;
using cyclus::ExchangeTranslationContext;
using cyclus::TestContext;

namespace rsrc_helpers {

/// @brief just some simple helper functions when dealing with resources and
/// exchanges
static int u235 = 92235;
static double helper_qty = 4.5;
    
static Material::Ptr get_mat(int nuc, double qty) {
  CompMap cm;
  cm[nuc] = qty;
  Composition::Ptr comp = Composition::CreateFromMass(cm);
  return Material::CreateUntracked(qty, comp);
}

static Material::Ptr get_mat() {
  return get_mat(u235, helper_qty);
}

static TestContext helper_tc;
static TestFacility* trader = new TestFacility(helper_tc.get());

static Request<Material>::Ptr get_req(std::string commod = "") {
  return Request<Material>::Create(get_mat(), trader, commod);
}

static Bid<Material>::Ptr get_bid() {
  return Bid<Material>::Ptr(
      Bid<Material>::Create(get_req(), get_mat(), trader));
}

struct TestConverter : public Converter<Material> {
  TestConverter() {}
  virtual ~TestConverter() {}
  
  virtual double convert(
      Material::Ptr r,
      Arc const * a = NULL,
      ExchangeTranslationContext<Material> const * ctx = NULL) const {
    return r->quantity() * helper_qty;
  }

  /// @returns true if a dynamic cast succeeds
  virtual bool operator==(Converter<Material>& other) const {
    return dynamic_cast<TestConverter*>(&other) != NULL;
  }
};

} // namespace rsrc_helpers

#endif // ifndef CYCLUS_TESTS_RESOURCE_HELPERS_H_
