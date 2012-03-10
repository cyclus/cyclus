
#include <gtest/gtest.h>
#include "IsoVector.h"
#include "MaterialStore.h"
#include "CycException.h"
#include "Logger.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class MaterialStoreTest : public ::testing::Test {
  protected:
    IsoVector vect1_, vect2_;

    int oxygen, u235, u238, pu240;

    double v1_m_oxygen, v1_m_u235, v1_m_u238;
    double v2_m_oxygen, v2_m_u235, v2_m_pu240;

    mat_rsrc_ptr mat1_;
    mat_rsrc_ptr mat2_;
    MatManifest mats;

    MaterialStore store_; // default constructed mat store
    MaterialStore filled_store_;

    double neg_cap, zero_cap, cap, low_cap;
    double exact_qty; // mass in filled_store_
    double exact_qty_under; // mass in filled_store - 0.9*STORE_EPS
    double exact_qty_over; // mass in filled_store + 0.9*STORE_EPS
    double over_qty;  // mass in filled_store - 1.1*STORE_EPS
    double under_qty; // mass in filled_store + 1.1*STORE_EPS

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

        mat1_ = mat_rsrc_ptr(new Material(vect1_));
        mat2_ = mat_rsrc_ptr(new Material(vect2_));
        mats.push_back(mat1_);
        mats.push_back(mat2_);

        neg_cap = -1;
        zero_cap = 0;
        cap = 334; // should be higher than mat1+mat2 masses
        low_cap = 332; // should be lower than mat1_mat2 masses

        exact_qty = mat1_->quantity();
        exact_qty_under = exact_qty - 0.9 * STORE_EPS;
        exact_qty_over = exact_qty + 0.9 * STORE_EPS;
        under_qty = exact_qty - 1.1 * STORE_EPS;
        over_qty = exact_qty + 1.1 * STORE_EPS;

        filled_store_.setCapacity(cap);
        filled_store_.addOne(mat1_);
        filled_store_.addOne(mat2_);
      } catch (std::exception err) {
        LOG(LEV_ERROR, "MSTest") << err.what();
        FAIL() << "An exception was thrown in the fixture SetUp.";
      }
    }
};

