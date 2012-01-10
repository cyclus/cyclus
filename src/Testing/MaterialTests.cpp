#include <gtest/gtest.h>
#include "Material.h"
#include "IsoVector.h"
#include "Timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class MaterialTest : public ::testing::Test {
  protected:
    Material* test_mat;

    virtual void SetUp(){
      IsoVector test_comp;
      test_mat = new Material(test_comp);
    };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialTest, Constructors){
  EXPECT_DOUBLE_EQ(test_mat->getQuantity(), 0.0);
}
