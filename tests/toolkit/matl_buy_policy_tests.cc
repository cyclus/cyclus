
#include <gtest/gtest.h>

#include "composition.h"
#include "material.h"
#include "request.h"

#include "toolkit/matl_buy_policy.h"
#include "toolkit/resource_buff.h"

namespace cyclus {
namespace toolkit {

class TestComp : public Composition {
 public:
  TestComp() {}
};

TEST(MatlBuyPolicyTest, Init) {
  double cap = 5;
  ResourceBuff buff;
  buff.set_capacity(cap);
  MatlBuyPolicy p;

  // defaults
  p.Init(NULL, &buff, "");
  ASSERT_FLOAT_EQ(p.TotalQty(), cap);
  ASSERT_FLOAT_EQ(p.ReqQty(), cap);
  ASSERT_EQ(p.NReq(), 1);

  // exclusive orders
  double quantize = 2.5;
  p.Init(NULL, &buff, "", quantize);
  ASSERT_FLOAT_EQ(p.TotalQty(), cap);
  ASSERT_FLOAT_EQ(p.ReqQty(), quantize);
  ASSERT_EQ(p.NReq(), static_cast<int>(cap / quantize));

  // S,s with nothing in buffer 
  double S = 4, s = 2;
  p.Init(NULL, &buff, "", -1, S, s);
  ASSERT_FLOAT_EQ(p.TotalQty(), S);
  ASSERT_FLOAT_EQ(p.ReqQty(), S);
  ASSERT_EQ(p.NReq(), 1);

  // S,s with something in buffer
  buff.Push(Product::CreateUntracked(s, "bananas"));
  ASSERT_FLOAT_EQ(p.TotalQty(), S - s);
  ASSERT_FLOAT_EQ(p.ReqQty(), S - s);
  ASSERT_EQ(p.NReq(), 1);
}

TEST(MatlBuyPolicyTest, Reqs) {
  double cap = 5;
  ResourceBuff buff;
  buff.set_capacity(cap);
  std::string commod1("foo"), commod2("bar");
  double p2 = 2.5;
  cyclus::Composition::Ptr c1 = cyclus::Composition::Ptr(new TestComp()); 
  cyclus::Composition::Ptr c2 = cyclus::Composition::Ptr(new TestComp()); 
  MatlBuyPolicy p;
  
  // two requests
  p.Init(NULL, &buff, "").Set(commod1, c1).Set(commod2, c2, p2);
  std::set<RequestPortfolio<Material>::Ptr> obs = p.GetMatlRequests();
  ASSERT_EQ(obs.size(), 1);
  ASSERT_EQ((*obs.begin())->requests().size(), 2);
  Request<Material>* req = (*obs.begin())->requests().at(0);
  if (req->commodity() == commod1) {
    ASSERT_FLOAT_EQ(req->preference(), 1.);
    ASSERT_FLOAT_EQ((*obs.begin())->requests().at(1)->preference(), p2);
  } else {
    ASSERT_FLOAT_EQ(req->preference(), p2);
    ASSERT_FLOAT_EQ((*obs.begin())->requests().at(1)->preference(), 1.);
  }
  ASSERT_FALSE(req->exclusive());
  ASSERT_FLOAT_EQ(req->target()->quantity(), cap);
  
  // two portfolios with quantize
  double quantize = 2.5;
  p.Init(NULL, &buff, "", quantize);
  obs = p.GetMatlRequests();
  ASSERT_EQ(obs.size(), 2);
  ASSERT_EQ((*obs.begin())->requests().size(), 2);
  ASSERT_EQ((*(obs.begin()++))->requests().size(), 2);
  req = (*obs.begin())->requests().at(0);
  ASSERT_TRUE(req->exclusive());
  ASSERT_FLOAT_EQ(req->target()->quantity(), quantize);
}

}
}
