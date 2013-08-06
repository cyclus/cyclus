#include "test_market.h"

#include "model.h"

extern "C" cyclus::Model* constructTestMarket() {
  return new TestMarket();
}

extern "C" cyclus::Model* destructTestMarket(cyclus::Model* model) {
  delete model;
}
