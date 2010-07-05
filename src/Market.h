// Market.h
#if !defined(_MARKET)
#define _MARKET
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

class Commodity;

//-----------------------------------------------------------------------------
/* The Market Class.
 * The Market class is the abstract class used by all objects that our 
 * factory will be responsible for creating, or manufacturing. 
 *
 * This is all our factory really knows about the products it's creating
 */
//-----------------------------------------------------------------------------
class Market
{
public:
	/**
	 * Constructor for the Market Class
	 */
	Market();

	/**
	 * Constructs a market that trades some set of commodities.
	 *
	 * @param name is the market's name, a string
	 * @param ID is the market's serial number, for serialization
	 */
	Market(string name, int ID);

	/**
	 * Returns the market's name.
	 */
	string getName();

	/**
	 * Returns the market's serial number.
	 */
	int getSN();

	/**
	 * Registers a market to the market map.
	 * The map has keys which are pointers to Commodity objects
	 * and values which are pointers to Markets!
	 */
	void registerMkt();

	/**
	 * Adds a commodity with the specfied parameters to this Market
	 *
	 * @param name is the commodity name
	 * @param fissile is true if the commodity is a fissile one
	 * @param sepMat is true if the commodity is separated spent fuel
	 */
	void addCommod(string name, bool fissile, bool sepMat);

	/**
	 * Adds the given pre-constructed Commodity to this Market.
	 *
	 * @param commod is the Commodity to add
	 */
	void addCommod(Commodity* commod);
		
	/**
	 * Returns a pointer to the Commodity with the given ID number, if it 
	 * is traded on this Market. Throws a GenException otherwise.
	 *
	 * @param SN the ID number of the Commodity being sought
	 * @return a pointer to it, if it's traded on this Market
	 */
	Commodity* getCommod(int SN);

	/**
	 * Returns a pointer to the Commodity with the given name, if it 
	 * is traded on this Market. Throws a GenException otherwise.
	 *
	 * @param name the name of the Commodity being sought
	 * @return a pointer to it, if it's traded on this Market
	 */
	Commodity* getCommod(string name);


	/**
	 * Every market should be able to write its name.
	 */
	virtual void printMyName();

	/**
	 * Returns the begin() and end() iterators over the
	 * Commodities in this Market.
	 *
	 * @return the pair of iterators
	 */
	pair<vector<Commodity*>::iterator , vector<Commodity*>::iterator> getCommods();

	/**
	 * Advances the Market nextID.
	 */
	static int getNextID();

	/**
	 * Markets should not be indestructible.
	 */
	virtual ~Market();

protected:
	/**
	 * Markets need names for reference
	 */
	string name;

	/**
	 * Markets need serial numbers for serialization
	 */
	int ID;

	/**
	 * Gives the next available market ID
	 */
	static int nextID;

	/**
	 * A vector of pointers to the Commodities traded in this Market. (The 
	 * Commodities themselves will reside, from a memory standpoint, in the 
	 * Manager's allCommods data vector.)
	 */
	vector<Commodity*> myCommods;

};

#endif



