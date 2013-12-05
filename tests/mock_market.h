#ifndef CYCLUS_TESTS_MOCK_MARKET_H_
#define CYCLUS_TESTS_MOCK_MARKET_H_

#include "context.h"
#include "market_model.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MockMarket : public cyclus::MarketModel {
 public:
  MockMarket(cyclus::Context* ctx)
      : cyclus::MarketModel(ctx),
        cyclus::Model(ctx) {};
  
  virtual ~MockMarket() {}
  virtual cyclus::Model* Clone() {return new MockMarket(context());};
  
  MockMarket(cyclus::Context* ctx, std::string commod)
      : cyclus::MarketModel(ctx),
        cyclus::Model(ctx) {
    commodity_ = commod;
  }
  virtual void ReceiveMessage(cyclus::Message::Ptr msg) {}
  virtual void Resolve() {}
};

#endif // CYCLUS_TESTS_MOCK_MARKET_H_
