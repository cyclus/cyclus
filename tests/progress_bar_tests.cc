#include <gtest/gtest.h>
#include <cstdlib>

#include "progress_bar.h"

namespace cyclus {

// Test constants to avoid magic numbers
const int DEFAULT_TOTAL = 100;
const int DEFAULT_WIDTH = 50;
const int DEFAULT_FREQUENCY = 1;
const int TEST_FREQUENCY = 5;
const int LARGE_TOTAL = 200;
const double HALF_PROGRESS = 50.0;
const double QUARTER_PROGRESS = 25.0;
const double FULL_PROGRESS = 100.0;

class ProgressBarTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Save original environment variable
    original_env_ = std::getenv("CYCLUS_PROGRESS_BAR");
  }

  void TearDown() override {
    // Restore original environment variable
    if (original_env_) {
      setenv("CYCLUS_PROGRESS_BAR", original_env_, 1);
    } else {
      unsetenv("CYCLUS_PROGRESS_BAR");
    }
  }

  const char* original_env_;
};

TEST_F(ProgressBarTest, Constructor) {
  ProgressBar pb(DEFAULT_TOTAL, DEFAULT_WIDTH, true, true);
  EXPECT_EQ(DEFAULT_FREQUENCY, pb.GetUpdateFrequency());  // Default frequency
}

TEST_F(ProgressBarTest, UpdateFrequency) {
  ProgressBar pb(DEFAULT_TOTAL);
  pb.SetUpdateFrequency(TEST_FREQUENCY);
  EXPECT_EQ(TEST_FREQUENCY, pb.GetUpdateFrequency());
}

TEST_F(ProgressBarTest, PercentageCalculation) {
  ProgressBar pb(DEFAULT_TOTAL);
  pb.Update(50);
  EXPECT_DOUBLE_EQ(HALF_PROGRESS, pb.GetPercentage());
  
  pb.Update(25);
  EXPECT_DOUBLE_EQ(QUARTER_PROGRESS, pb.GetPercentage());
  
  pb.Update(100);
  EXPECT_DOUBLE_EQ(FULL_PROGRESS, pb.GetPercentage());
}

TEST_F(ProgressBarTest, PercentageWithZeroTotal) {
  ProgressBar pb(0);  // Should be set to 1 internally
  pb.Update(0);
  // Even with a total of 0, the progress bar should still calculate correctly
  EXPECT_DOUBLE_EQ(0.0, pb.GetPercentage());
}

TEST_F(ProgressBarTest, EnvironmentVariableEnable) {
  setenv("CYCLUS_PROGRESS_BAR", "1", 1);
  ProgressBar pb(DEFAULT_TOTAL);
  EXPECT_TRUE(ProgressBar::IsEnabled());
}

TEST_F(ProgressBarTest, EnvironmentVariableDisable) {
  setenv("CYCLUS_PROGRESS_BAR", "0", 1);
  ProgressBar pb(DEFAULT_TOTAL);
  EXPECT_FALSE(ProgressBar::IsEnabled());
}

TEST_F(ProgressBarTest, EnvironmentVariableFalse) {
  setenv("CYCLUS_PROGRESS_BAR", "false", 1);
  ProgressBar pb(DEFAULT_TOTAL);
  EXPECT_FALSE(ProgressBar::IsEnabled());
}

TEST_F(ProgressBarTest, EnvironmentVariableNo) {
  setenv("CYCLUS_PROGRESS_BAR", "no", 1);
  ProgressBar pb(DEFAULT_TOTAL);
  EXPECT_FALSE(ProgressBar::IsEnabled());
}

TEST_F(ProgressBarTest, EnvironmentVariableOff) {
  setenv("CYCLUS_PROGRESS_BAR", "off", 1);
  ProgressBar pb(DEFAULT_TOTAL);
  EXPECT_FALSE(ProgressBar::IsEnabled());
}

TEST_F(ProgressBarTest, EnvironmentVariableUnset) {
  unsetenv("CYCLUS_PROGRESS_BAR");
  ProgressBar pb(DEFAULT_TOTAL);
  EXPECT_TRUE(ProgressBar::IsEnabled());  // Should default to enabled
}

TEST_F(ProgressBarTest, SetTotal) {
  ProgressBar pb(DEFAULT_TOTAL);
  pb.SetTotal(LARGE_TOTAL);
  pb.Update(100);
  EXPECT_DOUBLE_EQ(HALF_PROGRESS, pb.GetPercentage());
}

TEST_F(ProgressBarTest, SetTotalZero) {
  ProgressBar pb(DEFAULT_TOTAL);
  pb.SetTotal(0);  // Should be set to 1 internally
  pb.Update(0);
  EXPECT_DOUBLE_EQ(0.0, pb.GetPercentage());
}

TEST_F(ProgressBarTest, GlobalEnableDisable) {
  ProgressBar::SetEnabled(false);
  EXPECT_FALSE(ProgressBar::IsEnabled());
  
  ProgressBar::SetEnabled(true);
  EXPECT_TRUE(ProgressBar::IsEnabled());
}

TEST_F(ProgressBarTest, UpdateWithDisabledBar) {
  ProgressBar::SetEnabled(false);
  ProgressBar pb(DEFAULT_TOTAL);
  pb.Update(50);  // Should not crash or cause issues
  EXPECT_DOUBLE_EQ(HALF_PROGRESS, pb.GetPercentage());  // Should still calculate correctly
}

}  // namespace cyclus
