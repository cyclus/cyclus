// BuildingManagerTests.h
#include <gtest/gtest.h>

#include "Enrichment.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class EnrichmentTests : public ::testing::Test 
{
 protected:
  double feed_, product_, tails_;
  double assay_u_, mass_u_;
  cyclus::mat_rsrc_ptr mat_;

  double feed_qty_, tails_qty_, swu_;

 public:
  virtual void SetUp();  
  virtual void TearDown();
  void SetEnrichmentParameters();
};
