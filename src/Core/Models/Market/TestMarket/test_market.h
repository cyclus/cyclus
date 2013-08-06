#if !defined _TESTMARKET_H_
#define _TESTMARKET_H_

#include "market_model.h"
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Market, for testing.
class TestMarket : public cyclus::MarketModel {
  public :
    TestMarket() {}
    virtual ~TestMarket() {
    }
    TestMarket(std::string commod) {
      commodity_ = commod;
    }
    virtual void ReceiveMessage(cyclus::Message::Ptr msg) {
    }
    virtual void Resolve() {
    }
};
#endif
