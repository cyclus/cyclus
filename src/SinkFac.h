#if !defined(_SINKFAC)
#define _SINKFAC
#include <iostream>
#include "Facility.h"
#include "FacFactory.h"

using namespace std;

//-----------------------------------------------------------------------------
/*
 * The SinkFac class inherits from the Facility class and is instantiated
 * by the FacFactory class. 
 *
 * It is the concrete class of facilities at the end of a fuel cycle model
 * which absorb a finite (or infinite) amount of material that is offered on 
 * the final commodity market.
 *
 * A repository might be a member of the SinkFac class.
 *
*/
//-----------------------------------------------------------------------------
class SinkFac : public Facility 
{
public:
	virtual void printMyName();
};
#endif