/*
To check:

  * setCapacity(cap) throws when the new cap is lower than existing store

  * after makeUnlimited:
    * can add materials without restraint

*/

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, SetCapacityExceptions) {
  EXPECT_THROW(store_.setCapacity(neg_cap), CycOverCapException);
  EXPECT_THROW(filled_store_.setCapacity(low_cap), CycOverCapException);
  EXPECT_NO_THROW(store_.setCapacity(zero_cap));
  EXPECT_NO_THROW(store_.setCapacity(cap));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, GetCapacity_Exceptions) {
  ASSERT_NO_THROW(store_.capacity());
  store_.setCapacity(zero_cap);
  ASSERT_NO_THROW(store_.capacity());
  store_.setCapacity(zero_cap);
  ASSERT_NO_THROW(store_.capacity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, GetCapacity_Initial) {
  EXPECT_DOUBLE_EQ(store_.capacity(), 0.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, GetSetCapacity) {
  store_.setCapacity(zero_cap);
  EXPECT_DOUBLE_EQ(store_.capacity(), zero_cap);

  store_.setCapacity(cap);
  EXPECT_DOUBLE_EQ(store_.capacity(), cap);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, GetSpace) {
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), 0.0);

  store_.setCapacity(zero_cap);
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), zero_cap);

  store_.setCapacity(cap);
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), cap);

  double space = cap - (mat1_->quantity() + mat2_->quantity());
  ASSERT_NO_THROW(filled_store_.space());
  EXPECT_DOUBLE_EQ(filled_store_.space(), space);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, GetQuantity) {
  ASSERT_NO_THROW(store_.quantity());
  EXPECT_DOUBLE_EQ(store_.quantity(), 0.0);

  ASSERT_NO_THROW(filled_store_.quantity());
  double quantity = mat1_->quantity() + mat2_->quantity();
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), quantity);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, GetCount) {
  ASSERT_NO_THROW(store_.count());
  EXPECT_EQ(store_.count(), 0);

  ASSERT_NO_THROW(filled_store_.count());
  EXPECT_DOUBLE_EQ(filled_store_.count(), 2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, GetUnlimited_Initial) {
  ASSERT_NO_THROW(store_.unlimited());
  EXPECT_EQ(store_.unlimited(), false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, MakeUnlimited) {
  ASSERT_NO_THROW(store_.makeUnlimited());
  store_.makeUnlimited();
  EXPECT_EQ(store_.unlimited(), true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, GetCapacity_Unlimited) {
  store_.makeUnlimited();
  EXPECT_NO_THROW(store_.setCapacity(cap));
  EXPECT_DOUBLE_EQ(store_.capacity(), -1.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, GetSpace_Unlimited) {
  store_.makeUnlimited();
  EXPECT_DOUBLE_EQ(store_.space(), -1.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, MakeLimited_Exceptions) {
  store_.makeUnlimited();
  filled_store_.makeUnlimited();
  ASSERT_THROW(store_.makeLimited(neg_cap), CycOverCapException);
  ASSERT_THROW(filled_store_.makeLimited(low_cap), CycOverCapException);

  // be sure that unlimited status doesn't change if exceptions are thrown
  EXPECT_EQ(store_.unlimited(), true);

  ASSERT_NO_THROW(store_.makeLimited(zero_cap));
  ASSERT_NO_THROW(store_.makeLimited(cap));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, MakeLimited) {
  store_.makeUnlimited();
  filled_store_.makeUnlimited();

  ASSERT_EQ(store_.unlimited(), true);
  ASSERT_EQ(filled_store_.unlimited(), true);

  store_.makeLimited(zero_cap);
  filled_store_.makeLimited(cap);

  ASSERT_EQ(store_.unlimited(), false);
  ASSERT_EQ(filled_store_.unlimited(), false);

  EXPECT_DOUBLE_EQ(store_.capacity(), zero_cap);
  EXPECT_DOUBLE_EQ(filled_store_.capacity(), cap);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveQty_Exceptions) {
  MatManifest manifest;
  double qty = cap + 1.1 * STORE_EPS;
  ASSERT_THROW(manifest = filled_store_.removeQty(qty), CycNegQtyException);
  ASSERT_THROW(manifest = store_.removeQty(qty), CycNegQtyException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveQty_NoSplitExact) {
  // remove one no splitting leaving one mat in the store
  MatManifest manifest;

  ASSERT_NO_THROW(manifest = filled_store_.removeQty(exact_qty));
  ASSERT_EQ(manifest.size(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveQty_NoSplitOver) {
  // remove one no splitting leaving one mat in the store
  MatManifest manifest;

  ASSERT_NO_THROW(manifest = filled_store_.removeQty(exact_qty_over));
  ASSERT_EQ(manifest.size(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveQty_NoSplitUnder) {
  // remove one no splitting leaving one mat in the store
  MatManifest manifest;

  ASSERT_NO_THROW(manifest = filled_store_.removeQty(exact_qty_under));
  ASSERT_EQ(manifest.size(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveQty_SplitOver) {
  // remove two with splitting leaving one mat in the store
  MatManifest manifest;
  double store_final = mat1_->quantity() + mat2_->quantity() - over_qty;

  ASSERT_NO_THROW(manifest = filled_store_.removeQty(over_qty));
  ASSERT_EQ(manifest.size(), 2);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_DOUBLE_EQ(manifest.at(1)->quantity(), over_qty - mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), store_final);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveQty_SplitUnder) {
  // remove one with splitting leaving two mats in the store
  MatManifest manifest;
  double store_final = mat1_->quantity() + mat2_->quantity() - under_qty;

  ASSERT_NO_THROW(manifest = filled_store_.removeQty(under_qty));
  ASSERT_EQ(manifest.size(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), under_qty);
  EXPECT_NE(manifest.at(0), mat1_);
  EXPECT_EQ(filled_store_.count(), 2);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), store_final);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveNum_Exceptions) {
  MatManifest manifest;
  ASSERT_THROW(manifest = filled_store_.removeNum(3), CycNegQtyException);
  ASSERT_THROW(manifest = filled_store_.removeNum(-1), CycNegQtyException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveNum_Zero) {
  MatManifest manifest;
  double tot_qty = filled_store_.quantity();

  ASSERT_NO_THROW(manifest = filled_store_.removeNum(0));
  ASSERT_EQ(manifest.size(), 0);
  ASSERT_EQ(filled_store_.count(), 2);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), tot_qty);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveNum_One) {
  MatManifest manifest;

  ASSERT_NO_THROW(manifest = filled_store_.removeNum(1));
  ASSERT_EQ(manifest.size(), 1);
  ASSERT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveNum_Two) {
  MatManifest manifest;

  ASSERT_NO_THROW(manifest = filled_store_.removeNum(2));
  ASSERT_EQ(manifest.size(), 2);
  ASSERT_EQ(filled_store_.count(), 0);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_DOUBLE_EQ(manifest.at(1)->quantity(), mat2_->quantity());
  EXPECT_EQ(manifest.at(1), mat2_);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), 0.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveOne) {
  mat_rsrc_ptr mat;

  ASSERT_NO_THROW(mat = filled_store_.removeOne());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat1_->quantity());
  EXPECT_EQ(mat, mat1_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());

  ASSERT_NO_THROW(mat = filled_store_.removeOne());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat2_->quantity());
  EXPECT_EQ(mat, mat2_);
  EXPECT_EQ(filled_store_.count(), 0);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), 0.0);

  ASSERT_THROW(mat = filled_store_.removeOne(), CycNegQtyException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, AddOne) {
  ASSERT_NO_THROW(store_.setCapacity(cap));

  ASSERT_NO_THROW(store_.addOne(mat1_));
  ASSERT_EQ(store_.count(), 1);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity());

  ASSERT_NO_THROW(store_.addOne(mat2_));
  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, AddOne_OverCapacity) {
  ASSERT_NO_THROW(store_.setCapacity(cap));

  ASSERT_NO_THROW(store_.addOne(mat1_));
  ASSERT_NO_THROW(store_.addOne(mat2_));

  double toadd = cap - store_.quantity();
  mat_rsrc_ptr overmat = boost::dynamic_pointer_cast<Material>(mat1_->clone());
  overmat->setQuantity(toadd + 1.1 * STORE_EPS);

  ASSERT_THROW(store_.addOne(overmat), CycOverCapException);
  ASSERT_EQ(store_.count(), 2);
  ASSERT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());

  overmat->setQuantity(toadd + 0.9 * STORE_EPS);
  ASSERT_NO_THROW(store_.addOne(overmat));
  ASSERT_EQ(store_.count(), 3);

  double expected = mat1_->quantity() + mat2_->quantity() + 0.9 * STORE_EPS;
  ASSERT_DOUBLE_EQ(store_.quantity(), expected);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, AddOne_Duplicate) {
  ASSERT_NO_THROW(store_.setCapacity(cap));

  ASSERT_NO_THROW(store_.addOne(mat1_));
  ASSERT_THROW(store_.addOne(mat1_), CycDupMatException);

  ASSERT_EQ(store_.count(), 1);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, AddAll) {
  ASSERT_NO_THROW(store_.setCapacity(cap));
  ASSERT_NO_THROW(store_.addAll(mats));
  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, AddAll_Empty) {
  MatManifest manifest;
  ASSERT_NO_THROW(store_.setCapacity(cap));
  ASSERT_NO_THROW(store_.addAll(manifest));
  ASSERT_EQ(store_.count(), 0);
  EXPECT_DOUBLE_EQ(store_.quantity(), 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, AddAll_RetrieveOrder) {
  mat_rsrc_ptr mat;

  ASSERT_NO_THROW(store_.setCapacity(cap));
  ASSERT_NO_THROW(store_.addAll(mats));
  ASSERT_NO_THROW(mat = store_.removeOne());
  ASSERT_EQ(mat, mat1_);
  ASSERT_NO_THROW(mat = store_.removeOne());
  ASSERT_EQ(mat, mat2_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, AddAll_OverCapacity) {
  ASSERT_NO_THROW(store_.setCapacity(cap));
  ASSERT_NO_THROW(store_.addAll(mats));

  double toadd = cap - store_.quantity();
  mat_rsrc_ptr overmat = boost::dynamic_pointer_cast<Material>(mat1_->clone());
  overmat->setQuantity(toadd + 1.1 * STORE_EPS);
  MatManifest overmats;
  overmats.push_back(overmat);

  ASSERT_THROW(store_.addAll(overmats), CycOverCapException);
  ASSERT_EQ(store_.count(), 2);
  ASSERT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());

  overmat->setQuantity(toadd + 0.9 * STORE_EPS);
  overmats.clear();
  overmats.push_back(overmat);

  ASSERT_NO_THROW(store_.addAll(overmats));
  ASSERT_EQ(store_.count(), 3);

  double expected = mat1_->quantity() + mat2_->quantity() + 0.9 * STORE_EPS;
  ASSERT_DOUBLE_EQ(store_.quantity(), expected);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, AddAll_Duplicate) {
  ASSERT_NO_THROW(store_.setCapacity(cap));

  ASSERT_NO_THROW(store_.addAll(mats));
  ASSERT_THROW(store_.addOne(mat1_), CycDupMatException);
  ASSERT_THROW(store_.addOne(mat2_), CycDupMatException);
  ASSERT_THROW(store_.addAll(mats), CycDupMatException);

  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

