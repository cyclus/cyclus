#include "test_converter.h"

#include "model.h"

extern "C" cyclus::Model* constructTestConverter(cyclus::Context* ctx) {
  return new TestConverter(ctx);
}

extern "C" cyclus::Model* destructTestConverter(cyclus::Model* model) {
  delete model;
}
