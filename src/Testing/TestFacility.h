#if !defined _TESTFACILITY_H_
#define _TESTFACILITY_H_

#include "FacilityModel.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Facility, for testing
class TestFacility: public FacilityModel {
  
  void receiveMessage(Message* msg) {
    msg->setDir(DOWN_MSG);

    Model* what_model = NULL;

    msg->setRequester(what_model);

  }

  void copyFreshModel(Model* model) { }

  void receiveMaterial(Transaction trans, vector<Material*> manifest) { }
};

#endif
