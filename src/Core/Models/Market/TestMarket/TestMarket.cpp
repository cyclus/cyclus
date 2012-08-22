#if !defined _TESTMARKET_H_
#define _TESTMARKET_H_

#include "MarketModel.h"
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Market, for testing.
TestMarket::TestMarket(std::string commod) {
  commodity_ = commod;
  MarketModel::initSimInteraction(this);
}
void TestMarket::receiveMessage(msg_ptr msg) {
}
void TestMarket::resolve() {
}
void TestMarket::copy(TestMarket* src){
  commodity_ = src->commodity_;
}
void TestMarket::copyFreshModel(Model* src){
  copy(dynamic_cast<TestMarket*>(src));
}

extern "C" Model* constructTestMarket() {
  return new TestMarket();
}
