#include <gtest/gtest.h>

#include <sstream>

#include "indicators.hpp"

namespace cyclus {
namespace {

using indicators::ProgressBar;
using indicators::option::BarWidth;
using indicators::option::MaxProgress;
using indicators::option::ShowPercentage;
using indicators::option::Stream;

TEST(ProgressBarTest, SetProgressUpdatesCurrent) {
  std::stringstream out;
  ProgressBar pb{
      BarWidth{20},
      MaxProgress{100},
      ShowPercentage{true},
      Stream{out},
  };

  pb.set_progress(50);
  EXPECT_EQ(50, pb.current());
}

TEST(ProgressBarTest, TickIncrementsCurrent) {
  std::stringstream out;
  ProgressBar pb{
      BarWidth{20},
      MaxProgress{100},
      ShowPercentage{true},
      Stream{out},
  };

  pb.tick();
  pb.tick();
  EXPECT_EQ(2, pb.current());
}

TEST(ProgressBarTest, CurrentIsClampedByMaxProgress) {
  std::stringstream out;
  ProgressBar pb{
      BarWidth{20},
      MaxProgress{100},
      ShowPercentage{true},
      Stream{out},
  };

  pb.set_progress(150);
  EXPECT_EQ(100, pb.current());
}

TEST(ProgressBarTest, CanBeMarkedCompleted) {
  std::stringstream out;
  ProgressBar pb{
      BarWidth{20},
      MaxProgress{100},
      ShowPercentage{true},
      Stream{out},
  };

  EXPECT_FALSE(pb.is_completed());
  pb.mark_as_completed();
  EXPECT_TRUE(pb.is_completed());
}

}  // namespace
}  // namespace cyclus
