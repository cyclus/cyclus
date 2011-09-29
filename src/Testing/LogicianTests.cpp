#include <gtest/gtest.h>
#include "Logician.h"
#include "Model.h"
#include "Material.h"
#include "Commodity.h"

#include <string>
#include <map>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TestModel : public Model {
  public:
    int pre_history_handled_runs_;
    int tick_handled_runs_;
    int tock_handled_runs_;
    int market_resolved_runs_;

    TestModel() {}

    TestModel(string aname, ModelType amodel_type) {
      // needs adjustment because these are private to Model
      //handle_ = aname;
      //name_ = aname;

      setModelType("type-generic");

      pre_history_handled_runs_ = 0;
      tick_handled_runs_ = 0;
      tock_handled_runs_ = 0;
      market_resolved_runs_ = 0;
    }

    void handlePreHistory(int time) {
      pre_history_handled_runs_ += 1;
    }

    void handleTick(int time) {
      tick_handled_runs_ += 1;
    }

    void handleTock(int time) {
      tock_handled_runs_ += 1;
    }

    void resolve() {
      market_resolved_runs_ += 1;
    }

    void copyFreshModel(Model* model) {}
};

class LogicianTest : public ::testing::Test {
  protected:
    Logician* logician_;
    std::vector<TestModel> model_list;

    virtual void SetUp(){
      logician_ = Logician::Instance();

      buildModels();
    };

  private:
    void buildModels() {
      addModel("market_1", MARKET);
      addModel("market_2", MARKET);

      addModel("facility_1", FACILITY);
      addModel("facility_2", FACILITY);

      addModel("region_1", REGION);
      addModel("region_2", REGION);
    }

    void addModel(string name, ModelType type) {
      TestModel test_model;
      test_model = TestModel(name, type);
      model_list.push_back(test_model);
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

TEST_F(LogicianTest, AddRetrieveModels) {
  // finish writing test
  EXPECT_TRUE(true);
}

TEST_F(LogicianTest, AddRetrieveCommodity) {
  // finish writing test
  EXPECT_TRUE(true);
}

TEST_F(LogicianTest, AddRetrieveMaterial) {
  // finish writing test
  EXPECT_TRUE(true);
}

TEST_F(LogicianTest, CommodityRegistration) {
  // finish writing test
  EXPECT_TRUE(true);
}

TEST_F(LogicianTest, ExecutionOrder) {
  // prehistory, timestep, #calls, etc.

  // finish writing test
  EXPECT_TRUE(true);
}

