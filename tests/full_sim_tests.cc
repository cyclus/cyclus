#include <gtest/gtest.h>

#include "test_context.h"

#include "full_sim_tests.h"

using cyclus::TestContext;
using cyclus::Model;

TEST(FullSimTests, Test) {
  TestContext tc;
  
  TestSender* base_sender = new TestSender(tc.get());
  TestSender* sender = dynamic_cast<TestSender*>(base_sender->Clone());
  
  sender->Deploy(sender);

  int nsteps = 2;
  
  tc.timer()->Initialize(tc.get(), nsteps);
  tc.timer()->RunSim(tc.get());

  EXPECT_EQ(nsteps, sender->requests);
  EXPECT_EQ(0, sender->accept);
  
  delete sender;
}
