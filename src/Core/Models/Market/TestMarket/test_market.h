#if !defined _TESTMARKET_H_
#define _TESTMARKET_H_

#include "market_model.h"
#include "context.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Market, for testing.
class TestMarket : public cyclus::MarketModel {
 public:
  TestMarket(cyclus::Context* ctx) : cyclus::MarketModel(ctx) {}
  virtual ~TestMarket() { }
  virtual cyclus::Model* clone() {return new TestMarket(context());};
  TestMarket(cyclus::Context* ctx, std::string commod) : cyclus::MarketModel(ctx) {
    commodity_ = commod;
  }
  virtual void ReceiveMessage(cyclus::Message::Ptr msg) { }
  virtual void Resolve() { }
};
#endif
