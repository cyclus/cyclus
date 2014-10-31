#include "res_buf_tests.h"

#include <gtest/gtest.h>

namespace cyclus {
namespace toolkit {

// The "Empty" suffix indicates the test uses the store_ instance of
// ResBuf. The "Filled" suffix indicates the test uses the
// filled_store_ instance of ResBuf.

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - Getters, Setters, and Property changers - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, set_capacity_ExceptionsEmpty) {
  EXPECT_THROW(store_.capacity(neg_cap), ValueError);
  EXPECT_NO_THROW(store_.capacity(zero_cap));
  EXPECT_NO_THROW(store_.capacity(cap));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, set_capacity_ExceptionsFilled) {
  EXPECT_THROW(filled_store_.capacity(low_cap), ValueError);
  EXPECT_NO_THROW(filled_store_.capacity(cap));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, GetCapacity_ExceptionsEmpty) {
  ASSERT_NO_THROW(store_.capacity());
  store_.capacity(zero_cap);
  ASSERT_NO_THROW(store_.capacity());
  store_.capacity(zero_cap);
  ASSERT_NO_THROW(store_.capacity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, GetCapacity_InitialEmpty) {
  EXPECT_DOUBLE_EQ(store_.capacity(), INFINITY);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, Getset_capacityEmpty) {
  store_.capacity(zero_cap);
  EXPECT_DOUBLE_EQ(store_.capacity(), zero_cap);

  store_.capacity(cap);
  EXPECT_DOUBLE_EQ(store_.capacity(), cap);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, GetSpace_Empty) {
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
TEST_F(ResBufTest, GetSpace_Filled) {
  double space = cap - (mat1_->quantity() + mat2_->quantity());
  ASSERT_NO_THROW(filled_store_.space());
  EXPECT_DOUBLE_EQ(filled_store_.space(), space);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, GetQuantity_Empty) {
  ASSERT_NO_THROW(store_.quantity());
  EXPECT_DOUBLE_EQ(store_.quantity(), 0.0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, GetQuantity_Filled) {
  ASSERT_NO_THROW(filled_store_.quantity());
  double quantity = mat1_->quantity() + mat2_->quantity();
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), quantity);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, GetCount_Empty) {
  ASSERT_NO_THROW(store_.count());
  EXPECT_EQ(store_.count(), 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, GetCount_Filled) {
  ASSERT_NO_THROW(filled_store_.count());
  EXPECT_DOUBLE_EQ(filled_store_.count(), 2);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - Removing from buffer  - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveQty_ExceptionsEmpty) {
  Product::Ptr p;
  double qty = cap + overeps;
  ASSERT_THROW(p = filled_store_.Pop(qty), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveQty_ExceptionsFilled) {
  Product::Ptr p;
  double qty = cap + overeps;
  ASSERT_THROW(p = store_.Pop(qty), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveQty_SingleNoSplit) {
  // pop one no splitting leaving one mat in the store
  Product::Ptr p;

  ASSERT_NO_THROW(p = filled_store_.Pop(exact_qty));
  EXPECT_EQ(p, mat1_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveQty_SingleWithSplit) {
  // pop one no splitting leaving one mat in the store

  Product::Ptr p;

  double orig_qty = filled_store_.quantity();
  ASSERT_NO_THROW(p = filled_store_.Pop(exact_qty_under));
  EXPECT_DOUBLE_EQ(p->quantity(), exact_qty_under);
  EXPECT_EQ(filled_store_.count(), 2);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), orig_qty - exact_qty_under);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveQty_DoubleWithSplit) {
  // pop one no splitting leaving one mat in the store

  Product::Ptr p;

  double orig_qty = filled_store_.quantity();
  ASSERT_NO_THROW(p = filled_store_.Pop(exact_qty_over));
  EXPECT_DOUBLE_EQ(p->quantity(), exact_qty_over);
  EXPECT_TRUE(filled_store_.count() == 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), orig_qty - exact_qty_over);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveNum_ExceptionsFilled) {
  ProdVec manifest;
  ASSERT_THROW(manifest = filled_store_.PopN(3), ValueError);
  ASSERT_THROW(manifest = filled_store_.PopN(-1), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveNum_ZeroFilled) {
  ProdVec manifest;
  double tot_qty = filled_store_.quantity();

  ASSERT_NO_THROW(manifest = filled_store_.PopN(0));
  ASSERT_EQ(manifest.size(), 0);
  ASSERT_EQ(filled_store_.count(), 2);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), tot_qty);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveNum_OneFilled) {
  ProdVec manifest;

  ASSERT_NO_THROW(manifest = filled_store_.PopN(1));
  ASSERT_EQ(manifest.size(), 1);
  ASSERT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveNum_TwoFilled) {
  ProdVec manifest;

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
TEST_F(ResBufTest, RemoveOne_Filled) {
  Product::Ptr mat;

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
TEST_F(ResBufTest, PopBack) {
  Product::Ptr mat;

  ASSERT_NO_THROW(mat = filled_store_.PopBack());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat2_->quantity());
  EXPECT_EQ(mat, mat2_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat1_->quantity());

  ASSERT_NO_THROW(mat = filled_store_.PopBack());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat1_->quantity());
  EXPECT_EQ(mat, mat1_);
  EXPECT_EQ(filled_store_.count(), 0);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), 0.0);

