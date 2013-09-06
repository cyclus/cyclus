#include "test_market.h"

#include "model.h"

extern "C" cyclus::Model* ConstructTestMarket(cyclus::Context* ctx) {
  return new TestMarket(ctx);
}

extern "C" cyclus::Model* DestructTestMarket(cyclus::Model* model) {
  delete model;
}
