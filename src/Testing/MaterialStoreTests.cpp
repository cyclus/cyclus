
#include <gtest/gtest.h>
#include "IsoVector.h"
#include "Material.h"
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

    MaterialStore store_; // default constructed mat store
    MaterialStore filled_store_;

    double neg_cap, zero_cap, cap, low_cap;

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
        vect2_.setMass(u238, v2_m_u238);

        mat1_ = mat_rsrc_ptr(new Material(vect1_));
        mat2_ = mat_rsrc_ptr(new Material(vect2_));

        neg_cap = -1;
        zero_cap = 0;
        cap = 334; // should be higher than mat1+mat2 masses
        low_cap = 332; // should be lower than mat1_mat2 masses

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
TEST_F(MaterialStoreTest, GetCapacityExceptions) {
  ASSERT_NO_THROW(store_.capacity());
  store_.setCapacity(zero_cap);
  ASSERT_NO_THROW(store_.capacity());
  store_.setCapacity(zero_cap);
  ASSERT_NO_THROW(store_.capacity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, InitialCapacity) {
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

  double space = cap - (mat1_.quantity() + mat2_.quantity());
  ASSERT_NO_THROW(filled_store_.space());
  EXPECT_DOUBLE_EQ(filled_store_.space(), space);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, GetInventory) {
  ASSERT_NO_THROW(store.inventory());
  EXPECT_DOUBLE_EQ(store_.inventory(), 0.0);

  ASSERT_NO_THROW(filled_store_.inventory());
  double inventory = mat1_.quantity() + mat2_.quantity();
  EXPECT_DOUBLE_EQ(filled_store_.inventory(), inventory);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, InitialUnlimited) {
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
TEST_F(MaterialStoreTest, UnlimitedCap) {
  store_.makeUnlimited();
  EXPECT_NO_THROW(store_.setCapacity(cap));
  EXPECT_DOUBLE_EQ(store_.capacity(), -1.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, UnlimitedSpace) {
  store_.makeUnlimited();
  EXPECT_DOUBLE_EQ(store_.space(), -1.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, MakeLimitedExceptions) {
  store_.makeUnlimited();
  filled_store_.makeUnlimited();
  ASSERT_THROW(store_.makeLimited(neg_cap), CycOverCapException);
  ASSERT_THROW(filled_store_.makeLimited(low_cap), CycOverCapException);

  // be sure that unlimited status doesn't change if exceptions are thrown
  EXPECT_EQ(store_.unlimited(), true);
  EXPECT_EQ(store_.limited(), true);

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
TEST_F(MaterialStoreTest, DefaultRemoveProperties) {
  ASSERT_NO_THROW(store_.splitable());
  EXPECT_EQ(store_.splitable(), true);

  ASSERT_NO_THROW(store_.overQtyOnremove());
  EXPECT_EQ(store_.overQtyOnremove(), false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, MakeSplitableAndNot) {
  ASSERT_NO_THROW(store_.makeSplitable());
  store_.makeSplitable();
  EXPECT_EQ(store_.splitable(), true);
  EXPECT_EQ(store_.overQtyOnRemove(), false);

  ASSERT_NO_THROW(store_.makeNotSplitableUnder());
  store_.makeNotSplitableUnder();
  EXPECT_EQ(store_.splitable(), false);
  EXPECT_EQ(store_.overQtyOnRemove(), false);

  ASSERT_NO_THROW(store_.makeNotSplitableOver());
  store_.makeNotSplitableOver();
  EXPECT_EQ(store_.splitable(), false);
  EXPECT_EQ(store_.overQtyOnRemove(), true);

  ASSERT_NO_THROW(store_.makeSplitable());
  store_.makeSplitable();
  EXPECT_EQ(store_.splitable(), true);
  EXPECT_EQ(store_.overQtyOnRemove(), false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveQtySplitable) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveQtyNotSplitableOver) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveQtyNotSplitableUnder) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveOne) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, RemoveNum) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, AddOne) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, AddAll) {
}

