
#include <gtest/gtest.h>
#include "IsoVector.h"
#include "ResourceBuff.h"
#include "CycException.h"
#include "Logger.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class ResourceBuffTest : public ::testing::Test {
  protected:
    IsoVector vect1_, vect2_;

    int oxygen, u235, u238, pu240;

    double v1_m_oxygen, v1_m_u235, v1_m_u238;
    double v2_m_oxygen, v2_m_u235, v2_m_pu240;

    rsrc_ptr mat1_;
    rsrc_ptr mat2_;
    Manifest mats;

    ResourceBuff store_; // default constructed mat store
    ResourceBuff filled_store_;

    double neg_cap, zero_cap, cap, low_cap;
    double exact_qty; // mass in filled_store_
    double exact_qty_under; // mass in filled_store - 0.9*STORE_EPS
    double exact_qty_over; // mass in filled_store + 0.9*STORE_EPS
    double over_qty;  // mass in filled_store - 1.1*STORE_EPS
    double under_qty; // mass in filled_store + 1.1*STORE_EPS
    double overeps, undereps;

    virtual void SetUp() {
      try {
        oxygen = 8001;
        u235 = 92235;
        u238 = 92238;
        pu240 = 94240;

        v1_m_oxygen = 1;
        v1_m_u235 = 10;
        v1_m_u238 = 100;

        v2_m_oxygen = 2;
        v2_m_u235 = 20;
        v2_m_pu240 = 200;

        vect1_.setMass(oxygen, v1_m_oxygen);
        vect1_.setMass(u235, v1_m_u235);
        vect1_.setMass(u238, v1_m_u238);

        vect2_.setMass(oxygen, v2_m_oxygen);
        vect2_.setMass(u235, v2_m_u235);
        vect2_.setMass(pu240, v2_m_pu240);

        mat1_ = rsrc_ptr(new Material(vect1_));
        mat2_ = rsrc_ptr(new Material(vect2_));
        mats.push_back(mat1_);
        mats.push_back(mat2_);

        neg_cap = -1;
        zero_cap = 0;
        cap = 334; // should be higher than mat1+mat2 masses
        low_cap = 332; // should be lower than mat1_mat2 masses

        undereps = 0.9 * STORE_EPS;
        overeps = 1.1 * STORE_EPS;
        exact_qty = mat1_->quantity();
        exact_qty_under = exact_qty - undereps;
        exact_qty_over = exact_qty + undereps;
        under_qty = exact_qty - overeps;
        over_qty = exact_qty + overeps;

        filled_store_.setCapacity(cap);
        filled_store_.pushOne(mat1_);
        filled_store_.pushOne(mat2_);
      } catch (std::exception err) {
        LOG(LEV_ERROR, "MSTest") << err.what();
        FAIL() << "An exception was thrown in the fixture SetUp.";
      }
    }
};

/*
To check:

  * after makeUnlimited:
    * can push materials without restraint

*/

// Test order MATTERS.  Beware of reordering.

