#include "MarketModel.h"
#include "TestMarket.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TestMarket::TestMarket() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TestMarket::TestMarket(std::string commod) {
  commodity_ = commod;
  MarketModel::initSimInteraction(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TestMarket::receiveMessage(msg_ptr msg) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TestMarket::resolve() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TestMarket::copy(TestMarket* src){
  commodity_ = src->commodity_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TestMarket::copyFreshModel(Model* src){
  copy(dynamic_cast<TestMarket*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" Model* constructTestMarket() {
  return new TestMarket();
}
