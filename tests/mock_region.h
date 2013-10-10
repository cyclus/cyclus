#ifndef _CYCLUS_TESTS_MOCK_REGION_H_
#define _CYCLUS_TESTS_MOCK_REGION_H_

#include "context.h"
#include "region_model.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MockRegion : public cyclus::RegionModel {
 public:
  MockRegion(cyclus::Context* ctx) : cyclus::RegionModel(ctx) { };

  virtual ~MockRegion() { };

  virtual cyclus::Model* Clone() {
    MockRegion* m = new MockRegion(*this);
    m->InitFrom(this);
    return m;
  }
};

#endif // CYCLUS_TESTS_MOCK_REGION_H_
