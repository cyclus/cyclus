
#include <gtest/gtest.h>

#include <string>

#include "capacity_constraint.h"
#include "error.h"
#include "facility_model.h"
#include "mock_facility.h"
#include "request.h"
#include "resource.h"
#include "test_context.h"

#include "bid_portfolio.h" 

using std::string;
using cyclus::CapacityConstraint;
using cyclus::KeyError;
using cyclus::Request;
using cyclus::Bid;
using cyclus::BidPortfolio;
using cyclus::Resource;
using cyclus::TestContext;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class BidPortfolioTests: public ::testing::Test {
 protected:
  TestContext tc;
  MockFacility* fac1;
  MockFacility* fac2;
  string commod1;
  string commod2;
  
  Request<Resource>::Ptr req1;
  Request<Resource>::Ptr req2;
  
  virtual void SetUp() {
    fac1 = new MockFacility(tc.get());
    fac2 = new MockFacility(tc.get());
    commod1 = "commod1";
    commod2 = "commod2";
    req1 = Request<Resource>::Ptr(new Request<Resource>());
    req1->commodity = commod1;
    req2 = Request<Resource>::Ptr(new Request<Resource>());
    req2->commodity = commod2;
  };
  
  virtual void TearDown() {
    delete fac1;
    delete fac2;
  };
  
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BidPortfolioTests, RespAdd) {
  Bid<Resource>::Ptr r1 = Bid<Resource>::Ptr(new Bid<Resource>());
  r1->bidder = fac1;
  r1->request = req1;
  
  BidPortfolio<Resource> rp;
  ASSERT_EQ(rp.bids().size(), 0);
  EXPECT_NO_THROW(rp.AddResponse(r1));
  ASSERT_EQ(rp.bidder(), fac1);
  ASSERT_EQ(rp.bids().size(), 1);
  ASSERT_EQ(*rp.bids().begin(), r1);

  Bid<Resource>::Ptr r2 = Bid<Resource>::Ptr(new Bid<Resource>());
  r2->bidder = fac2;
  r2->request = req2;
  EXPECT_THROW(rp.AddResponse(r2), KeyError);  

  Bid<Resource>::Ptr r3 = Bid<Resource>::Ptr(new Bid<Resource>());
  r3->bidder = fac1;
  r3->request = req2;
  EXPECT_THROW(rp.AddResponse(r3), KeyError);    
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BidPortfolioTests, CapAdd) {
  CapacityConstraint<Resource> c;
  
  BidPortfolio<Resource> rp;
  EXPECT_NO_THROW(rp.AddConstraint(c));
  ASSERT_EQ(rp.constraints().count(c), 1);
  ASSERT_EQ(*rp.constraints().begin(), c);
}
