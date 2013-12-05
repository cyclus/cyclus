#if !defined _TESTFACILITY_H_
#define _TESTFACILITY_H_

#include "facility_model.h"
#include "material.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Facility, for testing
class TestFacility: public cyclus::FacilityModel {
 public:
  TestFacility(cyclus::Context* ctx)
      : cyclus::FacilityModel(ctx),
        cyclus::Model(ctx) {};
      
  virtual cyclus::Model* Clone() { return new TestFacility(context()); };

  void CloneModuleMembersFrom(cyclus::FacilityModel* source) {};
  void Tick(int time) {};
  void Tock(int time) {};
};

#endif
