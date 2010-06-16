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
	 * @param name is the commodity name, (ore, UF6, moxFuel, waste, etc... )
	 * @param ID is the commodity ID, like a serial number
	 * @param mkt is a pointer to the market on which this commodity is traded.
	 * @param fissile indicates whether or not this is a potential fuel.
	 * @param sepMat indicates whether or not this is a separated stream
	 */
	Commodity(string name, Market* mkt, bool fissile, bool sepMat);	

	/**
	 * Returns this Commodity's name.
	 *
	 * @return the name
	 */
	virtual const string getName() const;

	/**
	 * Returns this Commodity's ID number.
	 *
	 * @return the ID number
	 */
	virtual int getSN() const;

	/**
	 * advances the commodity serialization
	 */
	static int nextCommodID();

	/**
	 * Returns whether this Commodity is fissile.
	 *
	 * @return isFissle boolean
	 */
	virtual bool isFissile() const;
	
	/**
	 * Returns whether this Commodity is separated material.
	 *
	 * @return isSepMat boolean
	 */
	virtual bool isSepMat() const;
	
	/**
	 * Returns a pointer to this Commodity's Market.
	 *
	 * @return the pointer to the Market
	 */
	Market* getMarket();

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
	Market* myMarket;

	/**
	 * True if this commodity is a fissile material (e.g. eUF6, pwrFuel, etc.). 
	 * A value must be given. All commodities are either fissile or non-fissile. 
	 */
	bool fissile;

	/**
	 * True if this commodity is a separated stream (e.g. sepU, sepTc, etc.).
	 */
	bool sepMat;
	
};
#endif
