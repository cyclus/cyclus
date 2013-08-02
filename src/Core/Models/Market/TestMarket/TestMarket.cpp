#include "TestMarket.h"

#include "Model.h"

extern "C" cyclus::Model* constructTestMarket() {
  return new TestMarket();
}

extern "C" cyclus::Model* destructTestMarket(cyclus::Model* model) {
  delete model;
}
