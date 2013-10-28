
#include <gtest/gtest.h>

#include <string>
#include <set>

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
using std::set;

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
TEST_F(RequestPortfolioTests, ReqAdd){ 
  Request<Resource>::Ptr r1 = Request<Resource>::Ptr(new Request<Resource>());
  r1->requester = fac1;
  Request<Resource>::Ptr r2 = Request<Resource>::Ptr(new Request<Resource>());
  r2->requester = fac2;
  
  RequestPortfolio<Resource> rp;
  ASSERT_EQ(rp.requests().size(), 0);
  EXPECT_NO_THROW(rp.AddRequest(r1));
  ASSERT_EQ(rp.requester(), fac1);
  ASSERT_EQ(rp.requests().size(), 1);
  ASSERT_EQ(rp.requests()[0], r1);
  EXPECT_THROW(rp.AddRequest(r2), KeyError);

  rp.Clear();
  ASSERT_EQ(rp.requests().size(), 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(RequestPortfolioTests, CapAdd) {
  CapacityConstraint<Resource> c;
  
  RequestPortfolio<Resource> rp;
  EXPECT_NO_THROW(rp.AddConstraint(c));
  ASSERT_EQ(rp.constraints().count(c), 1);
  ASSERT_EQ(*rp.constraints().begin(), c);

  rp.Clear();
  ASSERT_EQ(rp.constraints().size(), 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(RequestPortfolioTests, Sets) {
  Request<Resource>::Ptr req1 = Request<Resource>::Ptr(new Request<Resource>());
  Request<Resource>::Ptr req2 = Request<Resource>::Ptr(new Request<Resource>());
  RequestPortfolio<Resource> rp1, rp2, rp3;
  string commod1, commod2;
  
  commod1 = "1";
  req1->commodity = commod1;
  req1->requester = fac1;
    
  commod2 = "2";
  req2->commodity = commod2;
  req2->requester = fac1;

  rp1.AddRequest(req1);
    
  rp2.AddRequest(req2);

  rp3.AddRequest(req1);
  rp3.AddRequest(req2);

  EXPECT_NE(rp1, rp2);
  EXPECT_NE(rp2, rp3);
  EXPECT_NE(rp3, rp1);

  EXPECT_NE(rp1.id(), rp2.id());
  EXPECT_NE(rp2.id(), rp3.id());
  EXPECT_NE(rp3.id(), rp1.id());
  
  set< RequestPortfolio<Resource> > requests;
  EXPECT_EQ(requests.size(), 0);
  EXPECT_EQ(requests.count(rp1), 0);
  EXPECT_EQ(requests.count(rp2), 0);
  EXPECT_EQ(requests.count(rp3), 0);

  requests.insert(rp1);
  EXPECT_EQ(requests.size(), 1);
  EXPECT_EQ(requests.count(rp1), 1);
  EXPECT_EQ(requests.count(rp2), 0);
  EXPECT_EQ(requests.count(rp3), 0);
  
  requests.insert(rp2);
  EXPECT_EQ(requests.size(), 2);
  EXPECT_EQ(requests.count(rp1), 1);
  EXPECT_EQ(requests.count(rp2), 1);
  EXPECT_EQ(requests.count(rp3), 0);
  
  requests.insert(rp3);
  EXPECT_EQ(requests.size(), 3);
  EXPECT_EQ(requests.count(rp1), 1);
  EXPECT_EQ(requests.count(rp2), 1);
  EXPECT_EQ(requests.count(rp3), 1);
}
