
#include <gtest/gtest.h>

#include <string>
#include <set>

#include "capacity_constraint.h"
#include "error.h"
#include "facility.h"
#include "product.h"
#include "material.h"
#include "test_modules/test_facility.h"
#include "request.h"
#include "resource_helpers.h"
#include "test_context.h"

#include "request_portfolio.h"

using std::set;
using std::string;

using cyclus::CapacityConstraint;
using cyclus::Converter;
using cyclus::Product;
using cyclus::KeyError;
using cyclus::Material;
using cyclus::Request;
using cyclus::RequestPortfolio;
using cyclus::TestContext;
using rsrc_helpers::get_mat;
using rsrc_helpers::TestConverter;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class RequestPortfolioTests: public ::testing::Test {
 protected:
  TestContext tc;
  TestFacility* fac1;
  TestFacility* fac2;

  virtual void SetUp() {
    fac1 = new TestFacility(tc.get());
    fac2 = new TestFacility(tc.get());
  };
  
  virtual void TearDown() {
  };
  
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(RequestPortfolioTests, ReqAdd){ 
  RequestPortfolio<Material>::Ptr rp(new RequestPortfolio<Material>());
  EXPECT_EQ(rp->requests().size(), 0);
  Request<Material>::Ptr r1 = rp->AddRequest(get_mat(), fac1);
  EXPECT_EQ(rp->requester(), fac1);
  EXPECT_EQ(rp->requests().size(), 1);
  EXPECT_EQ(rp->qty(), get_mat()->quantity());
  EXPECT_EQ(rp->requests()[0], r1);
  EXPECT_THROW(rp->AddRequest(get_mat(), fac2), KeyError); // a different requester
  EXPECT_THROW(rp->AddRequest(get_mat(92235, 150051.0), fac1), KeyError); // some different quantity
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(RequestPortfolioTests, CapAdd) {
  Converter<Material>::Ptr test_converter(new TestConverter());
  CapacityConstraint<Material> c(5, test_converter);
  
  RequestPortfolio<Material>::Ptr rp(new RequestPortfolio<Material>());
  EXPECT_NO_THROW(rp->AddConstraint(c));
  EXPECT_EQ(*rp->constraints().begin(), c);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(RequestPortfolioTests, Sets) {
  RequestPortfolio<Material>::Ptr rp1(new RequestPortfolio<Material>());
  RequestPortfolio<Material>::Ptr rp2(new RequestPortfolio<Material>());
  RequestPortfolio<Material>::Ptr rp3(new RequestPortfolio<Material>());
  
  set<RequestPortfolio<Material>::Ptr> requests;
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
