#ifndef CYCLUS_TESTS_MOCK_REGION_H_
#define CYCLUS_TESTS_MOCK_REGION_H_

#include "context.h"
#include "region_model.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MockRegion : public cyclus::RegionModel {
 public:
  MockRegion(cyclus::Context* ctx) : cyclus::RegionModel(ctx) {};

  virtual ~MockRegion() {};

  virtual cyclus::Model* Clone() {
    MockRegion* m = new MockRegion(context());
    m->InitFrom(this);
    return m;
  }
};

#endif // CYCLUS_TESTS_MOCK_REGION_H_
