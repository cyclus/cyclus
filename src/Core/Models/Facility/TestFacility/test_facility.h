#if !defined _TESTFACILITY_H_
#define _TESTFACILITY_H_

#include "facility_model.h"
#include "material.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Facility, for testing
class TestFacility: public cyclus::FacilityModel {
 public:
  TestFacility(cyclus::Context* ctx) : cyclus::FacilityModel(ctx) { };

  void ReceiveMessage(cyclus::Message::Ptr msg) {
    msg->SetDir(cyclus::DOWN_MSG);
  }

  void ReceiveMaterial(cyclus::Transaction trans,
                       std::vector<cyclus::Material::Ptr> manifest) { }

  cyclus::Prototype* clone() {
    return new TestFacility(context());
  }

  void CloneModuleMembersFrom(cyclus::FacilityModel* source) { }
  void HandleTick(int time) { };
  void HandleTock(int time) { };
};

#endif
