#ifndef CYCLUS_TESTS_RESOURCE_HELPERS_H_
#define CYCLUS_TESTS_RESOURCE_HELPERS_H_

#include "composition.h"
#include "material.h"
#include "product.h"
#include "test_context.h"
#include "test_modules/test_facility.h"
#include "request.h"
#include "bid.h"

namespace test_helpers {

/// @brief just some simple helper functions when dealing with resources and
/// exchanges
static int u235 = 92235;
static double helper_qty = 4.5;
    
static cyclus::Material::Ptr get_mat(int nuc, double qty) {
  cyclus::CompMap cm;
  cm[nuc] = qty;
  cyclus::Composition::Ptr comp = cyclus::Composition::CreateFromMass(cm);
  return cyclus::Material::CreateUntracked(qty, comp);
}

static cyclus::Material::Ptr get_mat() {
  return get_mat(u235, helper_qty);
}

static cyclus::TestContext helper_tc;
static TestFacility* trader = new TestFacility(helper_tc.get());

static cyclus::Request<cyclus::Material>::Ptr get_req(std::string commod = "") {
  return cyclus::Request<cyclus::Material>::Create(get_mat(), trader, commod);
}

static cyclus::Bid<cyclus::Material>::Ptr get_bid() {
  return cyclus::Bid<cyclus::Material>::Ptr(
      cyclus::Bid<cyclus::Material>::Create(get_req(), get_mat(), trader));
}

struct TestConverter : public cyclus::Converter<cyclus::Material> {
  TestConverter() {}
  virtual ~TestConverter() {}
  
  virtual double convert(cyclus::Material::Ptr r,
                         cyclus::Arc const * a = NULL,
                         cyclus::ExchangeTranslationContext<cyclus::Material> const *  ctx = NULL) const {
    return r->quantity() * helper_qty;
  }

  /// @returns true if a dynamic cast succeeds
  virtual bool operator==(Converter<cyclus::Material>& other) const {
    return dynamic_cast<TestConverter*>(&other) != NULL;
  }
};

} // namespace test_helpers

#endif // ifndef CYCLUS_TESTS_RESOURCE_HELPERS_H_
