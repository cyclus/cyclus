#if !defined(_FACFACTORY)
#define _FACFACTORY
#include <iostream>
#include "Facility.h"

using namespace std;
//-----------------------------------------------------------------------------
/*
 * The FacFactory class instantiates members of the Facility class 
 * with a function called 'Create' which accepts a facility type.
*/
//-----------------------------------------------------------------------------
class FacFactory   
{
public:
	enum{ recipe, null, source, sink};
	typedef int FacilityId;
	virtual Facility* Create(FacilityId id);
};
#endif
