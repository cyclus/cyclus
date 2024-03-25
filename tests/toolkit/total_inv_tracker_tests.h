#ifndef CYCLUS_TESTS_TOOLKIT_TOTAL_INV_TRACKER_TESTS_H
#define CYCLUS_TESTS_TOOLKIT_TOTAL_INV_TRACKER_TESTS_H

#include <gtest/gtest.h>

#include "error.h"
#include "toolkit/total_inv_tracker.h"
#include "toolkit/res_buf.h"
#include "composition.h"
#include "material.h"
#include "test_agents/test_facility.h"

namespace cyclus {
namespace toolkit {

class TotalInvTrackerTest : public ::testing::Test {   
  protected:
    cyclus::Timer ti;
    cyclus::Recorder rec;
    cyclus::Context* ctx;
    TestFacility* fac;

    TotalInvTracker empty_tracker_;
    TotalInvTracker single_tracker_;
    TotalInvTracker multi_tracker_;
    ResBuf<Material> buf0_, buf1_, buf2_, buf3_;
    double max_inv_size_;
    Material::Ptr mat0_, mat1_, mat2_;
    double qty1_, qty2_;

  virtual void SetUp() {
    try {
        ctx = new cyclus::Context(&ti, &rec);
        fac = new TestFacility(ctx);

        CompMap cm;
        cm[1001] = 1.0;
        qty1_ = 10;
        qty2_ = 50;
        mat0_ = Material::Create(fac, qty1_, Composition::CreateFromAtom(cm));
        mat1_ = Material::Create(fac, qty1_, Composition::CreateFromAtom(cm));
        mat2_ = Material::Create(fac, qty2_, Composition::CreateFromAtom(cm));

        max_inv_size_ = 100;

        buf0_.capacity(20);
        buf0_.Push(mat0_);
        single_tracker_.Init({&buf0_}, max_inv_size_);

        buf1_.capacity(50);
        buf2_.capacity(max_inv_size_);
        buf3_.capacity(50);
        buf1_.Push(mat1_);
        buf2_.Push(mat2_);
        multi_tracker_.Init({&buf1_, &buf2_, &buf3_}, max_inv_size_);
    } catch (std::exception err) {
      FAIL() << "An exception was thrown in the fixture SetUp.";
    }
    }
  virtual void TearDown() {
    delete ctx;
  }
};

} // namespace toolkit
} // namespace cyclus

#endif // CYCLUS_TESTS_TOOLKIT_TOTAL_INV_TRACKER_TESTS_H
