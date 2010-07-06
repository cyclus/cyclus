// Logician.h
#if !defined(_LOGICIAN)
# define _LOGICIAN

#include "Facility.h"
#include "Communicator.h"
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <list>

#define LI Logician::Instance()

using namespace std;

class Market;

/**
 * A (singleton) simulation logician class. This class sends tick messages and 
 * collects and processes requests from simulation objects. 
 */
class Logician : public Communicator
{

public:
	/**
	 * A type of map to register markets and their associated commodities.
	 */
	typedef map<Commodity*, Market*> MarketMap;
		
	/**
	 * Gives all simulation objects global access to the Logician by 
	 * returning a pointer to it.
	 *
	 * @return a pointer to the Logician
	 */
	static Logician* Instance();
		
	/**
	 * Adds the specified Market to the list of Markets this Logician
	 * manages.
	 * 
	 * @param newcomer the market to add
	 */
	void addMarket(Market* newcomer);
	
	/**
	 * Handles this Logician's monthly tick tasks.
	 *
	 * @param time the current time (in months from simulation start)
	 */
	void handleTick(int time);

	/**
	 * Handles this Logician's monthly tock tasks.
	 * 
	 * @param time the current time (in months from simulation start)
	 */
	void handleTock(int time);

	/**
	 * Returns the number of Facility objects loaded into the Logician.
	 *
	 * @return the number of Facility objects
	 */
	int getNumFacs() const;

	/**
	 * Returns the number of Markets loaded into the Logician.
	 *
	 * @return the number of Markets
	 */
	int getNumMarkets() const;

	/**
	 * Sets the duration of the simulation this Logician is overseeing.
	 *
	 * @param numMonths the simulation duration, in months
	 */
	void setSimDur(int numMonths);


	/**
	 * Returns a pointer to the Facility with the given ID number. Throws a 
	 * GenException if none exists.
	 *
	 * @param ID the ID number of the Facility being sought
	 * @return a pointer to that Facility
	 */
	Facility* getFacility(int ID);

	/**
	 * Returns a pointer to the Facility with the given name. Throws a 
	 * GenException if none exists.
	 *
	 * @param name the name of the Facility being sought
	 * @return a pointer to that Facility
	 */
	Facility* getFacility(string name);

	/**
	 * Returns a pointer to the Market with the given ID number. Throws a 
	 * GenException if none exists.
	 *
	 * @param ID the ID number of the Market being sought
	 * @return a pointer to that Market
	 */
	Market* getMarket(int ID);

	/**
	 * Returns a pointer to the Market with the given name. Throws a 
	 * GenException if none exists.
	 *
	 * @param name the name of the Market being sought
	 * @return a pointer to that Market
	 */
	Market* getMarket(string name);
	/**
	 * Returns a begin() and end() iterator over this Logician's Markets.
	 *
	 * @return the iterator
	 */
	pair<vector<Market*>::iterator, vector<Market*>::iterator> getMarkets();

	/**
	 * Returns a begin() and end() iterator over this Logician's Facilities.
	 *
	 * @return the iterator
	 */
	pair<vector<Facility*>::iterator, vector<Facility*>::iterator> getFacilities();

	/**
	 * Returns a pointer to the Commodity with the given ID number. Throws a 
	 * GenException if none exists.
	 *
	 * @param ID the ID number of the Commodity being sought
	 * @return a pointer to that Commodity
	 */
	Commodity* getCommod(int ID);

	/**
	 * Returns a pointer to the Commodity with the given name. Throws a 
	 * GenException if none exists.
	 *
	 * @param name the name of the Commodity being sought
	 * @return a pointer to that Commodity
	 */
	Commodity* getCommod(string name);

	/**
	 * The map that holds the markets and their commodities.
	 * The keys are commodities, and the values are the markets
	 * on which they're traded.
	 */
	MarketMap mktMap;

	/**
	 * Performs any final tasks that must be completed at the end of the 
	 * simulation, including passing the word to the Markets to do the same.
	 *
	 * @param time the current time
	 */
	void handleEnd(int time);

	/**
	 * Receives a message passed up to it from a region.
	 *
	 * @param theMessage is a pointer to the Message object
	 */
	virtual void receiveMessage(Message* theMessage);

	/**
	 * (Recursively) deletes this Logician (and the objects it manages).
	 */
	virtual ~Logician();

protected:

	/**
	 * The (protected) constructor for this class, which can only 
	 * be called indirectly by the client.
	 */
	Logician();
private:
	/**
	 * The vector where pointers to all the Facilities for this simulation 
	 * are stored.
	 */
	vector<Facility*> facilities;

	/**
	 * The vector where pointers to all the Markets for this simulation 
	 * are stored.
	 */
	vector<Market*> markets;

	/**
	 * A pointer to this Logician once it has been initialized.
	 */
	static Logician* _instance;

	/**
	 * The length of this simulation, in months.
	 */
	int simDur;

};
#endif
