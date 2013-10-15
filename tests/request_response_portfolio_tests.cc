
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

#include "request_response_portfolio.h" 

using std::string;
using cyclus::CapacityConstraint;
using cyclus::GenericResource;
using cyclus::KeyError;
using cyclus::Request;
using cyclus::RequestResponse;
using cyclus::ResponsePortfolio;
using cyclus::Resource;
using cyclus::TestContext;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class RequestResponsePortfolioTests: public ::testing::Test {
 protected:
  TestContext tc;
  MockFacility* fac1;
  MockFacility* fac2;
  string commod1;
  string commod2;
  
  virtual void SetUp() {
    fac1 = new MockFacility(tc.get());
    fac2 = new MockFacility(tc.get());
    commod1 = "commod1";
    commod2 = "commod2";
  };
  
  virtual void TearDown() {
    delete fac1;
    delete fac2;
  };
  
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(RequestResponsePortfolioTests, ReqAdd) {
  ResponsePortfolio<Resource> rp;
  RequestResponse<Resource> r1;
  r1.responder = fac1;
  RequestResponse<Resource> r2;
  r2.responder = fac2;
  
  // ASSERT_EQ(rp.requests().size(), 0);
  // EXPECT_NO_THROW(rp.AddRequestResponse(r1));
  // ASSERT_EQ(rp.requester(), fac1);
  // ASSERT_EQ(rp.requests().size(), 1);
  // ASSERT_EQ(rp.requests()[0], r1);
  // EXPECT_THROW(rp.AddRequestResponse(r2), KeyError);  
}
