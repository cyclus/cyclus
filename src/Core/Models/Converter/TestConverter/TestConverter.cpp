#include "TestConverter.h"

#include "Model.h"

extern "C" Model* constructTestConverter() {
  return new TestConverter();
}

extern "C" Model* destructTestConverter(Model* model) {
  delete model;
}
