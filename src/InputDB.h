// InputDB.h
#if !defined(_INPUTDB)
# define _INPUTDB
#include <deque>
#include <queue>
#include <list>
#include "Material.h"

#define INDB InputDB::Instance()

using namespace std;

/**
 * A (singleton) fake database to mimic the information that will 
 * someday be contained in the cyclus input database
 */
class InputDB
{
private:
		
	/**
	 * A pointer to this InputDB once it has been initialized.
	 */
	static InputDB* _instance;

protected:
		
	/**
	 * Constructs a new InputDB for this simulation.
	 */
	InputDB();
	
public:

	/**
	 * Gives all simulation objects global access to the InputDB by 
	 * returning a pointer to it.
	 *
	 * @return a pointer to the InputDB
	 */
	static InputDB* Instance();

	/**
	 * Returns the current facID to be constructed.
	 *
	 * @return ID 
	 */
	int getFacID();

	/**
	 * Returns a test name for the facility with ID=facID.
	 *
	 * @param facID is the facility ID
	 * @return name 
	 */
	string getFacName(int facID);

	/**
	 * Returns a test name for the market with ID=mktID.
	 * 
	 * @param mktID is the market ID
	 * @return name 
	 */
	string getMktName(int mktID);

	/**
	 * Returns a deque of input recipes for the facility with ID facID.
	 *
	 * @param facID is the facility ID
	 * @return inRecs
	 */
	deque<Material*> getInRecs(int facID);

	/**
	 * Returns a deque of output recipes for the facility with facID ID.
	 *
	 * @param facID is the facility ID
	 * @return outRecs
	 */
	deque<Material*> getOutRecs(int facID);

	/**
	 * Returns a feed commodities for the facility with facID ID.
	 *
	 * @param ID is the facID
	 * @return feeds
	 */
	list<Commodity*> getFeeds(int ID);

	/**
	 * Returns a feed commodities for the facility with facID ID.
	 *
	 * @param ID is the facID
	 * @return feeds
	 */
	list<Commodity*> getProds(int ID);


	/**
	 * Converts the given date into a GENIUS time.
	 *
	 * @param month the month corresponding to the date (Jan = 1, etc.)
	 * @param year the year corresponding to the date
	 * @return the GENIUS date
	 * 
	 */
	int convertDate(int month, int year);

	/**
	 * Converts the given GENIUS time into a (month, year) pair.
	 */
	pair<int, int> convertDate(int time);


};
#endif
