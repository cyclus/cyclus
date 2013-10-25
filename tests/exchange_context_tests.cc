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
  Request<Resource> req1, req2;
  RequestPortfolio<Resource> rp1, rp2;
  string commod1, commod2;
  
  virtual void SetUp() {
    fac1 = new MockFacility(tc.get());
    fac2 = new MockFacility(tc.get());
    
    commod1 = "commod1";
    req1.commodity = commod1;
    req1.requester = fac1;
    
    commod2 = "commod2";
    req2.commodity = commod2;
    req2.requester = fac2;

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

void test_request_ptrs(const Request<Resource>* p1, const Request<Resource>* p2) {
  EXPECT_EQ(p1->commodity, p2->commodity);
  EXPECT_EQ(p1->target, p2->target);
  EXPECT_EQ(p1->requester, p2->requester);
  EXPECT_TRUE(cyclus::DoubleEq(p1->preference, p2->preference));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeContextTests, AddRequest) {
  ExchangeContext<Resource> context;

  context.AddRequestPortfolio(rp1);
  
  vector< RequestPortfolio<Resource> > rpv;
  rpv.push_back(rp1);
  EXPECT_EQ(context.requests(), rpv);

  const vector< const Request<Resource>* >& rv =
      context.RequestsForCommod(commod1);
  EXPECT_EQ(rv.size(), 1);
  test_request_ptrs(rv[0], &req1);
  EXPECT_EQ(*rv[0], req1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeContextTests, AddRequest2) {
  ExchangeContext<Resource> context;

  Request<Resource> req;
  RequestPortfolio<Resource> rp;
  req.commodity = commod1;
  req.requester = fac2;
  req.preference = 5.0;
  rp.AddRequest(req);
    
  context.AddRequestPortfolio(rp1);
  context.AddRequestPortfolio(rp);

  const vector< const Request<Resource>* >& rv =
      context.RequestsForCommod(commod1);
  EXPECT_EQ(rv.size(), 2);
  test_request_ptrs(rv[0], &req1);
  test_request_ptrs(rv[1], &req);
  // std::cout << "test preference: " << rv[0]->preference << std::endl;
  // std::cout << "test preference: " << rv[1]->preference << std::endl;
  // std::cout << "test preference: " << (*rv[1]).preference << std::endl;
  // std::cout << "test preference: " << (*rv[0]).preference << std::endl;
  // std::cout << "test commodity: " << *rv[0] << std::endl;
  // std::cout << "test commodity: " << rv[1]->commodity << std::endl;
  // std::cout << "test commodity: " << (*rv[1]).commodity << std::endl;
  // std::cout << "test commodity: " << (*rv[0]).commodity << std::endl;
  // EXPECT_EQ((*rv[0]), req1);
  // EXPECT_EQ((*rv[1]), req);
  //  test_request_ptrs(rv.at(0), &req1);
}
//   vector< RequestPortfolio<Resource> > rpv;
//   rpv.push_back(rp1);
//   rpv.push_back(rp);
//   EXPECT_EQ(context.requests(), rpv);
  
//   //  EXPECT_EQ(*rv[0], req);//, req1);
//   //  EXPECT_EQ(*rv[1], req);
//   //  EXPECT_EQ(*rv[0], req1);
//   // EXPECT_EQ(*rv2[1], req1);
// }







// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TEST_F(ExchangeContextTests, 1Commod) {
//   ExchangeContext<Resource> agg;
//   EXPECT_TRUE(agg.RequestsByCommod().empty());
  
//   set< RequestPortfolio<Resource> > requests;
//   requests.insert(rp1);
  
//   EXPECT_EQ(requests.size(), 1);
//   EXPECT_NO_THROW(agg.set_requests(&requests));
//   EXPECT_EQ(agg.requests().size(), 1);
//   EXPECT_EQ(agg.RequestsByCommod().size(), 1);
  
//   EXPECT_EQ(agg.RequestsForCommod(commod1).size(), 1);
//   EXPECT_EQ(*agg.RequestsForCommod(commod1).at(0), req1);
// }

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TEST_F(ExchangeContextTests, 2Commod) {
//   ExchangeContext<Resource> agg;
//   EXPECT_TRUE(agg.RequestsByCommod().empty());

//   set< RequestPortfolio<Resource> > requests;
//   requests.insert(rp1);
//   EXPECT_EQ(requests.count(rp2), 0);
//   requests.insert(rp2);
//   EXPECT_EQ(requests.count(rp2), 1);
  
//   EXPECT_EQ(requests.size(), 2);
//   EXPECT_NO_THROW(agg.set_requests(&requests));
//   EXPECT_EQ(agg.requests().size(), 2);
//   EXPECT_EQ(agg.RequestsByCommod().size(), 2);
  
//   EXPECT_EQ(agg.RequestsForCommod(commod1).size(), 1);
//   EXPECT_EQ(*agg.RequestsForCommod(commod1).at(0), req1);

//   EXPECT_EQ(agg.RequestsForCommod(commod2).size(), 1);
//   EXPECT_EQ(*agg.RequestsForCommod(commod2).at(0), req2);
// }

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TEST_F(ExchangeContextTests, NCommod) {
//   ExchangeContext<Resource> agg;
//   EXPECT_TRUE(agg.RequestsByCommod().empty());

//   set< RequestPortfolio<Resource> > requests;
//   requests.insert(rp1);
//   requests.insert(rp2);
//   requests.insert(rp3);
  
//   EXPECT_EQ(requests.size(), 3);
//   EXPECT_NO_THROW(agg.set_requests(&requests));
//   EXPECT_EQ(agg.requests().size(), requests.size());
//   EXPECT_EQ(agg.RequestsByCommod().size(), 2);
  
//   EXPECT_EQ(agg.RequestsForCommod(commod1).size(), 2);
//   EXPECT_EQ(*agg.RequestsForCommod(commod1).at(0), req1);
//   EXPECT_EQ(*agg.RequestsForCommod(commod1).at(1), req1);

//   EXPECT_EQ(agg.RequestsForCommod(commod2).size(), 2);
//   EXPECT_EQ(*agg.RequestsForCommod(commod2).at(0), req2);
//   EXPECT_EQ(*agg.RequestsForCommod(commod2).at(1), req2);
// }

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TEST_F(ExchangeContextTests, 2Fac) {
//   MockFacility* fac2 = new MockFacility(tc.get());

//   Request<Resource> req;
//   req.commodity = commod1;
//   req.requester = fac2;
  
//   RequestPortfolio<Resource> rp;
//   rp.AddRequest(req);

//   set< RequestPortfolio<Resource> > requests;
//   requests.insert(rp1);
//   requests.insert(rp);  
//   EXPECT_EQ(requests.size(), 2);

//   ExchangeContext<Resource> agg;
//   EXPECT_NO_THROW(agg.set_requests(&requests));
//   EXPECT_EQ(agg.requests().size(), requests.size());
//   EXPECT_EQ(agg.RequestsByCommod().size(), 1);
  
//   EXPECT_EQ(agg.RequestsForCommod(commod1).size(), 2);
//   EXPECT_EQ(*agg.RequestsForCommod(commod1).at(0), req1);
//   EXPECT_EQ(*agg.RequestsForCommod(commod1).at(1), req);

//   delete fac2;
// }
