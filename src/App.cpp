#include <iostream>
#include <cstdlib>
#include <cstring>
#include "Facility.h"
#include "FacFactory.h"
#include "Market.h"
#include "MktFactory.h"
#include "Timer.h"
#include "Logician.h"
#include "InputDB.h"

using namespace std;


//-----------------------------------------------------------------------------
// Main entry point for the test application...
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	// Start the clock.
	int start = clock();

	// Handle the command-line arguments.

	// Initialize their default values
	int dur = 1200;
	int sMonth = 1;
	int sYear = 2010;
	char* flag;
	char* val;
	
	// Must have an odd number of args.
	if (argc % 2 != 1) {
		cerr << "Wrong number of command line arguments." << endl
				 << "Usage: cyclus [-d duration] [-y start_year]"
				 << " [-m start_month] " << endl;
		exit(1);
	}

	// Count the options.
	int numOpts = (argc - 1) / 2;

	// Read the options.
	
	for (int opt = 1; opt <= numOpts; opt ++) {
		flag = argv[2 * opt - 1];
		val = argv[2 * opt ];
		
		if (0 == strcmp(flag, "-d"))
			dur = atoi(val);
		else if (0 == strcmp(flag, "-y"))
			sYear = atoi(val);
		else if (0 == strcmp(flag, "-m"))
			sMonth = atoi(val);
		}

	// Check the input.

		if (dur < 1 || dur > 2400) { // upper limit: 200 years
			cerr << "Invalid duration: " << dur << " (months)" << endl;
			exit(1);
		}
		
		if (sMonth < 1 || sMonth > 12) {
			cerr << "Invalid start month: " << sMonth 
				<< " (must be between 1 and 12, inclusive)" << endl;
			exit(1);
		}
		
		if (sYear < 1956 || sYear > 2200) { 
			cerr << "Invalid start year: " << sYear 
				<< " (must be between 1956 and 2200, inclusive" 
				<< " so that we don't drive ahead of our headlights.)" << endl;
			exit(1);
		}
		
		TI->initialize(dur, sMonth, sYear);
		
		////////////////////////////////////////////////////////////////////////////

		// "Come, and trip it as ye go,
		// On the light fantastick toe."
		// -- Milton, L'Allegro

		TI->runSim();

		////////////////////////////////////////////////////////////////////////////

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
		Market  *netMkt = netFlowCreator->Create();   

		// and call its name printing function
		// to test whether everything was instantiated properly 
		rFac->printMyName();
		rFac->handleTick(TI->getTime());
		nFac->printMyName();	
		nFac2->printMyName();
		soFac->printMyName();
		siFac->printMyName();
		netMkt->printMyName();

		// Create a Commodity
		Commodity* tester = new Commodity("tester", netMkt, 0, 0);

		cout << "The Tester commodity has the nickname " << tester->getName() << endl;


		
		delete TI;
		delete LI;
		delete INDB;
		return 0;
}
