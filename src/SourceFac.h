#if !defined(_SOURCEFAC)
#define _SOURCEFAC
#include <iostream>

using namespace std;

//-----------------------------------------------------------------------------
/*
 * The SourceFac class inherits from the Facility class and is instantiated
 * by the FacFactory class. 
 *
 * It is the concrete class of facilities at the beginning of a fuel cycle model
 * which create a finite (or infinite) source of material to be offered on 
 * an initial commodity market. 
 *
 * A mine might be a member of the SourceFac class.
 *
*/
//-----------------------------------------------------------------------------
class SourceFac : public Facility 
{
public:
	virtual void printMyName();
};
#endif
