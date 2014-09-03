#include <gtest/gtest.h>

#include "agent.h"

#include "test_context.h"
#include "test_agents/test_agent.h"

namespace cyclus {

TEST(AgentClassTests, InChain) {
  TestContext tc;

  Agent* child = new TestAgent(tc.get());
  Agent* parent = new TestAgent(tc.get());
  Agent* grandparent = new TestAgent(tc.get());
  
  EXPECT_FALSE(grandparent->InChain(child));
  EXPECT_FALSE(grandparent->InChain(parent));
  EXPECT_FALSE(parent->InChain(child));
  EXPECT_FALSE(parent->InChain(grandparent));
  EXPECT_FALSE(child->InChain(parent));
  EXPECT_FALSE(child->InChain(grandparent));

  parent->Build(grandparent);
  child->Build(parent);
  
  EXPECT_TRUE(grandparent->InChain(child));
  EXPECT_TRUE(grandparent->InChain(parent));
  EXPECT_TRUE(parent->InChain(child));
  EXPECT_FALSE(parent->InChain(grandparent));
  EXPECT_FALSE(child->InChain(parent));
  EXPECT_FALSE(child->InChain(grandparent));  
}

} // namespace cyclus
