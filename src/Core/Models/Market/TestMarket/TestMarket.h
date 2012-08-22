#if !defined _TESTMARKET_H_
#define _TESTMARKET_H_

#include "MarketModel.h"
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Market, for testing.
class TestMarket : public MarketModel {
  public :
    virtual ~TestMarket() {
    }
    TestMarket(std::string commod) {
      commodity_ = commod;
      MarketModel::initSimInteraction(this);
    }
    virtual void receiveMessage(msg_ptr msg) {
    }
    virtual void resolve() {
    }
    virtual void copy(TestMarket* src){
      commodity_ = src->commodity_;
    }
    void copyFreshModel(Model* src){
      copy(dynamic_cast<TestMarket*>(src));
    }
};
#endif