// The "Empty" suffix indicates the test uses the store_ instance of
// ResourceBuff. The "Filled" suffix indicates the test uses the
// filled_store_ instance of ResourceBuff.

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, SetCapacity_ExceptionsEmpty) {
  EXPECT_THROW(store_.setCapacity(neg_cap), CycOverCapException);
  EXPECT_NO_THROW(store_.setCapacity(zero_cap));
  EXPECT_NO_THROW(store_.setCapacity(cap));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, SetCapacity_ExceptionsFilled) {
  EXPECT_THROW(filled_store_.setCapacity(low_cap), CycOverCapException);
  EXPECT_NO_THROW(filled_store_.setCapacity(cap));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, GetCapacity_ExceptionsEmpty) {
  ASSERT_NO_THROW(store_.capacity());
  store_.setCapacity(zero_cap);
  ASSERT_NO_THROW(store_.capacity());
  store_.setCapacity(zero_cap);
  ASSERT_NO_THROW(store_.capacity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, GetCapacity_InitialEmpty) {
  EXPECT_DOUBLE_EQ(store_.capacity(), 0.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, GetSetCapacityEmpty) {
  store_.setCapacity(zero_cap);
  EXPECT_DOUBLE_EQ(store_.capacity(), zero_cap);

  store_.setCapacity(cap);
  EXPECT_DOUBLE_EQ(store_.capacity(), cap);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, GetSpace_Empty) {
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), 0.0);

  store_.setCapacity(zero_cap);
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), zero_cap);

  store_.setCapacity(cap);
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), cap);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, GetSpace_Filled) {
  double space = cap - (mat1_->quantity() + mat2_->quantity());
  ASSERT_NO_THROW(filled_store_.space());
  EXPECT_DOUBLE_EQ(filled_store_.space(), space);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, GetQuantity_Empty) {
  ASSERT_NO_THROW(store_.quantity());
  EXPECT_DOUBLE_EQ(store_.quantity(), 0.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, GetQuantity_Filled) {
  ASSERT_NO_THROW(filled_store_.quantity());
  double quantity = mat1_->quantity() + mat2_->quantity();
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), quantity);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, GetCount_Empty) {
  ASSERT_NO_THROW(store_.count());
  EXPECT_EQ(store_.count(), 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, GetCount_Filled) {
  ASSERT_NO_THROW(filled_store_.count());
  EXPECT_DOUBLE_EQ(filled_store_.count(), 2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, GetUnlimited_InitialEmpty) {
  ASSERT_NO_THROW(store_.unlimited());
  EXPECT_EQ(store_.unlimited(), false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, MakeUnlimited_Empty) {
  ASSERT_NO_THROW(store_.makeUnlimited());
  store_.makeUnlimited();
  EXPECT_EQ(store_.unlimited(), true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, GetCapacity_UnlimitedEmpty) {
  store_.makeUnlimited();
  EXPECT_NO_THROW(store_.setCapacity(cap));
  EXPECT_DOUBLE_EQ(store_.capacity(), -1.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, GetSpace_UnlimitedEmpty) {
  store_.makeUnlimited();
  EXPECT_DOUBLE_EQ(store_.space(), -1.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, MakeLimited_ExceptionsEmpty) {
  store_.makeUnlimited();
  ASSERT_THROW(store_.makeLimited(neg_cap), CycOverCapException);

  // be sure that unlimited status doesn't change if exceptions are thrown
  EXPECT_EQ(store_.unlimited(), true);

  ASSERT_NO_THROW(store_.makeLimited(zero_cap));
  ASSERT_NO_THROW(store_.makeLimited(cap));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, MakeLimited_ExceptionsFilled) {
  filled_store_.makeUnlimited();
  ASSERT_THROW(filled_store_.makeLimited(low_cap), CycOverCapException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, MakeLimited_Empty) {
  store_.makeUnlimited();
  ASSERT_EQ(store_.unlimited(), true);

  store_.makeLimited(zero_cap);
  ASSERT_EQ(store_.unlimited(), false);

  EXPECT_DOUBLE_EQ(store_.capacity(), zero_cap);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, MakeLimited_Filled) {
  filled_store_.makeUnlimited();
  ASSERT_EQ(filled_store_.unlimited(), true);

  filled_store_.makeLimited(cap);
  ASSERT_EQ(filled_store_.unlimited(), false);

  EXPECT_DOUBLE_EQ(filled_store_.capacity(), cap);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveQty_ExceptionsEmpty) {
  Manifest manifest;
  double qty = cap + overeps;
  ASSERT_THROW(manifest = filled_store_.popQty(qty), CycNegQtyException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveQty_ExceptionsFilled) {
  Manifest manifest;
  double qty = cap + overeps;
  ASSERT_THROW(manifest = store_.popQty(qty), CycNegQtyException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveQty_NoSplitExactFilled) {
  // pop one no splitting leaving one mat in the store
  Manifest manifest;

  ASSERT_NO_THROW(manifest = filled_store_.popQty(exact_qty));
  ASSERT_EQ(manifest.size(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveQty_NoSplitOverFilled) {
  // pop one no splitting leaving one mat in the store
  Manifest manifest;

  ASSERT_NO_THROW(manifest = filled_store_.popQty(exact_qty_over));
  ASSERT_EQ(manifest.size(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveQty_NoSplitUnderFilled) {
  // pop one no splitting leaving one mat in the store
  Manifest manifest;

  ASSERT_NO_THROW(manifest = filled_store_.popQty(exact_qty_under));
  ASSERT_EQ(manifest.size(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveQty_SplitOverFilled) {
  // pop two with splitting leaving one mat in the store
  Manifest manifest;
  double store_final = mat1_->quantity() + mat2_->quantity() - over_qty;

  ASSERT_NO_THROW(manifest = filled_store_.popQty(over_qty));
  ASSERT_EQ(manifest.size(), 2);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_NEAR(manifest.at(1)->quantity(), overeps, STORE_EPS); // not sure why DOUBLE_EQ doesn't work
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_EQ(manifest.at(1), mat2_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), store_final);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveQty_SplitUnderFilled) {
  // pop one with splitting leaving two mats in the store
  Manifest manifest;
  double store_final = mat1_->quantity() + mat2_->quantity() - under_qty;

  ASSERT_NO_THROW(manifest = filled_store_.popQty(under_qty));
  ASSERT_EQ(manifest.size(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), under_qty);
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_EQ(filled_store_.count(), 2);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), store_final);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveNum_ExceptionsFilled) {
  Manifest manifest;
  ASSERT_THROW(manifest = filled_store_.popNum(3), CycNegQtyException);
  ASSERT_THROW(manifest = filled_store_.popNum(-1), CycNegQtyException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveNum_ZeroFilled) {
  Manifest manifest;
  double tot_qty = filled_store_.quantity();

  ASSERT_NO_THROW(manifest = filled_store_.popNum(0));
  ASSERT_EQ(manifest.size(), 0);
  ASSERT_EQ(filled_store_.count(), 2);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), tot_qty);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveNum_OneFilled) {
  Manifest manifest;

  ASSERT_NO_THROW(manifest = filled_store_.popNum(1));
  ASSERT_EQ(manifest.size(), 1);
  ASSERT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveNum_TwoFilled) {
  Manifest manifest;

  ASSERT_NO_THROW(manifest = filled_store_.popNum(2));
  ASSERT_EQ(manifest.size(), 2);
  ASSERT_EQ(filled_store_.count(), 0);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_DOUBLE_EQ(manifest.at(1)->quantity(), mat2_->quantity());
  EXPECT_EQ(manifest.at(1), mat2_);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), 0.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveOne_Filled) {
  rsrc_ptr mat;

  ASSERT_NO_THROW(mat = filled_store_.popOne());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat1_->quantity());
  EXPECT_EQ(mat, mat1_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());

  ASSERT_NO_THROW(mat = filled_store_.popOne());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat2_->quantity());
  EXPECT_EQ(mat, mat2_);
  EXPECT_EQ(filled_store_.count(), 0);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), 0.0);

  ASSERT_THROW(mat = filled_store_.popOne(), CycNegQtyException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, PushOne_Empty) {
  ASSERT_NO_THROW(store_.setCapacity(cap));

  ASSERT_NO_THROW(store_.pushOne(mat1_));
  ASSERT_EQ(store_.count(), 1);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity());

  ASSERT_NO_THROW(store_.pushOne(mat2_));
  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, PushOne_OverCapacityEmpty) {
  ASSERT_NO_THROW(store_.setCapacity(cap));

  ASSERT_NO_THROW(store_.pushOne(mat1_));
  ASSERT_NO_THROW(store_.pushOne(mat2_));

  double topush = cap - store_.quantity();
  rsrc_ptr overmat = mat1_->clone();
  overmat->setQuantity(topush + overeps);

  ASSERT_THROW(store_.pushOne(overmat), CycOverCapException);
  ASSERT_EQ(store_.count(), 2);
  ASSERT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());

  overmat->setQuantity(topush + undereps);
  ASSERT_NO_THROW(store_.pushOne(overmat));
  ASSERT_EQ(store_.count(), 3);

  double expected = mat1_->quantity() + mat2_->quantity() + overmat->quantity();
  ASSERT_DOUBLE_EQ(store_.quantity(), expected);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, PushOne_DuplicateEmpty) {
  ASSERT_NO_THROW(store_.setCapacity(cap));

  ASSERT_NO_THROW(store_.pushOne(mat1_));
  ASSERT_THROW(store_.pushOne(mat1_), CycDupResException);

  ASSERT_EQ(store_.count(), 1);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, PushAll_Empty) {
  ASSERT_NO_THROW(store_.setCapacity(cap));
  ASSERT_NO_THROW(store_.pushAll(mats));
  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, PushAll_NoneEmpty) {
  Manifest manifest;
  ASSERT_NO_THROW(store_.setCapacity(cap));
  ASSERT_NO_THROW(store_.pushAll(manifest));
  ASSERT_EQ(store_.count(), 0);
  EXPECT_DOUBLE_EQ(store_.quantity(), 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, PushAll_RetrieveOrderEmpty) {
  rsrc_ptr mat;

  ASSERT_NO_THROW(store_.setCapacity(cap));
  ASSERT_NO_THROW(store_.pushAll(mats));
  ASSERT_NO_THROW(mat = store_.popOne());
  ASSERT_EQ(mat, mat1_);
  ASSERT_NO_THROW(mat = store_.popOne());
  ASSERT_EQ(mat, mat2_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, PushAll_OverCapacityEmpty) {
  ASSERT_NO_THROW(store_.setCapacity(cap));
  ASSERT_NO_THROW(store_.pushAll(mats));

  double topush = cap - store_.quantity();
  rsrc_ptr overmat = mat1_->clone();
  overmat->setQuantity(topush + overeps);
  Manifest overmats;
  overmats.push_back(overmat);

  ASSERT_THROW(store_.pushAll(overmats), CycOverCapException);
  ASSERT_EQ(store_.count(), 2);
  ASSERT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());

  overmat->setQuantity(topush + undereps);
  overmats.clear();
  overmats.push_back(overmat);

  ASSERT_NO_THROW(store_.pushAll(overmats));
  ASSERT_EQ(store_.count(), 3);

  double expected = mat1_->quantity() + mat2_->quantity() + overmat->quantity();
  ASSERT_DOUBLE_EQ(store_.quantity(), expected);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, PushAll_DuplicateEmpty) {
  ASSERT_NO_THROW(store_.setCapacity(2 * cap));

  ASSERT_NO_THROW(store_.pushAll(mats));
  ASSERT_THROW(store_.pushOne(mat1_), CycDupResException);
  ASSERT_THROW(store_.pushOne(mat2_), CycDupResException);
  ASSERT_THROW(store_.pushAll(mats), CycDupResException);

  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

