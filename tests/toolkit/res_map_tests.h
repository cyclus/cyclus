#ifndef CYCLUS_TESTS_TOOLKIT_RES_MAP_TESTS_H_
#define CYCLUS_TESTS_TOOLKIT_RES_MAP_TESTS_H_

#include <gtest/gtest.h>

#include "cyc_limits.h"
#include "error.h"
#include "logger.h"
#include "product.h"
#include "toolkit/res_map.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ResMapTest : public ::testing::Test {
 protected:
  Product::Ptr mat1_, mat2_;
  double mass1, mass2;
  ProdVec mats;

  ResMap<std::string, Product> store_;  // default constructed mat store
  ResMap<std::string, Product> filled_store_;

  double neg_cap, zero_cap, cap, low_cap;
  double exact_qty;  // mass in filled_store_
  double exact_qty_under;  // mass in filled_store - 0.9*eps_rsrc()
  double exact_qty_over;  // mass in filled_store + 0.9*eps_rsrc()
  double over_qty;   // mass in filled_store - 1.1*eps_rsrc()
  double under_qty;  // mass in filled_store + 1.1*eps_rsrc()
  double overeps, undereps;

  virtual void SetUp() {
    try {
      mass1 = 111;
      mat1_ = Product::CreateUntracked(mass1, "bananas");
      mass2 = 222;
      mat2_ = Product::CreateUntracked(mass2, "bananas");
      mats.push_back(mat1_);
      mats.push_back(mat2_);

      neg_cap = -1;
      zero_cap = 0;
      cap = mat1_->quantity() + mat2_->quantity() +
            1;  // should be higher than mat1+mat2 masses
      low_cap = mat1_->quantity() + mat2_->quantity() -
                1;  // should be lower than mat1_mat2 masses

      undereps = 0.9 * eps_rsrc();
      overeps = 1.1 * eps_rsrc();
      exact_qty = mat1_->quantity();
      exact_qty_under = exact_qty - undereps;
      exact_qty_over = exact_qty + undereps;
      under_qty = exact_qty - overeps;
      over_qty = exact_qty + overeps;

      filled_store_.capacity(cap);
      filled_store_["mat1"] = mat1_;
      filled_store_["mat2"] = mat2_;
    } catch (std::exception err) {
      FAIL() << "An exception was thrown in the fixture SetUp.";
    }
  }
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_TESTS_TOOLKIT_RES_MAP_TESTS_H_
