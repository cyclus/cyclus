// Commodity.h
#if !defined(_COMMODITY)
#define _COMMODITY
#include <string>
#include "Market.h"


using namespace std;

class Commodity
{
public:
	/**
	 * Constructs a new commodity object 
	 *
	 * @param ID is the commodity ID, like a serial number
	 * @param name is the commodity name, (ore, UF6, moxFuel, waste, etc... )
	 * @param mkt is a pointer to the market on which this commodity is traded.
	 * @param fissile indicates whether or not this is a potential fuel.
	 * @param sepMat indicates whether or not this is a separated stream
	 */
	Commodity(int ID, string name, Market* mkt, bool fissile, bool sepMat);	

	/**
	 * Virtual Commodity destructor.
	 */
	virtual ~Commodity();

private:
	/**
	 * This gives the next available ID number, for serialization of 
	 * the commodities.
	 */
	static int nextID;

	/**
	 * This is the name of the commodity, indicative of the market on 
	 * which it's to be traded.
	 */
	string myName;
	
	/**
	 * The ID number is to be used for serialization of the commodity types.
	 */
	int myID;

	/**
	 * Each commodity should be associated with a market on which it will be
	 * traded between offering and requesting facilities at each timestep.
	 */
	Market* marketPtr;
	
	/**
	 * True if this commodity is a fissile material (e.g. eUF6, pwrFuel, etc.). 
	 * A value must be given. All commodities are either fissile or non-fissile. 
	 */
	bool isFissile;

	/**
	 * True if this commodity is a separated stream (e.g. sepU, sepTc, etc.).
	 */
	bool isSepMat;
	
};
#endif
