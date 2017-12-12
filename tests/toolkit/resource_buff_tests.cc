#include "resource_buff_tests.h"

#include <gtest/gtest.h>

namespace cyclus {
namespace toolkit {

// Test order MATTERS.  Beware of reordering.

// The "Empty" suffix indicates the test uses the store_ instance of
// ResourceBuff. The "Filled" suffix indicates the test uses the
// filled_store_ instance of ResourceBuff.

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - Getters, Setters, and Property changers - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, set_capacity_ExceptionsEmpty) {
  EXPECT_THROW(store_.set_capacity(neg_cap), ValueError);
  EXPECT_NO_THROW(store_.set_capacity(zero_cap));
  EXPECT_NO_THROW(store_.set_capacity(cap));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, set_capacity_ExceptionsFilled) {
  EXPECT_THROW(filled_store_.set_capacity(low_cap), ValueError);
  EXPECT_NO_THROW(filled_store_.set_capacity(cap));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, GetCapacity_ExceptionsEmpty) {
  ASSERT_NO_THROW(store_.capacity());
  store_.set_capacity(zero_cap);
  ASSERT_NO_THROW(store_.capacity());
  store_.set_capacity(zero_cap);
  ASSERT_NO_THROW(store_.capacity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, GetCapacity_InitialEmpty) {
  EXPECT_DOUBLE_EQ(store_.capacity(), kBuffInfinity);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, Getset_capacityEmpty) {
  store_.set_capacity(zero_cap);
  EXPECT_DOUBLE_EQ(store_.capacity(), zero_cap);

  store_.set_capacity(cap);
  EXPECT_DOUBLE_EQ(store_.capacity(), cap);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, GetSpace_Empty) {
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), kBuffInfinity);

