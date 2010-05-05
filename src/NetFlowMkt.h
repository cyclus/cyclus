#if !defined(_NETFLOWMKT)
#define _NETFLOWMKT
#include <iostream>
#include"Market.h"
#include"MktFactory.h"


using namespace std;

//-----------------------------------------------------------------------------
/*
 * The NetFlowMkt class inherits from the Market class and is instantiated by
 * the MktFactory class. 
 * 
 * This market will do nothing. It just holds whatever commodity it needs
 * to, and offers it at some point back onto the market. 
 * I wonder, in fact, if this market will be useful to our model in any
 * way whatsoever. With some decay it could be useful in representing cooling
 * pads, but why else would we need this object?
 *
 * This is all our factory really knows about the products it's creating
*/
//-----------------------------------------------------------------------------
class NetFlowMkt : public Market  
{
public:

	/**
	 * prints the name of this mktType
	 *
	 */
	virtual void printMyName();

	/**
	 * Constructs a NetFlowMkt with no specified data.
	 * 
	 */
	NetFlowMkt();

	/**
	 * Constructs a NetFlowMkt with specified data.
	 *
	 * @param a is a double
	 * @param b is a double
	 * 
	 */
	NetFlowMkt(double a, double b);

protected: 
	double bob;
};
#endif
