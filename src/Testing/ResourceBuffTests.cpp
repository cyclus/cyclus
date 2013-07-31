
#include <gtest/gtest.h>

#include "ResourceBuffTests.h"

// Test order MATTERS.  Beware of reordering.

// The "Empty" suffix indicates the test uses the store_ instance of
// ResourceBuff. The "Filled" suffix indicates the test uses the
// filled_store_ instance of ResourceBuff.

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
//- - - - - - - Getters, Setters, and Property changers - - - - - - - - - - - -    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, SetCapacity_ExceptionsEmpty) {
  EXPECT_THROW(store_.setCapacity(neg_cap), cyclus::ValueError);
  EXPECT_NO_THROW(store_.setCapacity(zero_cap));
  EXPECT_NO_THROW(store_.setCapacity(cap));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, SetCapacity_ExceptionsFilled) {
  EXPECT_THROW(filled_store_.setCapacity(low_cap), cyclus::ValueError);
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
//- - - - - - Removing from buffer  - - - - - - - - - - - - - - - - - - - - - -    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveQty_ExceptionsEmpty) {
  using cyclus::Manifest;
  using cyclus::CycNegQtyException;
  Manifest manifest;
  double qty = cap + overeps;
  ASSERT_THROW(manifest = filled_store_.popQty(qty), ValueError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveQty_ExceptionsFilled) {
  using cyclus::Manifest;
  using cyclus::CycNegQtyException;
  Manifest manifest;
  double qty = cap + overeps;
  ASSERT_THROW(manifest = store_.popQty(qty), ValueError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveQty_NoSplitExactFilled) {
  using cyclus::Manifest;
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
  using cyclus::Manifest;
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
  using cyclus::Manifest;
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
  using cyclus::Manifest;
  Manifest manifest;
  double store_final = mat1_->quantity() + mat2_->quantity() - over_qty;

  ASSERT_NO_THROW(manifest = filled_store_.popQty(over_qty));
  ASSERT_EQ(manifest.size(), 2);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_NEAR(manifest.at(1)->quantity(), overeps, cyclus::eps_rsrc()); // not sure why DOUBLE_EQ doesn't work
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_EQ(manifest.at(1), mat2_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), store_final);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveQty_SplitUnderFilled) {
  // pop one with splitting leaving two mats in the store
  using cyclus::Manifest;
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
  using cyclus::Manifest;
  using cyclus::CycNegQtyException;
  Manifest manifest;
  ASSERT_THROW(manifest = filled_store_.popNum(3), ValueError);
  ASSERT_THROW(manifest = filled_store_.popNum(-1), ValueError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveNum_ZeroFilled) {
  using cyclus::Manifest;
  Manifest manifest;
  double tot_qty = filled_store_.quantity();

  ASSERT_NO_THROW(manifest = filled_store_.popNum(0));
  ASSERT_EQ(manifest.size(), 0);
  ASSERT_EQ(filled_store_.count(), 2);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), tot_qty);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, RemoveNum_OneFilled) {
  using cyclus::Manifest;
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
  using cyclus::Manifest;
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
  using cyclus::rsrc_ptr;
  using cyclus::CycNegQtyException;
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

  ASSERT_THROW(mat = filled_store_.popOne(), ValueError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
//- - - - - - Pushing into buffer - - - - - - - - - - - - - - - - - - - - - - -    
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
  using cyclus::rsrc_ptr;
  using cyclus::CycDupResException;
  using cyclus::CycOverCapException;
  ASSERT_NO_THROW(store_.setCapacity(cap));

  ASSERT_NO_THROW(store_.pushOne(mat1_));
  ASSERT_NO_THROW(store_.pushOne(mat2_));

  double topush = cap - store_.quantity();
  rsrc_ptr overmat = mat1_->clone();
  overmat->setQuantity(topush + overeps);

  ASSERT_THROW(store_.pushOne(overmat), ValueError);
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
  using cyclus::CycDupResException;
  ASSERT_NO_THROW(store_.setCapacity(cap));

  ASSERT_NO_THROW(store_.pushOne(mat1_));
  ASSERT_THROW(store_.pushOne(mat1_), KeyError);

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
  using cyclus::Manifest;
  Manifest manifest;
  ASSERT_NO_THROW(store_.setCapacity(cap));
  ASSERT_NO_THROW(store_.pushAll(manifest));
  ASSERT_EQ(store_.count(), 0);
  EXPECT_DOUBLE_EQ(store_.quantity(), 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ResourceBuffTest, PushAll_RetrieveOrderEmpty) {
  using cyclus::rsrc_ptr;
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
  using cyclus::rsrc_ptr;
  using cyclus::Manifest;
  using cyclus::CycDupResException;
  using cyclus::CycOverCapException;
  ASSERT_NO_THROW(store_.setCapacity(cap));
  ASSERT_NO_THROW(store_.pushAll(mats));

  double topush = cap - store_.quantity();
  rsrc_ptr overmat = mat1_->clone();
  overmat->setQuantity(topush + overeps);
  Manifest overmats;
  overmats.push_back(overmat);

  ASSERT_THROW(store_.pushAll(overmats), ValueError);
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
  using cyclus::CycDupResException;
  ASSERT_NO_THROW(store_.setCapacity(2 * cap));

  ASSERT_NO_THROW(store_.pushAll(mats));
  ASSERT_THROW(store_.pushOne(mat1_), KeyError);
  ASSERT_THROW(store_.pushOne(mat2_), KeyError);
  ASSERT_THROW(store_.pushAll(mats), KeyError);

  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

