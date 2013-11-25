
#include <gtest/gtest.h>

#include <string>

#include "capacity_constraint.h"
#include "error.h"
#include "facility_model.h"
#include "mock_facility.h"
#include "request.h"
#include "material.h"
#include "resource_helpers.h"
#include "test_context.h"

#include "bid_portfolio.h" 

using cyclus::Bid;
using cyclus::BidPortfolio;
using cyclus::CapacityConstraint;
using cyclus::KeyError;
using cyclus::Request;
using cyclus::Material;
using cyclus::TestContext;
using std::string;
using test_helpers::get_mat;
using test_helpers::get_req;
using test_helpers::converter;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class BidPortfolioTests: public ::testing::Test {
 protected:
  TestContext tc;
  MockFacility* fac1;
  MockFacility* fac2;
  string commod1;
  string commod2;
  
  Request<Material>::Ptr req1;
  Request<Material>::Ptr req2;
  
  virtual void SetUp() {
    fac1 = new MockFacility(tc.get());
    fac2 = new MockFacility(tc.get());
    commod1 = "commod1";
    commod2 = "commod2";
    req1 = get_req(commod1);
    req2 = get_req(commod2);
  };
  
  virtual void TearDown() {
    delete fac1;
    delete fac2;
  };
  
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BidPortfolioTests, RespAdd) {
  Bid<Material>::Ptr r1(new Bid<Material>(req1, get_mat(), fac1));
  
  BidPortfolio<Material> rp;
  EXPECT_EQ(rp.bids().size(), 0);
  EXPECT_NO_THROW(rp.AddBid(r1));
  EXPECT_EQ(rp.bidder(), fac1);
  EXPECT_EQ(rp.bids().size(), 1);
  EXPECT_EQ(*rp.bids().begin(), r1);

  Bid<Material>::Ptr r2(new Bid<Material>(req2, get_mat(), fac2));
  EXPECT_THROW(rp.AddBid(r2), KeyError);  

  Bid<Material>::Ptr r3(new Bid<Material>(req2, get_mat(), fac1));
  EXPECT_THROW(rp.AddBid(r3), KeyError);    
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BidPortfolioTests, Copy) { 
  Bid<Material>::Ptr r1(new Bid<Material>(req1, get_mat(), fac1));
  BidPortfolio<Material> rp;
  EXPECT_NO_THROW(rp.AddBid(r1));
  BidPortfolio<Material> copy(rp);
  EXPECT_EQ(rp, copy);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BidPortfolioTests, CapAdd) {
  CapacityConstraint<Material> c(5, &converter);
  
  BidPortfolio<Material> rp;
  EXPECT_NO_THROW(rp.AddConstraint(c));
  EXPECT_EQ(rp.constraints().count(c), 1);
  EXPECT_EQ(*rp.constraints().begin(), c);
}
