#include <gtest/gtest.h>

#include "exchange_manager.h"
#include "greedy_solver.h"
#include "material.h"
#include "test_context.h"

using cyclus::ExchangeManager;
using cyclus::GreedySolver;
using cyclus::Material;
using cyclus::TestContext;

TEST(ExManagerTests, NullTest) {
  TestContext tc;
  GreedySolver* solver = new GreedySolver();
  tc.get()->solver(solver);
  ExchangeManager<Material> manager(tc.get());

  EXPECT_NO_THROW(manager.Execute());
}
