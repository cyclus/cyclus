#include <gtest/gtest.h>

#include <string>
#include <set>
#include <vector>

#include "mock_facility.h"
#include "request.h"
#include "request_portfolio.h"
#include "resource.h"
#include "test_context.h"

#include "exchange_context.h"

using std::string;
using std::set;
using std::vector;

using cyclus::ExchangeContext;
using cyclus::Request;
using cyclus::RequestPortfolio;
using cyclus::Resource;
using cyclus::TestContext;
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ExchangeContextTests: public ::testing::Test {
 protected:
  TestContext tc;
  MockFacility* fac1;
  MockFacility* fac2;
  Request<Resource>::Ptr req1, req2;
  RequestPortfolio<Resource> rp1, rp2;
  string commod1, commod2;
  
  virtual void SetUp() {
    fac1 = new MockFacility(tc.get());
    fac2 = new MockFacility(tc.get());

    commod1 = "commod1";
    req1 = Request<Resource>::Ptr(new Request<Resource>());
    req1->commodity = commod1;
    req1->requester = fac1;
    
    req2 = Request<Resource>::Ptr(new Request<Resource>());
    req2->commodity = commod1;
    req2->requester = fac2;

    rp1.AddRequest(req1);    
    rp2.AddRequest(req2);
  };
  
  virtual void TearDown() {
    rp1.Clear();
    rp2.Clear();
    delete fac1;
    delete fac2;
  };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeContextTests, Empty) {
  ExchangeContext<Resource> context;
  EXPECT_TRUE(context.requests().empty());
  EXPECT_TRUE(context.RequestsForCommod(commod2).empty());
  EXPECT_TRUE(context.RequestsForCommod(commod2).empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeContextTests, AddRequest1) {
  // 1 request for 1 commod
  ExchangeContext<Resource> context;
    
  context.AddRequestPortfolio(rp1);

  std::vector<RequestPortfolio<Resource> > vp;
  vp.push_back(rp1);
  EXPECT_EQ(vp, context.requests());
  
  EXPECT_EQ(1, context.RequestsForCommod(commod1).size());  
  std::vector<Request<Resource>::Ptr> vr;
  vr.push_back(req1);
  EXPECT_EQ(vr, context.RequestsForCommod(commod1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeContextTests, AddRequest2) {
  // 2 requests for 1 commod
  ExchangeContext<Resource> context;
    
  context.AddRequestPortfolio(rp1);
  context.AddRequestPortfolio(rp2);

  std::vector<RequestPortfolio<Resource> > vp;
  vp.push_back(rp1);
  vp.push_back(rp2);
  EXPECT_EQ(vp, context.requests());
  
  EXPECT_EQ(2, context.RequestsForCommod(commod1).size());  
  std::vector<Request<Resource>::Ptr> vr;
  vr.push_back(req1);
  vr.push_back(req2);
  EXPECT_EQ(vr, context.RequestsForCommod(commod1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeContextTests, AddRequest3) {
  // 2 requests for 2 commod
  Request<Resource>::Ptr req = Request<Resource>::Ptr(new Request<Resource>());
  req->commodity = commod2;
  req->requester = fac1;
  rp1.AddRequest(req);
  
  ExchangeContext<Resource> context;
    
  context.AddRequestPortfolio(rp1);
  
  EXPECT_EQ(1, context.RequestsForCommod(commod1).size());
  EXPECT_EQ(1, context.RequestsForCommod(commod2).size());
  
  std::vector<Request<Resource>::Ptr> vr;
  vr.push_back(req1);
  EXPECT_EQ(vr, context.RequestsForCommod(commod1));

  vr.clear();
  vr.push_back(req);
  EXPECT_EQ(vr, context.RequestsForCommod(commod2));  
}
