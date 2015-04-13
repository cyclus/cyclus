
#include <gtest/gtest.h>

#include "toolkit/matl_buy_policy.h"
#include "toolkit/resource_buff.h"

namespace cyclus {
namespace toolkit {

TEST(MatlBuyPolicyTest, Init) {

  double cap = 5;
  ResourceBuff buff;
  buff.set_capacity(cap);

  MatlBuyPolicy p1;
  p1.Init(NULL, &buff, "");
  ASSERT_FLOAT_EQ(p1.TotalQty(), cap);
  ASSERT_FLOAT_EQ(p1.ReqQty(), cap);
  ASSERT_FLOAT_EQ(p1.NReq(), 1);
  
}

}
}
