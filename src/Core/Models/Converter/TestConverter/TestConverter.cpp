#include "TestConverter.h"

#include "Model.h"

extern "C" Model* constructor() {
  return new TestConverter();
}

extern "C" Model* destructor(Model* model) {
  delete model;
}
