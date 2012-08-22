#if !defined _TESTMARKET_H_
#define _TESTMARKET_H_

#include "MarketModel.h"
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Market, for testing.
class TestMarket : public MarketModel {
  public :
    TestMarket() ;
    TestMarket(std::string commod) ;
    virtual void receiveMessage(msg_ptr msg) ;
    virtual void resolve() ;
    virtual void copy(TestMarket* src);
    void copyFreshModel(Model* src);
};
#endif
