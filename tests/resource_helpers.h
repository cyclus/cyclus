#ifndef CYCLUS_TESTS_RESOURCE_HELPERS_H_
#define CYCLUS_TESTS_RESOURCE_HELPERS_H_

#include "composition.h"
#include "material.h"

/// @brief just some simple helper functions when dealing with resources and
/// exchanges

static int helper_iso = 92235;
static double helper_qty = 4.5;

static cyclus::Material::Ptr get_mat(int iso, double qty) {
  cyclus::CompMap cm;
  cm[iso] = qty;
  cyclus::Composition::Ptr comp = cyclus::Composition::CreateFromMass(cm);
  return cyclus::Material::CreateUntracked(qty, comp);
}

static cyclus::Material::Ptr get_mat() {
  return get_mat(helper_iso, helper_qty);
}

#endif // ifndef CYCLUS_TESTS_RESOURCE_HELPERS_H_
