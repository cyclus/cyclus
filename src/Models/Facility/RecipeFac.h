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
	 * @param i is the Institution to which this facility belongs
	 * @param SN is the facility's ID number, an integer. 
	 * @param inRecs are the one or more recipes the facility receives. 
	 * @param outRecs are the one or more recipes the facility produces.
	 * @param inCommods are the one or more commodities this facility receives.
	 * @param inCommods are the one or more commodities this facility produces.
	 */
	RecipeFac(string name, Inst* i, int SN, map<Iso, NumDens> inRecs,
			map<Iso, NumDens> outRecs, list<Commodity*> inCommods, list<Commodity*> outCommods);

	/**
	 * This process occurs at each timestep.
	 */
	virtual void handleTick(int time);

protected:
	/**
	 * The one or more recipes this facility receives.
	 */
	map<Iso, NumDens > inRecs;

	/**
	 * The one or more recipes this facility produces.
	 */
	map<Iso, NumDens> outRecs;

};

extern "C" Facility* construct() {
    return new NullFac;
}

extern "C" void destruct(Facility* p) {
    delete p;
}


#endif
