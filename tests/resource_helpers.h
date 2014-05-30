#ifndef CYCLUS_TESTS_RESOURCE_HELPERS_H_
#define CYCLUS_TESTS_RESOURCE_HELPERS_H_

#include "composition.h"
#include "material.h"
#include "product.h"

using cyclus::Arc;
using cyclus::CompMap;
using cyclus::Composition;
using cyclus::Converter;
using cyclus::Material;
using cyclus::ExchangeTranslationContext;

namespace test_helpers {

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

}  // namespace test_helpers

#endif  // CYCLUS_TESTS_RESOURCE_HELPERS_H_
