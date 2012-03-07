
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

    MaterialStore store_;

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
        v2_m_pu240 = 2000;

        vect1_.setMass(oxygen, v1_m_oxygen);
        vect1_.setMass(u235, v1_m_u235);
        vect1_.setMass(u238, v1_m_u238);

        vect2_.setMass(oxygen, v2_m_oxygen);
        vect2_.setMass(u235, v2_m_u235);
        vect2_.setMass(u238, v2_m_u238);

        mat1_ = mat_rsrc_ptr(new Material(vect1_));
        mat2_ = mat_rsrc_ptr(new Material(vect2_));
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
    * capacity and space always return -1 regardless of how many materials are added

*/

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, GetCapacity) {
  ASSERT_NO_THROW(store_.capacity());
  EXPECT_DOUBLE_EQ(store_.capacity(), 0.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, SetCapacity) {
  double capacity = 1500.0;
  EXPECT_THROW(store_.setCapacity(-1), CycRangeException);
  ASSERT_NO_THROW(store_.setCapacity(capacity));
  EXPECT_DOUBLE_EQ(store_.capacity(), capacity);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, GetSpace) {
  double capacity = 1500.0;
  store_.setCapacity(capacity);
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), capacity);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, MakeUnlimited) {
  double capacity = 1500.0;
  store_.setCapacity(capacity);
  ASSERT_NO_THROW(store_.makeUnlimited());
  EXPECT_EQ(store_.capacity(), -1);
  EXPECT_EQ(store_.space(), -1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, NoSplitOver) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, NoSplitUnder) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, Splitable) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, ExtractQty) {
  // returns a std::vector
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, ExtractOne) {
  // returns mat_rsrc_ptr
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, ExtractNum) {
  // returns a std::vector
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, AddOne) {
  // takes a mat_rsrc_ptr
  // throw if added mat breaks capacity
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialStoreTest, AddAll) {
  // takes a std::vector
  // throw if added mats break capacity
}

