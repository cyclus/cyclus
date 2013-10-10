// building_manager_tests.h
#include <gtest/gtest.h>

#include "enrichment.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class EnrichmentTests : public ::testing::Test 
{
 protected:
  double feed_, product_, tails_;
  double assay_u_, mass_u_;
  cyclus::Material::Ptr mat_;

  double feed_qty_, tails_qty_, swu_;

 public:
  virtual void SetUp();  
  virtual void TearDown();
  void SetEnrichmentParameters();
};
