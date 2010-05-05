#include <iostream>
#include "FacFactory.h"
#include "Facility.h"

using namespace std;


// a dictionary of facility constructors
map<string, FacFactory *> & 

// a Facility to grab the Facility from the map
get_fac_map()
{
     static map<string,FacFactory *> theFacilityMap;
	 return theFacilityMap;
}

