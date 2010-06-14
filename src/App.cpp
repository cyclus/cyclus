#include <iostream>
#include "Facility.h"
#include "FacFactory.h"
#include "Market.h"
#include "MktFactory.h"

using namespace std;


//-----------------------------------------------------------------------------
// Main entry point for the test application...
//-----------------------------------------------------------------------------
int main( void )
{

	// Ask the FacFactory map for pointers to subtype constructors
	FacFactory *recipeCreator = get_fac_map()[ "recipe" ];
	FacFactory *nullCreator   = get_fac_map()[ "null"   ];
	FacFactory *nullCreator2  = get_fac_map()[ "null2"  ];
	FacFactory *sourceCreator = get_fac_map()[ "source" ];
	FacFactory *sinkCreator   = get_fac_map()[ "sink"   ];
	MktFactory *netFlowCreator= get_mkt_map()[ "netFlow"   ];

	// Get a pointer to a new instance 
	// of the named Facility subtype
	Facility *rFac  = recipeCreator->Create(); 
	Facility *nFac  = nullCreator->Create(); 
	Facility *nFac2 = nullCreator2->Create(); 
	Facility *soFac = sourceCreator->Create();
	Facility *siFac = sinkCreator->Create();   
	Market  *mktFac = netFlowCreator->Create();   

	// and call its name printing function
	// to test whether everything was instantiated properly 
	rFac->printMyName();
	nFac->printMyName();
	nFac2->printMyName();
	soFac->printMyName();
	siFac->printMyName();
	mktFac->printMyName();

		return 0;
}
