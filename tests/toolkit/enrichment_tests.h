#ifndef CYCLUS_TESTS_TOOLKIT_ENRICHMENT_TESTS_H_
#define CYCLUS_TESTS_TOOLKIT_ENRICHMENT_TESTS_H_

#include <gtest/gtest.h>

#include "toolkit/enrichment.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class EnrichmentTests : public ::testing::Test {
 public:
  virtual void SetUp();
  virtual void TearDown();
  void SetEnrichmentParameters();

 protected:
  double feed_, product_, tails_;
  double assay_u_, mass_u_;
  Material::Ptr mat_;
  double feed_qty_, tails_qty_, swu_;
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_TESTS_TOOLKIT_ENRICHMENT_TESTS_H_
