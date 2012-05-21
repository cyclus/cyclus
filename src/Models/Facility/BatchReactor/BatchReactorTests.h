// BatchReactorTests.cpp
#include <gtest/gtest.h>

#include "BatchReactor.h"
#include "CycException.h"
#include "Message.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeBatchReactor : public BatchReactor {
  public:
  FakeBatchReactor() : BatchReactor() {};

  FakeBatchReactor(int lencycle, int life, double loadcore, int nbatch) : BatchReactor() {
    
    // initialize facility parameters
    setCycleLength(lencycle); 
    setLifetime(life);
    setCoreLoading(loadcore);
    setNBatches(nbatch);
    setBatchLoading( coreLoading() / nBatches() );
    
    CompMapPtr test_comp = CompMapPtr(new CompMap(MASS));
    
    int u235 = 92235;
    double one = 1.0;
    (*test_comp)[u235]=one;
    string test_mat_unit = "test_mat_unit";
    string test_rec_name = "test_rec_name";
    double test_size = 10.0;
    bool test_template = true;
    IsoVector recipe(test_comp);
    
    // all facilities require commodities - possibly many
    string recipe_name;
    std::string in_commod;
    std::string out_commod;
    IsoVector in_recipe;
    IsoVector out_recipe;

    // for each fuel pair, there is an in and an out commodity
    int pairs = 1;
    for (int i = 0; i < pairs; i++){
      // get commods
      out_commod = "out-commod";
      in_commod = "in-commod";

      // get recipes
      setInRecipe(recipe);
      setOutRecipe(recipe);

      addFuelPair(in_commod, in_recipe, out_commod, out_recipe);
    }
  }

  virtual ~FakeBatchReactor() { }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* BatchReactorModelConstructor(){
  return dynamic_cast<Model*>(new FakeBatchReactor());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel* BatchReactorConstructor(){
  return dynamic_cast<FacilityModel*>(new FakeBatchReactor());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class BatchReactorTest : public ::testing::Test {
protected:
  FakeBatchReactor* src_facility;
  FakeBatchReactor* new_facility; 
  TestMarket* out_market_;
  TestMarket* in_market_;

  int lencycle, life, nbatch;
  double loadcore;

  virtual void SetUp(){
    // set up model parameters
    lencycle = 3;
    life = 9;
    loadcore = 10.0;
    nbatch = 5;

    // set up models
    src_facility = new FakeBatchReactor(lencycle, life, loadcore, nbatch);
    src_facility->setParent(new TestInst());
    new_facility = new FakeBatchReactor();
    in_market_ = new TestMarket(src_facility->inCommod());
    out_market_ = new TestMarket(src_facility->outCommod());
  }

  virtual void TearDown() {
    delete src_facility;
    delete new_facility;
    delete in_market_;
    delete out_market_;
  }
};
