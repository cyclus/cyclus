#if !defined _TESTFACILITY_H_
#define _TESTFACILITY_H_

#include "FacilityModel.h"
#include "Material.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Facility, for testing
class TestFacility: public FacilityModel {
 public:
  
  void receiveMessage(msg_ptr msg) {
    msg->setDir(DOWN_MSG);
  }

  void copyFreshModel(Model* src) { copy(dynamic_cast<TestFacility*>(src)); }

  void receiveMaterial(Transaction trans, std::vector<mat_rsrc_ptr> manifest) { }

};

#endif
