#include "test_converter.h"

#include "model.h"

extern "C" cyclus::Model* constructTestConverter() {
  return new TestConverter();
}

extern "C" cyclus::Model* destructTestConverter(cyclus::Model* model) {
  delete model;
}
