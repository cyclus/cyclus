#include "test_market.h"

#include "model.h"

extern "C" cyclus::Model* constructTestMarket(cyclus::Context* ctx) {
  return new TestMarket(ctx);
}

extern "C" cyclus::Model* destructTestMarket(cyclus::Model* model) {
  delete model;
}
