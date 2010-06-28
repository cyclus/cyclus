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

	/**
	 * Generic constructor for the RecipeFac class 
	 */
	RecipeFac();

	/** 
	 * Constructs a recipe-model facility with the following parameters:
	 *
	 * @param name is the facility's name, a string. 
	 * @param inRecs are the one or more recipes the facility receives. 
	 * @param outRecs are the one or more recipes the facility produces.
	 */
	RecipeFac(string name, deque<Material*> inRecs, deque<Material*> outRecs);

	/**
	 * This process occurs at each timestep.
	 */
	virtual void handleTick(int time);

protected:
	/**
	 * The one or more recipes this facility receives.
	 */
	list< Material* > inRecs;

	/**
	 * The one or more recipes this facility produces.
	 */
	list< Material* > outRecs;

};

#endif
