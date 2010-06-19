// RecipeFac.h
#if !defined(_RECIPEFAC)
#define _RECIPEFAC
#include <iostream>
#include"Facility.h"
#include"FacFactory.h"

	using namespace std;

/**
 * The RecipeFac inherits from the Facility class and is instantiated by
 * the FacFactory class. 
 *
 * It will be the type of facility that takes an input recipe of 
 * some commodity type A and outputs an output recipe of some commodity 
 * type B. The RecipeFac may hold onto the material for a certain time 
 * and depending on the genre of facility that this facility represents 
 * (conversion, enrichment, reactor) it will produce
 * power or swus or whatever as well as its output commodity.
 *
 * That output commodity will then be offered to the market associated with
 * that commodity.
*/
class RecipeFac : public Facility 
{
public:
	/**
	 * Every facility should be able to write its name.
	 */
	virtual void printMyName();
};
#endif
