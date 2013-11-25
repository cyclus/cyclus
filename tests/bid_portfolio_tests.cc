
#include <gtest/gtest.h>

#include <string>
#include <set>

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
TEST_F(BidPortfolioTests, Sets) {
  BidPortfolio<Material> rp1, rp2, rp3;
  
  Bid<Material>::Ptr bid1(new Bid<Material>(req1, get_mat(), fac1));
  Bid<Material>::Ptr bid2(new Bid<Material>(req1, get_mat(), fac1));

  rp1.AddBid(bid1);
    
  rp2.AddBid(bid2);

  rp3.AddBid(bid1);
  rp3.AddBid(bid2);

  EXPECT_NE(rp1, rp2);
  EXPECT_NE(rp2, rp3);
  EXPECT_NE(rp3, rp1);

  EXPECT_NE(rp1.id(), rp2.id());
  EXPECT_NE(rp2.id(), rp3.id());
  EXPECT_NE(rp3.id(), rp1.id());
  
  std::set< BidPortfolio<Material> > bids;
  EXPECT_EQ(bids.size(), 0);
  EXPECT_EQ(bids.count(rp1), 0);
  EXPECT_EQ(bids.count(rp2), 0);
  EXPECT_EQ(bids.count(rp3), 0);

  bids.insert(rp1);
  EXPECT_EQ(bids.size(), 1);
  EXPECT_EQ(bids.count(rp1), 1);
  EXPECT_EQ(bids.count(rp2), 0);
  EXPECT_EQ(bids.count(rp3), 0);
  
  bids.insert(rp2);
  EXPECT_EQ(bids.size(), 2);
  EXPECT_EQ(bids.count(rp1), 1);
  EXPECT_EQ(bids.count(rp2), 1);
  EXPECT_EQ(bids.count(rp3), 0);
  
  bids.insert(rp3);
  EXPECT_EQ(bids.size(), 3);
  EXPECT_EQ(bids.count(rp1), 1);
  EXPECT_EQ(bids.count(rp2), 1);
  EXPECT_EQ(bids.count(rp3), 1);
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
