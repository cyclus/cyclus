#if !defined _TESTFACILITY_H_
#define _TESTFACILITY_H_

#include "FacilityModel.h"
#include "Material.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Facility, for testing
class TestFacility: public FacilityModel {
 public:
  TestFacility() : FacilityModel() {}
  
  virtual ~TestFacility() {}
  
  void receiveMessage(msg_ptr msg) {
    msg->setDir(DOWN_MSG);

    Model* what_model = NULL;

    msg->setRequester(what_model);

  }

  void copyFreshModel(Model* src) { copy(dynamic_cast<TestFacility*>(src)); }

  void receiveMaterial(Transaction trans, std::vector<mat_rsrc_ptr> manifest) { }

};

#endif
