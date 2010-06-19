// SourceFac.h
#if !defined(_SOURCEFAC)
#define _SOURCEFAC
#include <iostream>
#include"Facility.h"
#include"FacFactory.h"

using namespace std;

/**
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
class SourceFac : public Facility 
{
public:
	/**
	 * Every facility sould be able to print its own name.
	 */
	virtual void printMyName();
};
#endif
