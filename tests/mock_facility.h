#ifndef CYCLUS_TESTS_MOCK_FACILITY_H_
#define CYCLUS_TESTS_MOCK_FACILITY_H_

#include "context.h"
#include "facility_model.h"
#include "material.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MockFacility : public cyclus::FacilityModel {
 public:
  MockFacility(cyclus::Context* ctx) : cyclus::FacilityModel(ctx) {};

  virtual ~MockFacility() {};

  virtual cyclus::Model* Clone() {
    MockFacility* m = new MockFacility(context());
    m->InitFrom(this);
    return m;
  };

  void InitFrom(MockFacility* m) {
    cyclus::FacilityModel::InitFrom(m);
  };

  void CloneModuleMembersFrom(cyclus::FacilityModel* source) {};
  void Tick(int time) {};
  void Tock(int time) {};
};

#endif // CYCLUS_TESTS_MOCK_FACILITY_H_
