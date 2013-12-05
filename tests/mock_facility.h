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

  void ReceiveMessage(cyclus::Message::Ptr msg) {};
  void ReceiveMaterial(cyclus::Transaction trans,
                       std::vector<cyclus::Material::Ptr> manifest) {};
  void CloneModuleMembersFrom(cyclus::FacilityModel* source) {};
  void HandleTick(int time) {};
  void HandleTock(int time) {};
};

#endif // CYCLUS_TESTS_MOCK_FACILITY_H_
