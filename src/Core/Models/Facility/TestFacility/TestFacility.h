#if !defined _TESTFACILITY_H_
#define _TESTFACILITY_H_

#include "FacilityModel.h"
#include "Material.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Facility, for testing
class TestFacility: public FacilityModel {
 public:
  
  void receiveMessage(msg_ptr msg) ;

  void copyFreshModel(Model* src) ;

  void receiveMaterial(Transaction trans, std::vector<mat_rsrc_ptr> manifest) ;

};

#endif
