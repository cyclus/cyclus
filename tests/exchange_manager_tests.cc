#include <gtest/gtest.h>

#include "greedy_solver.h"
#include "material.h"
#include "test_context.h"

#include "exchange_manager.h"

using cyclus::ExchangeManager;
using cyclus::GreedySolver;
using cyclus::Material;
using cyclus::TestContext;

TEST(ExManagerTests, NullTest) {
  TestContext tc;
  GreedySolver solver;
  ExchangeManager<Material> manager(tc.get(), &solver);

  EXPECT_NO_THROW(manager.Execute());
}
