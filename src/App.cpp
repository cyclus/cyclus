#include <iostream>
#include "Facility.h"
#include "FacFactory.h"

using namespace std;

//-----------------------------------------------------------------------------
// Main entry point for the test application...
//-----------------------------------------------------------------------------
int main( void )
{
	// Ask the FacFactory map for pointers to subtype constructors
	FacFactory *recipeCreator = get_fac_map()[ "recipe" ];
	FacFactory *nullCreator   = get_fac_map()[ "null"   ];
	FacFactory *sourceCreator = get_fac_map()[ "source" ];
	FacFactory *sinkCreator   = get_fac_map()[ "sink"   ];

	// Get a pointer to a new instance 
	// of the named Facility subtype
	Facility *rFac  = recipeCreator->Create(); 
	Facility *nFac  = nullCreator->Create(); 
	Facility *soFac = sourceCreator->Create();
	Facility *siFac = sinkCreator->Create();   

	// and call its name printing function
	rFac->printMyName();
	nFac->printMyName();
	soFac->printMyName();
	siFac->printMyName();

		return 0;
}
