#include "TestMarket.h"

#include "Model.h"

extern "C" Model* constructor() {
  return new TestMarket();
}

extern "C" Model* destructor(Model* model) {
  delete model;
}
