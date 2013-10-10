#include "building_test_helper.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
BuildingTestHelper::BuildingTestHelper()
{
  using cyclus::action_building::Builder;
  builder1 = new Builder();
  builder2 = new Builder();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
BuildingTestHelper::~BuildingTestHelper() 
{
  delete builder1;
  delete builder2;
}
