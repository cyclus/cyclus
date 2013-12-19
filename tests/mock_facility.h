#ifndef _CYCLUS_TESTS_MOCK_FACILITY_H_
#define _CYCLUS_TESTS_MOCK_FACILITY_H_

#include "context.h"
#include "facility_model.h"
#include "material.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MockFacility : public cyclus::FacilityModel {
 public:
  MockFacility(cyclus::Context* ctx)
      : cyclus::FacilityModel(ctx),
        cyclus::Model(ctx) {};

  virtual ~MockFacility() {};

  virtual cyclus::Model* Clone() {
    MockFacility* m = new MockFacility(*this);
    m->InitFrom(this);
    return m;
  };

  void CloneModuleMembersFrom(cyclus::FacilityModel* source) {};
  void Tick(int time) {};
  void Tock(int time) {};
};

#endif // CYCLUS_TESTS_MOCK_FACILITY_H_
