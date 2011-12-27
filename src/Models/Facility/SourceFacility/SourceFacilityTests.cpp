#include <gtest/gtest.h>

#include "SourceFacility.h"
#include "Message.h"
#include "MarketModel.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeSourceFacility : public SourceFacility {
  public:
    FakeSourceFacility() : SourceFacility() {
      out_commod_ = "my-commod";

      CompMap test_comp;

      Iso u235 = 92235;
      Atoms one = 1.0;
      test_comp[u235]=one;
      string test_mat_unit = "test_mat_unit";
      string test_rec_name = "test_rec_name";
      double test_size = 10.0;
      Basis test_type = ATOMBASED;
      bool test_template = true;

      recipe_ = new Material(test_comp, test_mat_unit, test_rec_name, test_size, 
          test_type, test_template); 

      capacity_ = 25;
      inventory_size_ = 50;
      commod_price_ = 5000;

      inventory_ = deque<Material*>();
      ordersWaiting_ = deque<Message*>();
    }

    virtual ~FakeSourceFacility() {
      delete recipe_;
    }

    double fakeCheckInventory() { return checkInventory(); }

    std::string getOutCommod() {return out_commod_;}
    double getCapacity() {return capacity_;}
    double getInvSize() {return inventory_size_;}
    double getCommodPrice() {return commod_price_;}
    Material* getRecipe() {return recipe_;}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class InfiniteDump : public FacilityModel {
  void receiveMessage(Message* msg) {

  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CommodMarket : public MarketModel {
  public :
    CommodMarket(std::string commod) : MarketModel() {
      commodity_ = commod;
    }
    void receiveMessage(Message* msg) {
    }
    void resolve() {
    }
    void copy(CommodMarket* src){
      commodity_ = src->commodity_;
    }
    void copyFreshModel(Model* src){
      copy(dynamic_cast<CommodMarket*>(src));
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TransactionMaker : public InstModel {
  
  void receiveMessage(Message* msg) {
    msg->setDir(DOWN_MSG);

    Model* what_model = NULL;

    msg->setRequester(what_model);

  }

  void copyFreshModel(Model* model) { }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SourceFacilityTest : public ::testing::Test {
  protected:
    FakeSourceFacility* src_facility;
    FakeSourceFacility* new_facility; 
    CommodMarket* commod_market;

    virtual void SetUp(){
      src_facility = new FakeSourceFacility();
      src_facility->setParent(new TransactionMaker());
      new_facility = new FakeSourceFacility();
      commod_market = new CommodMarket(src_facility->getOutCommod());
    };

    virtual void TearDown() {
      delete src_facility;
      delete commod_market;
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SourceFacilityTest, InitialState) {
  int time = 1;
  EXPECT_DOUBLE_EQ(0.0, src_facility->fakeCheckInventory());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SourceFacilityTest, CopyFacility) {
  new_facility->copy(src_facility); 
  EXPECT_DOUBLE_EQ(0.0, new_facility->fakeCheckInventory()); // fresh inventory
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SourceFacilityTest, CopyFreshModel) {
  new_facility->copyFreshModel(dynamic_cast<Model*>(src_facility)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<SourceFacility*>(new_facility)); // still a source facility
  EXPECT_NO_THROW(dynamic_cast<FakeSourceFacility*>(new_facility)); // still a fake source facility
  EXPECT_DOUBLE_EQ(0.0, new_facility->fakeCheckInventory()); // fresh inventory
  EXPECT_EQ(src_facility->getCapacity(), new_facility->getCapacity());
  EXPECT_EQ(src_facility->getInvSize(), new_facility->getInvSize());
  EXPECT_EQ(src_facility->getCommodPrice(), new_facility->getCommodPrice());
  EXPECT_EQ(src_facility->getRecipe(), new_facility->getRecipe());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SourceFacilityTest, Print) {
  EXPECT_NO_THROW(src_facility->print());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SourceFacilityTest, ReceiveMessage) {
  Message* msg;
  int time = 1;
  EXPECT_DOUBLE_EQ(0.0, src_facility->fakeCheckInventory());
  EXPECT_NO_THROW(src_facility->handleTick(time));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SourceFacilityTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_facility->handleTick(time));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SourceFacilityTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_facility->handleTock(time));
}


