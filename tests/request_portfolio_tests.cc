
#include <gtest/gtest.h>

#include <string>
#include <set>

#include "capacity_constraint.h"
#include "error.h"
#include "facility_model.h"
#include "generic_resource.h"
#include "material.h"
#include "mock_facility.h"
#include "request.h"
#include "resource_helpers.h"
#include "test_context.h"

#include "request_portfolio.h"

using std::set;
using std::string;

using cyclus::CapacityConstraint;
using cyclus::Converter;
using cyclus::GenericResource;
using cyclus::KeyError;
using cyclus::Material;
using cyclus::Request;
using cyclus::RequestPortfolio;
using cyclus::TestContext;
using test_helpers::get_mat;
using test_helpers::TestConverter;

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
  Request<Material>::Ptr r1 = Request<Material>::Create(get_mat(), fac1);
  // a different requester
  Request<Material>::Ptr r2 = Request<Material>::Create(get_mat(), fac2);
  // some different quantity
  Request<Material>::Ptr r3 = Request<Material>::Create(get_mat(92235, 150051.0), fac1);
  
  RequestPortfolio<Material>::Ptr rp(new RequestPortfolio<Material>());
  EXPECT_EQ(rp->requests().size(), 0);
  EXPECT_NO_THROW(rp->AddRequest(r1));
  EXPECT_EQ(rp->requester(), fac1);
  EXPECT_EQ(rp->requests().size(), 1);
  EXPECT_EQ(rp->qty(), get_mat()->quantity());
  EXPECT_EQ(rp->requests()[0], r1);
  EXPECT_THROW(rp->AddRequest(r2), KeyError);
  EXPECT_THROW(rp->AddRequest(r3), KeyError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(RequestPortfolioTests, CapAdd) {
  Converter<Material>::Ptr test_converter(new TestConverter());
  CapacityConstraint<Material> c(5, test_converter);
  
  RequestPortfolio<Material>::Ptr rp(new RequestPortfolio<Material>());
  EXPECT_NO_THROW(rp->AddConstraint(c));
  EXPECT_EQ(rp->constraints().count(c), 1);
  EXPECT_EQ(*rp->constraints().begin(), c);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(RequestPortfolioTests, Sets) {
  RequestPortfolio<Material>::Ptr rp1(new RequestPortfolio<Material>());
  RequestPortfolio<Material>::Ptr rp2(new RequestPortfolio<Material>());
  RequestPortfolio<Material>::Ptr rp3(new RequestPortfolio<Material>());
  
  std::string commod1 = "1";
  Request<Material>::Ptr req1 = Request<Material>::Create(get_mat(), fac1, commod1);
  
  std::string commod2 = "2";
  Request<Material>::Ptr req2 = Request<Material>::Create(get_mat(), fac1, commod2);

  rp1->AddRequest(req1);
    
  rp2->AddRequest(req2);

  rp3->AddRequest(req1);
  rp3->AddRequest(req2);
  
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
