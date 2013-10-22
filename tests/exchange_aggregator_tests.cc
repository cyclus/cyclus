#include <gtest/gtest.h>

#include <string>
#include <set>

#include "mock_facility.h"
#include "request.h"
#include "request_portfolio.h"
#include "resource.h"
#include "test_context.h"

#include "exchange_aggregator.h"

using std::string;
using std::set;

using cyclus::ExchangeAggregator;
using cyclus::Request;
using cyclus::RequestPortfolio;
using cyclus::Resource;
using cyclus::TestContext;
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ExchangeAggregatorTests: public ::testing::Test {
 protected:
  string commod1, commod2;
  Request<Resource> req1, req2;
  RequestPortfolio<Resource> rp1, rp2, rp3;
  set< RequestPortfolio<Resource> > requests;
  TestContext tc;
  MockFacility* fac;
  
  virtual void SetUp() {
    fac = new MockFacility(tc.get());
    commod1 = "1";
    commod2 = "2";
    req1.commodity = commod1;
    req1.requester = fac;
    req2.commodity = commod2;
    req2.requester = fac;

    rp1.AddRequest(req1);
    rp2.AddRequest(req2);
    rp3.AddRequest(req1);
    rp3.AddRequest(req1);
    rp3.AddRequest(req2);
  };
  
  virtual void TearDown() {
    EXPECT_EQ(rp1.requests().size(), 1);
    delete fac;
  };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeAggregatorTests, 1Commod) {
  ExchangeAggregator<Resource> agg;
  EXPECT_TRUE(agg.RequestsByCommod().empty());

  requests.insert(rp1);
  EXPECT_EQ(rp1.requests().size(), requests.begin()->requests().size());
  EXPECT_NO_THROW(agg.set_requests(&requests));
  EXPECT_EQ(agg.requests().size(), 1);
  EXPECT_EQ(agg.RequestsByCommod().size(), 1);
  
  EXPECT_EQ(agg.RequestsForCommod(commod1).size(), 1);
  EXPECT_EQ(*agg.RequestsForCommod(commod1).at(0), req1);
}
