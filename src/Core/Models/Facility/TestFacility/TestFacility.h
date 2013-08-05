#if !defined _TESTFACILITY_H_
#define _TESTFACILITY_H_

#include "FacilityModel.h"
#include "Material.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Facility, for testing
class TestFacility: public cyclus::FacilityModel {
 public:
  
  void ReceiveMessage(cyclus::msg_ptr msg) {
    msg->SetDir(cyclus::DOWN_MSG);
  }

  void ReceiveMaterial(cyclus::Transaction trans, 
                       std::vector<cyclus::mat_rsrc_ptr> manifest) { }
  
  cyclus::Prototype* clone() { return new TestFacility(); }

  void CloneModuleMembersFrom(cyclus::FacilityModel* source) { }
  void HandleTick(int time) { };
  void HandleTock(int time) { };
};

#endif
