#include "total_inv_tracker_tests.h"

#include <gtest/gtest.h>

namespace cyclus {
namespace toolkit {


TEST_F(TotalInvTrackerTest, Init) {
  EXPECT_THROW(empty_tracker_.Init({}), ValueError);
  EXPECT_THROW(empty_tracker_.Init({&buf0_, &buf1_}, 0), ValueError);
  EXPECT_THROW(empty_tracker_.Init({&buf0_, &buf1_}, -1), ValueError);
}

TEST_F(TotalInvTrackerTest, quantity) {
  EXPECT_NO_THROW(single_tracker_.quantity());
  EXPECT_NO_THROW(multi_tracker_.quantity());
  EXPECT_EQ(single_tracker_.quantity(), qty1_);
  EXPECT_EQ(multi_tracker_.quantity(), qty1_ + qty2_);
}

TEST_F(TotalInvTrackerTest, capacity) { 
  EXPECT_EQ(single_tracker_.capacity(), 20);
  EXPECT_EQ(single_tracker_.tracker_capacity(), max_inv_size_);
  EXPECT_EQ(multi_tracker_.capacity(), max_inv_size_);
  EXPECT_EQ(multi_tracker_.tracker_capacity(), max_inv_size_);
}

TEST_F(TotalInvTrackerTest, space) {
  EXPECT_EQ(single_tracker_.space(), 20 - qty1_);
  EXPECT_EQ(multi_tracker_.space(), max_inv_size_ - qty1_ - qty2_);
}

TEST_F(TotalInvTrackerTest, buf_space) {
  EXPECT_EQ(multi_tracker_.buf_space(&buf1_), buf1_.space());
  EXPECT_EQ(multi_tracker_.buf_space(&buf2_), max_inv_size_ - qty1_ - qty2_);
}

TEST_F(TotalInvTrackerTest, empty) {
  EXPECT_THROW(empty_tracker_.empty(), ValueError);
  EXPECT_THROW(empty_tracker_.quantity(), ValueError);
  EXPECT_THROW(empty_tracker_.capacity(), ValueError);
  EXPECT_THROW(empty_tracker_.set_capacity(-1), ValueError);
  EXPECT_THROW(empty_tracker_.set_capacity(1000), ValueError);
  EXPECT_THROW(empty_tracker_.space(), ValueError);
}

TEST_F(TotalInvTrackerTest, num_bufs) {
  EXPECT_THROW(empty_tracker_.num_bufs(), ValueError);
  EXPECT_NO_THROW(multi_tracker_.num_bufs());
  EXPECT_EQ(multi_tracker_.num_bufs(), 3);
}

TEST_F(TotalInvTrackerTest, set_capacity) {
  EXPECT_THROW(multi_tracker_.set_capacity(-1), ValueError);
  EXPECT_THROW(multi_tracker_.set_capacity(10), ValueError);
  EXPECT_NO_THROW(multi_tracker_.set_capacity(1000));
}

}  // namespace toolkit
}  // namespace cyclus
