#include "TestConverter.h"

#include "Model.h"

extern "C" cyclus::Model* constructTestConverter() {
  return new TestConverter();
}

extern "C" cyclus::Model* destructTestConverter(cyclus::Model* model) {
  delete model;
}
