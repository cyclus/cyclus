#include "TestConverter.h"

extern "C" Model* constructTestConverter() {
  return new TestConverter();
}
