#include <iostream>
#include "Facility.h"
#include "FacFactory.h"

using namespace std;

//-----------------------------------------------------------------------------
// Main entry point for the test application...
//-----------------------------------------------------------------------------
int main( void )
{
	enum {recipe, null, source, sink};
	FacFactory fFactory;
	
	Facility *rFac = fFactory.Create( recipe );
	Facility *nFac = fFactory.Create( null );
	
	rFac->printMyName();
	nFac->printMyName();
/*
    MyCreator mCreator;

    Facility *rMFac = mCreator.Create( recipe );
    Facility *nMFac = mCreator.Create( null );
    Facility *bMFac = mCreator.Create( bu );


    rMFac->printMyName();
    nMFac->printMyName();
    bMFac->printMyName();
*/
		return 0;
}
