#include "res_map_tests.h"

#include <gtest/gtest.h>

namespace cyclus {
namespace toolkit {

// The "Empty" suffix indicates the test uses the store_ instance of
// ResMap. The "Filled" suffix indicates the test uses the
// filled_store_ instance of v1.2.

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - Getters, Setters, and Property changers - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, set_capacity_ExceptionsEmpty) {
  EXPECT_THROW(store_.capacity(neg_cap), ValueError);
  EXPECT_NO_THROW(store_.capacity(zero_cap));
  EXPECT_NO_THROW(store_.capacity(cap));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, set_capacity_ExceptionsFilled) {
  EXPECT_THROW(filled_store_.capacity(low_cap), ValueError);
  EXPECT_NO_THROW(filled_store_.capacity(cap));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, GetCapacity_ExceptionsEmpty) {
  ASSERT_NO_THROW(store_.capacity());
  store_.capacity(zero_cap);
  ASSERT_NO_THROW(store_.capacity());
  store_.capacity(zero_cap);
  ASSERT_NO_THROW(store_.capacity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, GetCapacity_InitialEmpty) {
  EXPECT_DOUBLE_EQ(store_.capacity(), INFINITY);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, Getset_capacityEmpty) {
  store_.capacity(zero_cap);
  EXPECT_DOUBLE_EQ(store_.capacity(), zero_cap);

  store_.capacity(cap);
  EXPECT_DOUBLE_EQ(store_.capacity(), cap);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, GetSpace_Empty) {
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), INFINITY);

  store_.capacity(zero_cap);
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), zero_cap);

  store_.capacity(cap);
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), cap);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, GetSpace_Filled) {
  double space = cap - (mat1_->quantity() + mat2_->quantity());
  ASSERT_NO_THROW(filled_store_.space());
  EXPECT_DOUBLE_EQ(filled_store_.space(), space);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, GetQuantity_Empty) {
  ASSERT_NO_THROW(store_.quantity());
  EXPECT_DOUBLE_EQ(store_.quantity(), 0.0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, GetQuantity_Filled) {
  ASSERT_NO_THROW(filled_store_.quantity());
  double quantity = mat1_->quantity() + mat2_->quantity();
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), quantity);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, GetSizeEmpty) {
  ASSERT_NO_THROW(store_.size());
  EXPECT_EQ(store_.size(), 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, GetSizeFilled) {
  ASSERT_NO_THROW(filled_store_.size());
  EXPECT_DOUBLE_EQ(filled_store_.size(), 2);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - Pushing into buffer - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, PushEmpty) {
  ASSERT_NO_THROW(store_.capacity(cap));

  ASSERT_NO_THROW(store_["mat1"] = mat1_);
  ASSERT_EQ(store_.size(), 1);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity());

  ASSERT_NO_THROW(store_["mat2"] = mat2_);
  ASSERT_EQ(store_.size(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, Push_OverCapacityEmpty) {
  ASSERT_NO_THROW(store_.capacity(cap));

  ASSERT_NO_THROW(store_["map2"] = mat1_);
  ASSERT_NO_THROW(store_["mat2"] = mat2_);

  double topush = cap - store_.quantity();
  Product::Ptr overmat = Product::CreateUntracked(topush + overeps, "food");

  store_["overmat"] = overmat;
  ASSERT_THROW(store_.quantity(), ValueError);
  ASSERT_EQ(store_.size(), 3);

  store_.erase("overmat");
  overmat = Product::CreateUntracked(topush + undereps, "food");
  store_["overmat"] = overmat;
  ASSERT_NO_THROW(store_.quantity());
  ASSERT_EQ(store_.size(), 3);

  double expected = mat1_->quantity() + mat2_->quantity() + overmat->quantity();
  ASSERT_DOUBLE_EQ(store_.quantity(), expected);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, Push_DuplicateEmpty) {
  ASSERT_NO_THROW(store_.capacity(cap));

  ASSERT_NO_THROW(store_["mat1"] = mat1_);
  ASSERT_NO_THROW(store_["mat1"] = mat1_);

  ASSERT_EQ(store_.size(), 1);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, Values) {
  std::vector<Product::Ptr> vals = filled_store_.Values();
  ASSERT_EQ(vals.size(), 2);
  EXPECT_EQ(vals[0], mat1_);
  EXPECT_EQ(vals[1], mat2_);
}

TEST_F(ResMapTest, ResValues) {
  std::vector<Resource::Ptr> vals = filled_store_.ResValues();
  std::vector<Resource::Ptr> resvals = ResCast(filled_store_.Values());
  ASSERT_EQ(vals.size(), 2);
  EXPECT_EQ(vals[0], resvals[0]);
  EXPECT_EQ(vals[1], resvals[1]);
}

}  // namespace toolkit
}  // namespace cyclus
