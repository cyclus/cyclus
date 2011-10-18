#include <gtest/gtest.h>

#include "Facility/SourceFacility.h"
#include "Commodity.h"
#include "Message.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeSourceFacility : public SourceFacility {
  public:
    FakeSourceFacility() : SourceFacility() {
      out_commod_ = new Commodity();
      recipe_ = new Material();

      capacity_ = 25;
      inventory_size_ = 50;
      commod_price_ = 5000;

      inventory_ = deque<Material*>();
      ordersWaiting_ = deque<Message*>();
    }

    virtual ~FakeSourceFacility() {
      delete out_commod_;
      delete recipe_;
    }

    InstModel* my_inst_;

    InstModel* getFacInst() {
      return my_inst_;
    }

    double getInventory() {return checkInventory();}
};

class InfiniteDump : public FacilityModel {
  void receiveMessage(Message* msg) {

  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TransactionMaker : public InstModel {
  
  void receiveMessage(Message* msg) {
    msg->setDir(DOWN_MSG);

    int what_id = 0;

    msg->setRequesterID(what_id);

  }

  void copyFreshModel(Model* model) { }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SourceFacilityTest : public ::testing::Test {
  protected:
    FakeSourceFacility* src_facility;

    virtual void SetUp(){
      src_facility = new FakeSourceFacility();
      src_facility->my_inst_ = new TransactionMaker();
    };

    virtual void TearDown() {
      delete src_facility;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SourceFacilityTest, ReceiveMessage) {
  int time = 1;

  EXPECT_DOUBLE_EQ(0.0, src_facility->getInventory());
  EXPECT_NO_THROW(src_facility->handleTick(time));
}

