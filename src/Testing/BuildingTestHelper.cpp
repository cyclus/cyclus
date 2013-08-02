#include "BuildingTestHelper.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
BuildingTestHelper::BuildingTestHelper()
{
  using cyclus::ActionBuilding::Builder;
  builder1 = new Builder();
  builder2 = new Builder();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
BuildingTestHelper::~BuildingTestHelper() 
{
  delete builder1;
  delete builder2;
}
