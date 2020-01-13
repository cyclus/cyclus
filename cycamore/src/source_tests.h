#ifndef CYCAMORE_SRC_SOURCE_TESTS_H_
#define CYCAMORE_SRC_SOURCE_TESTS_H_
#include "source.h"

#include <gtest/gtest.h>

#include <boost/shared_ptr.hpp>

#include "agent_tests.h"
#include "context.h"
#include "exchange_context.h"
#include "facility_tests.h"
#include "material.h"

namespace cycamore {

class SourceTest : public ::testing::Test {
 public:
  cyclus::TestContext tc;
  TestFacility* trader;
  cycamore::Source* src_facility;
  std::string commod, recipe_name;
  double capacity;
  cyclus::Composition::Ptr recipe;

  virtual void SetUp();
  virtual void TearDown();
  void InitParameters();
  void SetUpSource();

  std::string outrecipe(cycamore::Source* s) { return s->outrecipe; }
  std::string outcommod(cycamore::Source* s) { return s->outcommod; }
  double throughput(cycamore::Source* s) { return s->throughput; }

  void outrecipe(cycamore::Source* s, std::string recipe) {
    s->outrecipe = recipe;
  }
  void outcommod(cycamore::Source* s, std::string commod) {
    s->outcommod = commod;
  }
  void throughput(cycamore::Source* s, double val) { s->throughput = val; }

  boost::shared_ptr<cyclus::ExchangeContext<cyclus::Material> > GetContext(
      int nreqs, std::string commodity);
};

} // namespace cycamore

#endif  // CYCAMORE_SRC_SOURCE_TESTS_H_
