#include <gtest/gtest.h>

#include "IsoVector.h"

class IsoVectorTests : public ::testing::Test {
protected:
  CompMapPtr comp, add, subtract;
  IsoVector vec;

  // this sets up the fixtures
  virtual void SetUp() {
    comp = CompMapPtr(new CompMap(MASS));
    vec = IsoVector(comp);
  };
  
  // this tears down the fixtures
  virtual void TearDown() {
  }
};
