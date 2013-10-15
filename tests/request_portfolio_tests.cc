
#include <gtest/gtest.h>

#include <string>

#include "capacity_constraint.h"
#include "error.h"
#include "facility_model.h"
#include "generic_resource.h"
#include "mock_facility.h"
#include "request.h"
#include "resource.h"
#include "test_context.h"

#include "request_portfolio.h"

using std::string;
using cyclus::CapacityConstraint;
using cyclus::GenericResource;
using cyclus::KeyError;
using cyclus::Request;
using cyclus::RequestPortfolio;
using cyclus::Resource;
using cyclus::TestContext;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class RequestPortfolioTests: public ::testing::Test {
 protected:
  TestContext tc;
  MockFacility* fac1;
  MockFacility* fac2;

  virtual void SetUp() {
    fac1 = new MockFacility(tc.get());
    fac2 = new MockFacility(tc.get());
  };
  virtual void TearDown() {
    delete fac1;
    delete fac2;
  };
  
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(RequestPortfolioTests, ReqAdd) {
  Request<Resource> r1;
  r1.requester = fac1;
  Request<Resource> r2;
  r2.requester = fac2;
  
  RequestPortfolio<Resource> rp;
  ASSERT_EQ(rp.requests().size(), 0);
  EXPECT_NO_THROW(rp.AddRequest(r1));
  ASSERT_EQ(rp.requester(), fac1);
  ASSERT_EQ(rp.requests().size(), 1);
  ASSERT_EQ(rp.requests()[0], r1);
  EXPECT_THROW(rp.AddRequest(r2), KeyError);  
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(RequestPortfolioTests, CapAdd) {
  CapacityConstraint<Resource> c;
  
  RequestPortfolio<Resource> rp;
  EXPECT_NO_THROW(rp.AddConstraint(c));
  ASSERT_EQ(rp.constraints().count(c), 1);
  ASSERT_EQ(*rp.constraints().begin(), c);
}
