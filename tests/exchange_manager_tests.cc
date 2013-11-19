#include <gtest/gtest.h>

#include "material.h"
#include "test_context.h"
#include "greedy_solver.h"

#include "exchange_manager.h"

using cyclus::TestContext;
using cyclus::GreedySolver;
using cyclus::ExchangeManager;
using cyclus::Material;

TEST(ExManagerTests, NullTest) {
  TestContext tc;
  GreedySolver solver;
  ExchangeManager<Material> manager(tc.get(), &solver);

  EXPECT_NO_THROW(manager.Execute());
}
