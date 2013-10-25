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

using cyclus::ExchangeAggregator;
using cyclus::Request;
using cyclus::RequestPortfolio;
using cyclus::Resource;
using cyclus::TestContext;
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ExchangeAggregatorTests: public ::testing::Test {
 protected:
  TestContext tc;
  MockFacility* fac;
  Request<Resource> req1, req2;
  RequestPortfolio<Resource> rp1, rp2, rp3;
  string commod1, commod2;
  
  virtual void SetUp() {
    fac = new MockFacility(tc.get());
    
    commod1 = "1";
    req1.commodity = commod1;
    req1.requester = fac;
    
    commod2 = "2";
    req2.commodity = commod2;
    req2.requester = fac;

    rp1.AddRequest(req1);
    
    rp2.AddRequest(req2);
    
    rp3.AddRequest(req1);
    rp3.AddRequest(req2);
  };
  
  virtual void TearDown() {
    rp1.Clear();
    rp2.Clear();
    rp3.Clear();
    delete fac;
  };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeAggregatorTests, 1Commod) {
  ExchangeAggregator<Resource> agg;
  EXPECT_TRUE(agg.RequestsByCommod().empty());
  
  set< RequestPortfolio<Resource> > requests;
  requests.insert(rp1);
  
  EXPECT_EQ(requests.size(), 1);
  EXPECT_NO_THROW(agg.set_requests(&requests));
  EXPECT_EQ(agg.requests().size(), 1);
  EXPECT_EQ(agg.RequestsByCommod().size(), 1);
  
  EXPECT_EQ(agg.RequestsForCommod(commod1).size(), 1);
  EXPECT_EQ(*agg.RequestsForCommod(commod1).at(0), req1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeAggregatorTests, 2Commod) {
  ExchangeAggregator<Resource> agg;
  EXPECT_TRUE(agg.RequestsByCommod().empty());

  set< RequestPortfolio<Resource> > requests;
  requests.insert(rp1);
  EXPECT_EQ(requests.count(rp2), 0);
  requests.insert(rp2);
  EXPECT_EQ(requests.count(rp2), 1);
  
  EXPECT_EQ(requests.size(), 2);
  EXPECT_NO_THROW(agg.set_requests(&requests));
  EXPECT_EQ(agg.requests().size(), 2);
  EXPECT_EQ(agg.RequestsByCommod().size(), 2);
  
  EXPECT_EQ(agg.RequestsForCommod(commod1).size(), 1);
  EXPECT_EQ(*agg.RequestsForCommod(commod1).at(0), req1);

  EXPECT_EQ(agg.RequestsForCommod(commod2).size(), 1);
  EXPECT_EQ(*agg.RequestsForCommod(commod2).at(0), req2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeAggregatorTests, NCommod) {
  ExchangeAggregator<Resource> agg;
  EXPECT_TRUE(agg.RequestsByCommod().empty());

  set< RequestPortfolio<Resource> > requests;
  requests.insert(rp1);
  requests.insert(rp2);
  requests.insert(rp3);
  
  EXPECT_EQ(requests.size(), 3);
  EXPECT_NO_THROW(agg.set_requests(&requests));
  EXPECT_EQ(agg.requests().size(), requests.size());
  EXPECT_EQ(agg.RequestsByCommod().size(), 2);
  
  EXPECT_EQ(agg.RequestsForCommod(commod1).size(), 2);
  EXPECT_EQ(*agg.RequestsForCommod(commod1).at(0), req1);
  EXPECT_EQ(*agg.RequestsForCommod(commod1).at(1), req1);

  EXPECT_EQ(agg.RequestsForCommod(commod2).size(), 2);
  EXPECT_EQ(*agg.RequestsForCommod(commod2).at(0), req2);
  EXPECT_EQ(*agg.RequestsForCommod(commod2).at(1), req2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeAggregatorTests, 2Fac) {
  MockFacility* fac2 = new MockFacility(tc.get());

  Request<Resource> req;
  req.commodity = commod1;
  req.requester = fac2;
  
  RequestPortfolio<Resource> rp;
  rp.AddRequest(req);

  set< RequestPortfolio<Resource> > requests;
  requests.insert(rp1);
  requests.insert(rp);  
  EXPECT_EQ(requests.size(), 2);

  ExchangeAggregator<Resource> agg;
  EXPECT_NO_THROW(agg.set_requests(&requests));
  EXPECT_EQ(agg.requests().size(), requests.size());
  EXPECT_EQ(agg.RequestsByCommod().size(), 1);
  
  EXPECT_EQ(agg.RequestsForCommod(commod1).size(), 2);
  EXPECT_EQ(*agg.RequestsForCommod(commod1).at(0), req1);
  EXPECT_EQ(*agg.RequestsForCommod(commod1).at(1), req);

  delete fac2;
}
