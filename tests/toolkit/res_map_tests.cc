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


TEST_F(ResMapTest, ObjIds) {
  std::map<std::string, int> oids;
  ASSERT_NO_THROW(oids = filled_store_.obj_ids());
  ASSERT_NO_THROW(store_.obj_ids(oids));
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - Pushing into buffer - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, PushEmpty) {
  ASSERT_NO_THROW(store_["mat1"] = mat1_);
  ASSERT_EQ(store_.size(), 1);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity());

  ASSERT_NO_THROW(store_["mat2"] = mat2_);
  ASSERT_EQ(store_.size(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, Push_DuplicateEmpty) {
  ASSERT_NO_THROW(store_["mat1"] = mat1_);
  ASSERT_NO_THROW(store_["mat1"] = mat1_);

  ASSERT_EQ(store_.size(), 1);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResMapTest, GetValues) {
  std::vector<Product::Ptr> vals = filled_store_.Values();
  ASSERT_EQ(vals.size(), 2);
  EXPECT_EQ(vals[0], mat1_);
  EXPECT_EQ(vals[1], mat2_);
}

TEST_F(ResMapTest, GetResValues) {
  std::vector<Resource::Ptr> vals = filled_store_.ResValues();
  std::vector<Resource::Ptr> resvals = ResCast(filled_store_.Values());
  ASSERT_EQ(vals.size(), 2);
  EXPECT_EQ(vals[0], resvals[0]);
  EXPECT_EQ(vals[1], resvals[1]);
}

TEST_F(ResMapTest, SetValues) {
  store_.obj_ids(filled_store_.obj_ids());
  store_.Values(filled_store_.Values());
  std::vector<Product::Ptr> vals = store_.Values();
  ASSERT_EQ(vals.size(), filled_store_.size());
  EXPECT_EQ(vals, filled_store_.Values());
}

TEST_F(ResMapTest, SetResValues) {
  store_.obj_ids(filled_store_.obj_ids());
  store_.ResValues(filled_store_.ResValues());
  std::vector<Resource::Ptr> vals = store_.ResValues();
  ASSERT_EQ(vals.size(), filled_store_.size());
  EXPECT_EQ(vals, filled_store_.ResValues());
}


TEST_F(ResMapTest, Pop) {
  Product::Ptr mat1 = filled_store_.Pop("mat1");
  ASSERT_EQ(filled_store_.size(), 1);
}


}  // namespace toolkit
}  // namespace cyclus
