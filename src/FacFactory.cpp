#include <iostream>
#include "FacFactory.h"
#include "Facility.h"
#include "NullFac.h"
#include "RecipeFac.h"
#include "SourceFac.h"
#include "SinkFac.h"

using namespace std;
Facility* FacFactory::Create(FacilityId id)
{
		// change this syntax to a more extensible case switching?
		// a registration moment? 	
		cout << "creating a facility with id : " << id << endl;	
		if ( id == recipe ) return new RecipeFac;
		if ( id == null   ) return new NullFac;
		if ( id == source ) return new SourceFac;
		if ( id == sink   ) return new SinkFac;
		return 0;
};
