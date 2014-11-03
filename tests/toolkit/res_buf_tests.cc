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
  EXPECT_THROW(store_.cap(neg_cap), ValueError);
  EXPECT_NO_THROW(store_.cap(zero_cap));
  EXPECT_NO_THROW(store_.cap(cap));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, set_capacity_ExceptionsFilled) {
  EXPECT_THROW(filled_store_.cap(low_cap), ValueError);
  EXPECT_NO_THROW(filled_store_.cap(cap));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, GetCapacity_ExceptionsEmpty) {
  ASSERT_NO_THROW(store_.cap());
  store_.cap(zero_cap);
  ASSERT_NO_THROW(store_.cap());
  store_.cap(zero_cap);
  ASSERT_NO_THROW(store_.cap());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, GetCapacity_InitialEmpty) {
  EXPECT_DOUBLE_EQ(store_.cap(), INFINITY);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, Getset_capacityEmpty) {
  store_.cap(zero_cap);
  EXPECT_DOUBLE_EQ(store_.cap(), zero_cap);

  store_.cap(cap);
  EXPECT_DOUBLE_EQ(store_.cap(), cap);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, GetSpace_Empty) {
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), INFINITY);

  store_.cap(zero_cap);
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), zero_cap);

  store_.cap(cap);
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
  ASSERT_NO_THROW(store_.qty());
  EXPECT_DOUBLE_EQ(store_.qty(), 0.0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, GetQuantity_Filled) {
  ASSERT_NO_THROW(filled_store_.qty());
  double quantity = mat1_->quantity() + mat2_->quantity();
  EXPECT_DOUBLE_EQ(filled_store_.qty(), quantity);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, GetCount_Empty) {
  ASSERT_NO_THROW(store_.n());
  EXPECT_EQ(store_.n(), 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, GetCount_Filled) {
  ASSERT_NO_THROW(filled_store_.n());
  EXPECT_DOUBLE_EQ(filled_store_.n(), 2);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - Removing from buffer  - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveQty_ExceptionsEmpty) {
  ProdVect manifest;
  double qty = cap + overeps;
  ASSERT_THROW(manifest = filled_store_.PopQty(qty), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveQty_ExceptionsFilled) {
  ProdVect manifest;
  double qty = cap + overeps;
  ASSERT_THROW(manifest = store_.PopQty(qty), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveQty_SingleNoSplit) {
  // pop one no splitting leaving one mat in the store
  ProdVect manifest;

  ASSERT_NO_THROW(manifest = filled_store_.PopQty(exact_qty));
  ASSERT_EQ(manifest.size(), 1);
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_EQ(filled_store_.n(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.qty(), mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveQty_SingleWithSplit) {
  // pop one no splitting leaving one mat in the store

  ProdVect manifest;

  double orig_qty = filled_store_.qty();
  ASSERT_NO_THROW(manifest = filled_store_.PopQty(exact_qty_under));
  ASSERT_EQ(manifest.size(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), exact_qty_under);
  EXPECT_EQ(filled_store_.n(), 2);
  EXPECT_DOUBLE_EQ(filled_store_.qty(), orig_qty - exact_qty_under);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveQty_DoubleWithSplit) {
  // pop one no splitting leaving one mat in the store

  ProdVect manifest;

  double orig_qty = filled_store_.qty();
  ASSERT_NO_THROW(manifest = filled_store_.PopQty(exact_qty_over));
  ASSERT_EQ(manifest.size(), 2);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity() + \
                   manifest.at(1)->quantity(), exact_qty_over);
  EXPECT_TRUE(filled_store_.n() == 1);
  EXPECT_DOUBLE_EQ(filled_store_.qty(), orig_qty - exact_qty_over);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveNum_ExceptionsFilled) {
  ProdVect manifest;
  ASSERT_THROW(manifest = filled_store_.PopN(3), ValueError);
  ASSERT_THROW(manifest = filled_store_.PopN(-1), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveNum_ZeroFilled) {
  ProdVect manifest;
  double tot_qty = filled_store_.qty();

  ASSERT_NO_THROW(manifest = filled_store_.PopN(0));
  ASSERT_EQ(manifest.size(), 0);
  ASSERT_EQ(filled_store_.n(), 2);
  EXPECT_DOUBLE_EQ(filled_store_.qty(), tot_qty);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveNum_OneFilled) {
  ProdVect manifest;

  ASSERT_NO_THROW(manifest = filled_store_.PopN(1));
  ASSERT_EQ(manifest.size(), 1);
  ASSERT_EQ(filled_store_.n(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_DOUBLE_EQ(filled_store_.qty(), mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveNum_TwoFilled) {
  ProdVect manifest;

  ASSERT_NO_THROW(manifest = filled_store_.PopN(2));
  ASSERT_EQ(manifest.size(), 2);
  ASSERT_EQ(filled_store_.n(), 0);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_DOUBLE_EQ(manifest.at(1)->quantity(), mat2_->quantity());
  EXPECT_EQ(manifest.at(1), mat2_);
  EXPECT_DOUBLE_EQ(filled_store_.qty(), 0.0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, RemoveOne_Filled) {
  Product::Ptr mat;

  ASSERT_NO_THROW(mat = filled_store_.Pop());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat1_->quantity());
  EXPECT_EQ(mat, mat1_);
  EXPECT_EQ(filled_store_.n(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.qty(), mat2_->quantity());

  ASSERT_NO_THROW(mat = filled_store_.Pop());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat2_->quantity());
  EXPECT_EQ(mat, mat2_);
  EXPECT_EQ(filled_store_.n(), 0);
  EXPECT_DOUBLE_EQ(filled_store_.qty(), 0.0);

  ASSERT_THROW(mat = filled_store_.Pop(), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, PopBack) {
  Product::Ptr mat;

  ASSERT_NO_THROW(mat = filled_store_.PopBack());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat2_->quantity());
  EXPECT_EQ(mat, mat2_);
  EXPECT_EQ(filled_store_.n(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.qty(), mat1_->quantity());

  ASSERT_NO_THROW(mat = filled_store_.PopBack());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat1_->quantity());
  EXPECT_EQ(mat, mat1_);
  EXPECT_EQ(filled_store_.n(), 0);
  EXPECT_DOUBLE_EQ(filled_store_.qty(), 0.0);

  ASSERT_THROW(mat = filled_store_.PopBack(), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - Pushing into buffer - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, Push_Empty) {
  ASSERT_NO_THROW(store_.cap(cap));

  ASSERT_NO_THROW(store_.Push(mat1_));
  ASSERT_EQ(store_.n(), 1);
  EXPECT_DOUBLE_EQ(store_.qty(), mat1_->quantity());

  ASSERT_NO_THROW(store_.Push(mat2_));
  ASSERT_EQ(store_.n(), 2);
  EXPECT_DOUBLE_EQ(store_.qty(), mat1_->quantity() + mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, Push_OverCapacityEmpty) {
  ASSERT_NO_THROW(store_.cap(cap));

  ASSERT_NO_THROW(store_.Push(mat1_));
  ASSERT_NO_THROW(store_.Push(mat2_));

  double topush = cap - store_.qty();
  Product::Ptr overmat = Product::CreateUntracked(topush + overeps, "food");

  ASSERT_THROW(store_.Push(overmat), ValueError);
  ASSERT_EQ(store_.n(), 2);
  ASSERT_DOUBLE_EQ(store_.qty(), mat1_->quantity() + mat2_->quantity());

  overmat = Product::CreateUntracked(topush + undereps, "food");
  ASSERT_NO_THROW(store_.Push(overmat));
  ASSERT_EQ(store_.n(), 3);

  double expected = mat1_->quantity() + mat2_->quantity() + overmat->quantity();
  ASSERT_DOUBLE_EQ(store_.qty(), expected);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, Push_DuplicateEmpty) {
  ASSERT_NO_THROW(store_.cap(cap));

  ASSERT_NO_THROW(store_.Push(mat1_));
  ASSERT_THROW(store_.Push(mat1_), KeyError);

  ASSERT_EQ(store_.n(), 1);
  EXPECT_DOUBLE_EQ(store_.qty(), mat1_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, PushAll_Empty) {
  ASSERT_NO_THROW(store_.cap(cap));
  ASSERT_NO_THROW(store_.PushAll(mats));
  ASSERT_EQ(store_.n(), 2);
  EXPECT_DOUBLE_EQ(store_.qty(), mat1_->quantity() + mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, PushAll_ResCast) {
  ResVect rs;
  for (int i = 0; i < mats.size(); ++i) {
    rs.push_back(mats[i]);
  }
  ASSERT_NO_THROW(store_.cap(cap));
  ASSERT_NO_THROW(store_.PushAll(rs));
  ASSERT_EQ(store_.n(), 2);
  EXPECT_DOUBLE_EQ(store_.qty(), mat1_->quantity() + mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, PushAll_NoneEmpty) {
  ProdVect manifest;
  ASSERT_NO_THROW(store_.cap(cap));
  ASSERT_NO_THROW(store_.PushAll(manifest));
  ASSERT_EQ(store_.n(), 0);
  EXPECT_DOUBLE_EQ(store_.qty(), 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, PushAll_RetrieveOrderEmpty) {
  Product::Ptr mat;

  ASSERT_NO_THROW(store_.cap(cap));
  ASSERT_NO_THROW(store_.PushAll(mats));
  ASSERT_NO_THROW(mat = store_.Pop());
  ASSERT_EQ(mat, mat1_);
  ASSERT_NO_THROW(mat = store_.Pop());
  ASSERT_EQ(mat, mat2_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, PushAll_OverCapacityEmpty) {
  ASSERT_NO_THROW(store_.cap(cap));
  ASSERT_NO_THROW(store_.PushAll(mats));

  double topush = cap - store_.qty();
  Product::Ptr overmat = Product::CreateUntracked(topush + overeps, "food");
  ProdVect overmats;
  overmats.push_back(overmat);

  ASSERT_THROW(store_.PushAll(overmats), ValueError);
  ASSERT_EQ(store_.n(), 2);
  ASSERT_DOUBLE_EQ(store_.qty(), mat1_->quantity() + mat2_->quantity());

  overmats.clear();
  overmat = Product::CreateUntracked(topush + undereps, "food");
  overmats.push_back(overmat);

  ASSERT_NO_THROW(store_.PushAll(overmats));
  ASSERT_EQ(store_.n(), 3);

  double expected = mat1_->quantity() + mat2_->quantity() + overmat->quantity();
  ASSERT_DOUBLE_EQ(store_.qty(), expected);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResBufTest, PushAll_DuplicateEmpty) {
  ASSERT_NO_THROW(store_.cap(2 * cap));

  ASSERT_NO_THROW(store_.PushAll(mats));
  ASSERT_THROW(store_.Push(mat1_), KeyError);
  ASSERT_THROW(store_.Push(mat2_), KeyError);
  ASSERT_THROW(store_.PushAll(mats), KeyError);

  ASSERT_EQ(store_.n(), 2);
  EXPECT_DOUBLE_EQ(store_.qty(), mat1_->quantity() + mat2_->quantity());
}

}  // namespace toolkit
}  // namespace cyclus
