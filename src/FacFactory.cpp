// FacFactory.cpp
// Implements the FacFactory class
#include <iostream>
#include "FacFactory.h"
#include "Facility.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
map<string, FacFactory *> & 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
get_fac_map()
{
     static map<string,FacFactory *> theFacilityMap;
	 return theFacilityMap;
}

