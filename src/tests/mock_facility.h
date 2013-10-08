#ifndef _CYCLUS_TESTS_MOCK_FACILITY_H_
#define _CYCLUS_TESTS_MOCK_FACILITY_H_

#include "context.h"
#include "facility_model.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MockFac : public cyclus::FacilityModel {
 public:
  MockFac(cyclus::Context* ctx) : cyclus::FacilityModel(ctx) {};

  virtual ~MockFac() {};

  virtual cyclus::Model* Clone() {
    MockFac* m = new MockFac(*this);
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