  store_.set_capacity(zero_cap);
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), zero_cap);

  store_.set_capacity(cap);
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), cap);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, GetSpace_Filled) {
  double space = cap - (mat1_->quantity() + mat2_->quantity());
  ASSERT_NO_THROW(filled_store_.space());
  EXPECT_DOUBLE_EQ(filled_store_.space(), space);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, GetQuantity_Empty) {
  ASSERT_NO_THROW(store_.quantity());
  EXPECT_DOUBLE_EQ(store_.quantity(), 0.0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, GetQuantity_Filled) {
  ASSERT_NO_THROW(filled_store_.quantity());
  double quantity = mat1_->quantity() + mat2_->quantity();
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), quantity);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, GetCount_Empty) {
  ASSERT_NO_THROW(store_.count());
  EXPECT_EQ(store_.count(), 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, GetCount_Filled) {
  ASSERT_NO_THROW(filled_store_.count());
  EXPECT_DOUBLE_EQ(filled_store_.count(), 2);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - Removing from buffer  - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, RemoveQty_ExceptionsEmpty) {
  Manifest manifest;
  double qty = cap + overeps;
  ASSERT_THROW(manifest = filled_store_.PopQty(qty), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, RemoveQty_ExceptionsFilled) {
  Manifest manifest;
  double qty = cap + overeps;
  ASSERT_THROW(manifest = store_.PopQty(qty), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, RemoveQty_SingleNoSplit) {
  // pop one no splitting leaving one mat in the store
  Manifest manifest;

  ASSERT_NO_THROW(manifest = filled_store_.PopQty(exact_qty));
  ASSERT_EQ(manifest.size(), 1);
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, RemoveQty_SingleWithSplit) {
  // pop one no splitting leaving one mat in the store

  Manifest manifest;

  double orig_qty = filled_store_.quantity();
  ASSERT_NO_THROW(manifest = filled_store_.PopQty(exact_qty_under));
  ASSERT_EQ(manifest.size(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), exact_qty_under);
  EXPECT_EQ(filled_store_.count(), 2);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), orig_qty - exact_qty_under);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, RemoveQty_DoubleWithSplit) {
  // pop one no splitting leaving one mat in the store

  Manifest manifest;

  double orig_qty = filled_store_.quantity();
  ASSERT_NO_THROW(manifest = filled_store_.PopQty(exact_qty_over));
  ASSERT_EQ(manifest.size(), 2);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity() + \
                   manifest.at(1)->quantity(), exact_qty_over);
  EXPECT_TRUE(filled_store_.count() == 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), orig_qty - exact_qty_over);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, RemoveNum_ExceptionsFilled) {
  Manifest manifest;
  ASSERT_THROW(manifest = filled_store_.PopN(3), ValueError);
  ASSERT_THROW(manifest = filled_store_.PopN(-1), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, RemoveNum_ZeroFilled) {
  Manifest manifest;
  double tot_qty = filled_store_.quantity();

  ASSERT_NO_THROW(manifest = filled_store_.PopN(0));
  ASSERT_EQ(manifest.size(), 0);
  ASSERT_EQ(filled_store_.count(), 2);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), tot_qty);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, RemoveNum_OneFilled) {
  Manifest manifest;

  ASSERT_NO_THROW(manifest = filled_store_.PopN(1));
  ASSERT_EQ(manifest.size(), 1);
  ASSERT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, RemoveNum_TwoFilled) {
  Manifest manifest;

  ASSERT_NO_THROW(manifest = filled_store_.PopN(2));
  ASSERT_EQ(manifest.size(), 2);
  ASSERT_EQ(filled_store_.count(), 0);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_DOUBLE_EQ(manifest.at(1)->quantity(), mat2_->quantity());
  EXPECT_EQ(manifest.at(1), mat2_);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), 0.0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, RemoveOne_Filled) {
  Resource::Ptr mat;

  ASSERT_NO_THROW(mat = filled_store_.Pop());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat1_->quantity());
  EXPECT_EQ(mat, mat1_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());

  ASSERT_NO_THROW(mat = filled_store_.Pop());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat2_->quantity());
  EXPECT_EQ(mat, mat2_);
  EXPECT_EQ(filled_store_.count(), 0);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), 0.0);

  ASSERT_THROW(mat = filled_store_.Pop(), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, PopBack) {
  Resource::Ptr mat;
  ResourceBuff::AccessDir dir = ResourceBuff::BACK;

  ASSERT_NO_THROW(mat = filled_store_.Pop(dir));
  EXPECT_DOUBLE_EQ(mat->quantity(), mat2_->quantity());
  EXPECT_EQ(mat, mat2_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat1_->quantity());

  ASSERT_NO_THROW(mat = filled_store_.Pop(dir));
  EXPECT_DOUBLE_EQ(mat->quantity(), mat1_->quantity());
  EXPECT_EQ(mat, mat1_);
  EXPECT_EQ(filled_store_.count(), 0);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), 0.0);

  ASSERT_THROW(mat = filled_store_.Pop(dir), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - Pushing into buffer - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, Push_Empty) {
  ASSERT_NO_THROW(store_.set_capacity(cap));

  ASSERT_NO_THROW(store_.Push(mat1_));
  ASSERT_EQ(store_.count(), 1);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity());

  ASSERT_NO_THROW(store_.Push(mat2_));
  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, Push_OverCapacityEmpty) {
  ASSERT_NO_THROW(store_.set_capacity(cap));

  ASSERT_NO_THROW(store_.Push(mat1_));
  ASSERT_NO_THROW(store_.Push(mat2_));

  double topush = cap - store_.quantity();
  Resource::Ptr overmat = Product::CreateUntracked(topush + overeps, "food")->Clone();

  ASSERT_THROW(store_.Push(overmat), ValueError);
  ASSERT_EQ(store_.count(), 2);
  ASSERT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());

  overmat = Product::CreateUntracked(topush + undereps, "food")->Clone();
  ASSERT_NO_THROW(store_.Push(overmat));
  ASSERT_EQ(store_.count(), 3);

  double expected = mat1_->quantity() + mat2_->quantity() + overmat->quantity();
  ASSERT_DOUBLE_EQ(store_.quantity(), expected);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, Push_DuplicateEmpty) {
  ASSERT_NO_THROW(store_.set_capacity(cap));

  ASSERT_NO_THROW(store_.Push(mat1_));
  ASSERT_THROW(store_.Push(mat1_), KeyError);

  ASSERT_EQ(store_.count(), 1);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, PushAll_Empty) {
  ASSERT_NO_THROW(store_.set_capacity(cap));
  ASSERT_NO_THROW(store_.PushAll(mats));
  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, PushAll_NoneEmpty) {
  Manifest manifest;
  ASSERT_NO_THROW(store_.set_capacity(cap));
  ASSERT_NO_THROW(store_.PushAll(manifest));
  ASSERT_EQ(store_.count(), 0);
  EXPECT_DOUBLE_EQ(store_.quantity(), 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, PushAll_RetrieveOrderEmpty) {
  Resource::Ptr mat;

  ASSERT_NO_THROW(store_.set_capacity(cap));
  ASSERT_NO_THROW(store_.PushAll(mats));
  ASSERT_NO_THROW(mat = store_.Pop());
  ASSERT_EQ(mat, mat1_);
  ASSERT_NO_THROW(mat = store_.Pop());
  ASSERT_EQ(mat, mat2_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, PushAll_OverCapacityEmpty) {
  ASSERT_NO_THROW(store_.set_capacity(cap));
  ASSERT_NO_THROW(store_.PushAll(mats));

  double topush = cap - store_.quantity();
  Resource::Ptr overmat = Product::CreateUntracked(topush + overeps, "food")->Clone();
  Manifest overmats;
  overmats.push_back(overmat);

  ASSERT_THROW(store_.PushAll(overmats), ValueError);
  ASSERT_EQ(store_.count(), 2);
  ASSERT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());

  overmats.clear();
  overmat = Product::CreateUntracked(topush + undereps, "food")->Clone();
  overmats.push_back(overmat);

  ASSERT_NO_THROW(store_.PushAll(overmats));
  ASSERT_EQ(store_.count(), 3);

  double expected = mat1_->quantity() + mat2_->quantity() + overmat->quantity();
  ASSERT_DOUBLE_EQ(store_.quantity(), expected);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceBuffTest, PushAll_DuplicateEmpty) {
  ASSERT_NO_THROW(store_.set_capacity(2 * cap));

  ASSERT_NO_THROW(store_.PushAll(mats));
  ASSERT_THROW(store_.Push(mat1_), KeyError);
  ASSERT_THROW(store_.Push(mat2_), KeyError);
  ASSERT_THROW(store_.PushAll(mats), KeyError);

  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

}  // namespace toolkit
}  // namespace cyclus
