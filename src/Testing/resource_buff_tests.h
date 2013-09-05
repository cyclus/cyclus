
#include <gtest/gtest.h>

#include "generic_resource.h"
#include "resource_buff.h"
#include "error.h"
#include "cyc_limits.h"
#include "logger.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ResourceBuffTest : public ::testing::Test {
 protected:
  cyclus::Resource::Ptr mat1_, mat2_;
  double mass1, mass2;
  cyclus::Manifest mats;

  cyclus::ResourceBuff store_; // default constructed mat store
  cyclus::ResourceBuff filled_store_;

  double neg_cap, zero_cap, cap, low_cap;
  double exact_qty; // mass in filled_store_
  double exact_qty_under; // mass in filled_store - 0.9*cyclus::eps_rsrc()
  double exact_qty_over; // mass in filled_store + 0.9*cyclus::eps_rsrc()
  double over_qty;  // mass in filled_store - 1.1*cyclus::eps_rsrc()
  double under_qty; // mass in filled_store + 1.1*cyclus::eps_rsrc()
  double overeps, undereps;

  virtual void SetUp() {
    using cyclus::Resource;
    using cyclus::GenericResource;
    try {
      mass1 = 111;
      mat1_ = GenericResource::Create(mass1, "bananas", "kg")->Clone();
      mass2 = 222;
      mat2_ = GenericResource::Create(mass2, "bananas", "kg")->Clone();
      mats.push_back(mat1_);
      mats.push_back(mat2_);

      neg_cap = -1;
      zero_cap = 0;
      cap = mat1_->quantity() + mat2_->quantity() +
            1; // should be higher than mat1+mat2 masses
      low_cap = mat1_->quantity() + mat2_->quantity() -
                1; // should be lower than mat1_mat2 masses

      undereps = 0.9 * cyclus::eps_rsrc();
      overeps = 1.1 * cyclus::eps_rsrc();
      exact_qty = mat1_->quantity();
      exact_qty_under = exact_qty - undereps;
      exact_qty_over = exact_qty + undereps;
      under_qty = exact_qty - overeps;
      over_qty = exact_qty + overeps;

      filled_store_.SetCapacity(cap);
      filled_store_.PushOne(mat1_);
      filled_store_.PushOne(mat2_);
    } catch (std::exception err) {
      FAIL() << "An exception was thrown in the fixture SetUp.";
    }
  }
};
