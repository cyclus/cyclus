#include <gtest/gtest.h>

#include "SDManagerTests.h"

#include "CycException.h"
#include "SupplyDemandManager.h"
#include "SupplyDemand.h"
#include "SymbolicFunctions.h"

#include <vector>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SDManagerTests::SetUp() {
  manager = SupplyDemandManager();
  
  commod = new Commodity("commod");
  demand = FunctionPtr(new LinearFunction(5,5));

  capacity = 1;
  cost = 1;
  p1 = new Producer("p1",*commod,capacity,cost);
  p2 = new Producer("p2",*commod,capacity,cost);

  initCommodity();
}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SDManagerTests::TearDown() {
  delete commod, p1, p2;
}  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SDManagerTests::initCommodity() {
  vector<Producer> v;
  v.push_back(*p1);
  v.push_back(*p2);
  manager.registerCommodity(*commod,demand,v);
}  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SDManagerTests,TestProduceres) {
  EXPECT_EQ(manager.nProducers(*commod),2);
  EXPECT_EQ(*manager.producer(*commod,0),*p1);
  EXPECT_EQ(*manager.producer(*commod,1),*p2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SDManagerTests,TestDemand) {
  EXPECT_EQ(manager.demandFunction(*commod),demand);
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(manager.demand(*commod,i),demand->value(i));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SDManagerTests,TestSupply) {
  double base = 0;
  double val = 5;
  EXPECT_EQ(manager.supply(*commod),base);

  // increase
  double total = 0;
  for (int i = 0; i < 10; i++) {
    double incr = (i+1) * val;
    manager.increaseSupply(*commod,incr);
    total += incr;
    EXPECT_EQ(manager.supply(*commod),total);
  }

  // decrease
  for (int i = 0; i < 10; i++){
    double decr = (i+1) * val;
    manager.decreaseSupply(*commod,decr);
    total -= decr;
    EXPECT_EQ(manager.supply(*commod),total);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SDManagerTests,TestSupplyThrow) {
  EXPECT_THROW(manager.decreaseSupply(*commod,1),
               CycNegativeValueException);
}
