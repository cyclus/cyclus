#include <gtest/gtest.h>

#include "iso_vector.h"

class IsoVectorTests : public ::testing::Test {
protected:
  cyclus::CompMapPtr comp;
  cyclus::IsoVector vec;
  cyclus::IsoVector zero_vec;

  std::vector<int> isotopes;
  double ratio;
  cyclus::CompMapPtr add_result, mix_result;
  cyclus::IsoVector add_to_vec, to_add_vec;
  double efficiency;
  cyclus::CompMapPtr subtract_result, separate_result;
  cyclus::IsoVector subtract_from_vec, to_subtract_vec;
  

  // this sets up the fixtures
  virtual void SetUp() {
    comp = cyclus::CompMapPtr(new cyclus::CompMap(cyclus::MASS));
    vec = cyclus::IsoVector(comp);
  };
  
  // this tears down the fixtures
  virtual void TearDown() {
  }

  void LoadMaps() {
    using cyclus::IsoVector;
    using cyclus::CompMap;
    using cyclus::CompMapPtr;
    using cyclus::MASS;

    isotopes.push_back(1001),isotopes.push_back(2004);
    
    // add, mix terms
    ratio = 2;
    CompMapPtr to_add = CompMapPtr(new CompMap(MASS));
    (*to_add)[isotopes.at(isotopes.size()-1)] = 1;
    to_add_vec = IsoVector(to_add);
    CompMapPtr add_to = CompMapPtr(new CompMap(MASS));
    (*add_to)[isotopes.at(0)] = 2;
    add_to_vec = IsoVector(add_to);
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
    to_subtract_vec = IsoVector(to_add);
    // subtract, separate results
    subtract_result = add_to;
    separate_result = CompMapPtr(new CompMap(MASS));
    (*separate_result)[isotopes.at(0)] = 1;
    (*separate_result)[isotopes.at(isotopes.size()-1)] = 1-efficiency;
    separate_result->normalize();
  }
};