  ASSERT_THROW(mat = filled_store_.PopBack(), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - Pushing into buffer - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, Push_Empty) {
  ASSERT_NO_THROW(store_.capacity(cap));

  ASSERT_NO_THROW(store_.Push(mat1_));
  ASSERT_EQ(store_.count(), 1);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity());

  ASSERT_NO_THROW(store_.Push(mat2_));
  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, Push_OverCapacityEmpty) {
  ASSERT_NO_THROW(store_.capacity(cap));

  ASSERT_NO_THROW(store_.Push(mat1_));
  ASSERT_NO_THROW(store_.Push(mat2_));

  double topush = cap - store_.quantity();
  Product::Ptr overmat = Product::CreateUntracked(topush + overeps, "food");

  ASSERT_THROW(store_.Push(overmat), ValueError);
  ASSERT_EQ(store_.count(), 2);
  ASSERT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());

  overmat = Product::CreateUntracked(topush + undereps, "food");
  ASSERT_NO_THROW(store_.Push(overmat));
  ASSERT_EQ(store_.count(), 3);

  double expected = mat1_->quantity() + mat2_->quantity() + overmat->quantity();
  ASSERT_DOUBLE_EQ(store_.quantity(), expected);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, Push_DuplicateEmpty) {
  ASSERT_NO_THROW(store_.capacity(cap));

  ASSERT_NO_THROW(store_.Push(mat1_));
  ASSERT_THROW(store_.Push(mat1_), KeyError);

  ASSERT_EQ(store_.count(), 1);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, PushAll_Empty) {
  ASSERT_NO_THROW(store_.capacity(cap));
  ASSERT_NO_THROW(store_.Push(mats));
  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, PushAll_ResCast) {
  ResVec rs;
  for (int i = 0; i < mats.size(); ++i) {
    rs.push_back(mats[i]);
  }
  ASSERT_NO_THROW(store_.capacity(cap));
  ASSERT_NO_THROW(store_.Push(rs));
  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, PushAll_NoneEmpty) {
  ProdVec manifest;
  ASSERT_NO_THROW(store_.capacity(cap));
  ASSERT_NO_THROW(store_.Push(manifest));
  ASSERT_EQ(store_.count(), 0);
  EXPECT_DOUBLE_EQ(store_.quantity(), 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, PushAll_RetrieveOrderEmpty) {
  Product::Ptr mat;

  ASSERT_NO_THROW(store_.capacity(cap));
  ASSERT_NO_THROW(store_.Push(mats));
  ASSERT_NO_THROW(mat = store_.Pop());
  ASSERT_EQ(mat, mat1_);
  ASSERT_NO_THROW(mat = store_.Pop());
  ASSERT_EQ(mat, mat2_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, PushAll_OverCapacityEmpty) {
  ASSERT_NO_THROW(store_.capacity(cap));
  ASSERT_NO_THROW(store_.Push(mats));

  double topush = cap - store_.quantity();
  Product::Ptr overmat = Product::CreateUntracked(topush + overeps, "food");
  ProdVec overmats;
  overmats.push_back(overmat);

  ASSERT_THROW(store_.Push(overmats), ValueError);
  ASSERT_EQ(store_.count(), 2);
  ASSERT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());

  overmats.clear();
  overmat = Product::CreateUntracked(topush + undereps, "food");
  overmats.push_back(overmat);

  ASSERT_NO_THROW(store_.Push(overmats));
  ASSERT_EQ(store_.count(), 3);

  double expected = mat1_->quantity() + mat2_->quantity() + overmat->quantity();
  ASSERT_DOUBLE_EQ(store_.quantity(), expected);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, PushAll_DuplicateEmpty) {
  ASSERT_NO_THROW(store_.capacity(2 * cap));

  ASSERT_NO_THROW(store_.Push(mats));
  ASSERT_THROW(store_.Push(mat1_), KeyError);
  ASSERT_THROW(store_.Push(mat2_), KeyError);
  ASSERT_THROW(store_.Push(mats), KeyError);

  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

}  // namespace toolkit
}  // namespace cyclus
