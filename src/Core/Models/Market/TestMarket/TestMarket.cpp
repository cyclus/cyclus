#include "TestMarket.h"

#include "Model.h"

extern "C" Model* constructTestMarket() {
  return new TestMarket();
}

extern "C" Model* destructTestMarket(Model* model) {
  delete model;
}
