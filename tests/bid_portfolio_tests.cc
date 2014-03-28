
#include <gtest/gtest.h>

#include <string>
#include <set>

#include "capacity_constraint.h"
#include "error.h"
#include "facility.h"
#include "test_modules/test_facility.h"
#include "request.h"
#include "material.h"
#include "resource_helpers.h"
#include "test_context.h"

#include "bid_portfolio.h" 

using cyclus::Bid;
using cyclus::BidPortfolio;
using cyclus::CapacityConstraint;
using cyclus::Converter;
using cyclus::KeyError;
using cyclus::Request;
using cyclus::Material;
using cyclus::TestContext;
using std::string;
using test_helpers::get_mat;
using test_helpers::get_req;
using test_helpers::TestConverter;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class BidPortfolioTests: public ::testing::Test {
 protected:
  TestContext tc;
  TestFacility* fac1;
  TestFacility* fac2;
  string commod1;
  string commod2;
  
  Request<Material>::Ptr req1;
  Request<Material>::Ptr req2;
  
  virtual void SetUp() {
    fac1 = new TestFacility(tc.get());
    fac2 = new TestFacility(tc.get());
    commod1 = "commod1";
    commod2 = "commod2";
    req1 = get_req(commod1);
    req2 = get_req(commod2);
  };
  
  virtual void TearDown() {};
  
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BidPortfolioTests, RespAdd) {
  
  BidPortfolio<Material>::Ptr rp(new BidPortfolio<Material>());
  EXPECT_EQ(rp->bids().size(), 0);
  Bid<Material>::Ptr r1 = rp->AddBid(req1, get_mat(), fac1);
  EXPECT_EQ(rp->bidder(), fac1);
  EXPECT_EQ(rp->bids().size(), 1);
  EXPECT_EQ(*rp->bids().begin(), r1);

  EXPECT_THROW(rp->AddBid(req2, get_mat(), fac2), KeyError);  
  EXPECT_THROW(rp->AddBid(req2, get_mat(), fac1), KeyError);    
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BidPortfolioTests, Sets) {
  BidPortfolio<Material>::Ptr rp1(new BidPortfolio<Material>());
  BidPortfolio<Material>::Ptr rp2(new BidPortfolio<Material>());
  BidPortfolio<Material>::Ptr rp3(new BidPortfolio<Material>());

  rp3->AddBid(req1, get_mat(), fac1);
  rp3->AddBid(req1, get_mat(), fac1);
  
  std::set< BidPortfolio<Material>::Ptr > bids;
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
TEST_F(BidPortfolioTests, CapAdd) {
  Converter<Material>::Ptr test_converter(new TestConverter());
  CapacityConstraint<Material> c(5, test_converter);
  
  BidPortfolio<Material>::Ptr rp(new BidPortfolio<Material>());
  EXPECT_NO_THROW(rp->AddConstraint(c));
  EXPECT_EQ(*rp->constraints().begin(), c);
}
