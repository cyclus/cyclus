#include "BuildingTestHelper.h"

using namespace ActionBuilding;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
BuildingTestHelper::BuildingTestHelper()
{
  builder1 = new Builder();
  builder2 = new Builder();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
BuildingTestHelper::~BuildingTestHelper() 
{
  delete builder1;
  delete builder2;
}
