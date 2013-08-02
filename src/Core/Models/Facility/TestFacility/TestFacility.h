#if !defined _TESTFACILITY_H_
#define _TESTFACILITY_H_

#include "FacilityModel.h"
#include "Material.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Facility, for testing
class TestFacility: public cyclus::FacilityModel {
 public:
  
  void receiveMessage(cyclus::msg_ptr msg) {
    msg->setDir(cyclus::DOWN_MSG);
  }

  void receiveMaterial(cyclus::Transaction trans, 
                       std::vector<cyclus::mat_rsrc_ptr> manifest) { }
  
  cyclus::Prototype* clone() { return new TestFacility(); }

  void cloneModuleMembersFrom(cyclus::FacilityModel* source) { }
  void handleTick(int time) { };
  void handleTock(int time) { };
};

#endif
