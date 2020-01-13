#ifndef CYCAMORE_SRC_ENRICHMENT_TESTS_
#define CYCAMORE_SRC_ENRICHMENT_TESTS_

#include <gtest/gtest.h>

#include <boost/shared_ptr.hpp>

#include "test_context.h"
#include "env.h"
#include "exchange_context.h"
#include "material.h"

#include "enrichment.h"

namespace cycamore {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class EnrichmentTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  Enrichment* src_facility;
  std::string feed_commod, product_commod, feed_recipe, tails_commod;
  cyclus::Composition::Ptr recipe;
  TestFacility* trader;

  double feed_assay, tails_assay, inv_size, swu_capacity, max_enrich;

  bool order_prefs;

  double reserves;

  virtual void SetUp();
  virtual void TearDown();
  void InitParameters();
  void SetUpSource();
  cyclus::Material::Ptr GetMat(double qty);
  /// @param enr the enrichment percent, i.e. for 5 w/o, enr = 0.05
  cyclus::Material::Ptr GetReqMat(double qty, double enr);
  void DoAddMat(cyclus::Material::Ptr mat);
  cyclus::Material::Ptr DoRequest();
  cyclus::Material::Ptr DoBid(cyclus::Material::Ptr mat);
  cyclus::Material::Ptr DoOffer(cyclus::Material::Ptr mat);
  cyclus::Material::Ptr DoEnrich(cyclus::Material::Ptr mat, double qty);
  /// @param nreqs the total number of requests
  /// @param nvalid the number of requests that are valid
  boost::shared_ptr< cyclus::ExchangeContext<cyclus::Material> >
      GetContext(int nreqs, int nvalid);
};

}  // namespace cycamore

#endif  // CYCAMORE_SRC_ENRICHMENT_FACILITY_TESTS_
