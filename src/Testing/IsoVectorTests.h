#include <gtest/gtest.h>

#include "IsoVector.h"

class IsoVectorTests : public ::testing::Test {
protected:
  CompMapPtr comp;
  IsoVector vec;
  IsoVector zero_vec;

  std::vector<int> isotopes;
  double ratio;
  CompMapPtr add_result, mix_result;
  IsoVector orig_vec, vec_to_add;
  double efficiency;
  CompMapPtr subtract_result, separate_result;
  IsoVector subtract_from_vec, to_subtract_vec;
  

  // this sets up the fixtures
  virtual void SetUp() {
    comp = CompMapPtr(new CompMap(MASS));
    vec = IsoVector(comp);
  };
  
  // this tears down the fixtures
  virtual void TearDown() {
  }

  void LoadMaps() {
    isotopes.push_back(1001),isotopes.push_back(2004);
    
    // add, mix terms
    ratio = 2;
    CompMapPtr comp_to_add = CompMapPtr(new CompMap(MASS));
    (*comp_to_add)[isotopes.at(isotopes.size()-1)] = 1;
    vec_to_add = IsoVector(comp_to_add);
    CompMapPtr orig_comp = CompMapPtr(new CompMap(MASS));
    (*orig_comp)[isotopes.at(0)] = 1;
    orig_vec = IsoVector(orig_comp);
    // add, mix results
    add_result = CompMapPtr(new CompMap(MASS));
    (*add_result)[isotopes.at(0)] = 1;
    (*add_result)[isotopes.at(isotopes.size()-1)] = 1;
    add_result->normalize();
    mix_result = CompMapPtr(new CompMap(MASS));
    (*mix_result)[isotopes.at(0)] = 1;
    (*mix_result)[isotopes.at(isotopes.size()-1)] = 1*ratio;
    mix_result->normalize();    

    // subtract, separate terms
    efficiency = 0.9;
    subtract_from_vec = IsoVector(add_result);
    to_subtract_vec = IsoVector(comp_to_add);
    // subtract, separate results
    subtract_result = orig_comp;
    separate_result = CompMapPtr(new CompMap(MASS));
    (*separate_result)[isotopes.at(0)] = 1;
    (*separate_result)[isotopes.at(isotopes.size()-1)] = 1-efficiency;
    separate_result->normalize();
  }
};
