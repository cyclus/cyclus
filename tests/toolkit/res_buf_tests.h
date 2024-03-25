#ifndef CYCLUS_TESTS_TOOLKIT_RESOURCE_BUFF_TESTS_H_
#define CYCLUS_TESTS_TOOLKIT_RESOURCE_BUFF_TESTS_H_

#include <gtest/gtest.h>

#include "cyc_limits.h"
#include "error.h"
#include "logger.h"
#include "product.h"
#include "composition.h"
#include "material.h"
#include "toolkit/res_buf.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ProductBufTest : public ::testing::Test {
 protected:
  Product::Ptr mat1_, mat2_;
  double mass1, mass2;
  ProdVec mats;

  ResBuf<Product> store_;  // default constructed mat store
  ResBuf<Product> filled_store_;

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
      filled_store_.Push(mat1_);
      filled_store_.Push(mat2_);
    } catch (std::exception err) {
      FAIL() << "An exception was thrown in the fixture SetUp.";
    }
  }
};

class MaterialBufTest : public ::testing::Test {
 protected:

  ResBuf<Material> mat_store_;  // default constructed mat store
  ResBuf<Material> bulk_store_ = ResBuf<Material>(true);

  Nuc sr89_, fe59_;
  Material::Ptr mat1a_, mat1b_, mat2a_, mat2b_, mat3_;
  Composition::Ptr test_comp1_, test_comp2_, test_comp3_;

  double cap_;

  virtual void SetUp() {
    try {

      sr89_ = 380890000;
      fe59_ = 260590000;

      CompMap v, w;
      v[sr89_] = 1;
      test_comp1_ = Composition::CreateFromMass(v);

      w[fe59_] = 2;
      test_comp2_ = Composition::CreateFromMass(w);

      w[sr89_] = 1;
      test_comp3_ = Composition::CreateFromMass(w);

      double mat_size = 5 * units::g;

      mat1a_ = Material::CreateUntracked(mat_size, test_comp1_);
      mat1b_ = Material::CreateUntracked(mat_size, test_comp1_);
      mat2a_ = Material::CreateUntracked(mat_size, test_comp2_);
      mat2b_ = Material::CreateUntracked(mat_size, test_comp2_);
      mat3_ = Material::CreateUntracked(mat_size, test_comp3_);

      cap_ = 10 * mat_size;

      mat_store_.capacity(cap_);
      bulk_store_.capacity(cap_);

    } catch (std::exception err) {
      FAIL() << "An exception was thrown in the fixture SetUp.";
    }
  }
};


}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_TESTS_TOOLKIT_RESOURCE_BUFF_TESTS_H_
