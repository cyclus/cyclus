#if !defined _TESTMARKET_H_
#define _TESTMARKET_H_

#include "MarketModel.h"
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Market, for testing.
class TestMarket : public MarketModel {
  public :
    TestMarket() {}
    virtual ~TestMarket() {
    }
    TestMarket(std::string commod) {
      commodity_ = commod;
    }
    virtual void receiveMessage(msg_ptr msg) {
    }
    virtual void resolve() {
    }
};
#endif
