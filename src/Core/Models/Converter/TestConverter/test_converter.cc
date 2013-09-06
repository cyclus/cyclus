#include "test_converter.h"

#include "model.h"

extern "C" cyclus::Model* ConstructTestConverter(cyclus::Context* ctx) {
  return new TestConverter(ctx);
}

extern "C" cyclus::Model* DestructTestConverter(cyclus::Model* model) {
  delete model;
}
