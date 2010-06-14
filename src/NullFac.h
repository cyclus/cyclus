#if !defined(_NULLFAC)
#define _NULLFAC
#include <iostream>
#include"Facility.h"
#include"FacFactory.h"


using namespace std;

//-----------------------------------------------------------------------------
/*
 * The NullFac class inherits from the Facility class and is instantiated by
 * the FacFactory class. 
 * 
 * This facility will do nothing. It just holds whatever commodity it needs
 * to, and offers it at some point back onto the market. 
 * I wonder, in fact, if this facility will be useful to our model in any
 * way whatsoever. With some decay it could be useful in representing cooling
 * pads, but why else would we need this object?
 *
*/
//-----------------------------------------------------------------------------
class NullFac : public Facility  
{
public:

	/**
	 * prints the name of this facType
	 *
	 */
	virtual void printMyName();

	/**
	 * Constructs a NullFac with no specified data.
	 * 
	 */
	NullFac();

	/**
	 * Constructs a NullFac with specified data.
	 *
	 * @param a is a double
	 * @param b is a double
	 * 
	 */
	NullFac(double a, double b);

protected: 
	double bob;
};
#endif
