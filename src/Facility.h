// Facility.h

#if !defined(_FACILITY)
#define _FACILITY
#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <queue>
#include <vector>
#include <list>
#include "Commodity.h"

using namespace std;

class Material;
class Commodity;
class Inst;
//-----------------------------------------------------------------------------
/*
 * The Facility class is the abstract class used by all objects that our 
 * factory will be responsible for creating, or manufacturing. 
 *
 * This is all our factory really knows about the products it's creating
*/
//-----------------------------------------------------------------------------
class Facility
{

public:
	/**
	 * Constructor for a generic Facility
	 */
	Facility();
		
	/**
	 * Constructs a Facility with the specified data.
	 *
	 * @param name this Facility's name
	 * @param i is the Institution to which this Facility belongs
	 * @param SN the ID number for this Facility
	 * @param feeds a list of Commodity type(s) this Facility uses as raw material
	 * @param prods a list of Commoidty type(s) this Facility produces
	 */
	Facility(string name, Inst* i, int SN, list<Commodity*> feeds, list<Commodity*> prods);

	/** 
	 * Initially, these facilities only know how to print their names
	 */
	virtual void printMyName();
	
	/**
	 * Returns the name of this Facility.
	 *
	 * @return the name of this Facility
	 */
	virtual const string getName() const;
		
	/**
	 * Returns the ID number of this Facility.
	 *
	 * @return the ID number of this Faciility
	 */
	virtual int getSN() const;
		
	/**
	 * Returns a pointer to this Facility's owner Institution.
	 *
	 * @return the pointer to this Facility's owner
	 */
	Inst* getInst();	
		
	/**
	 * Handles this Facility's monthly tick tasks.
	 *
	 * @param time the current simulation time (in months)
	 */
	virtual void handleTick(int time);

	/**
	 * Handles this Facility's monthly tock tasks.
	 *
	 * @param time the current simulation time (in months)
	 */
	virtual void handleTock(int time);

	/**
	 * (Recursively) delete this Facility (and its Materials).
	 */
	virtual ~Facility();

	/**
	 * Sends the given Material to the given Communicator. This includes logging of 
	 * the transfer, so be sure to use it rather than calling receiveMaterial 
	 * directly.
	 *
	 * @param mat the Material to be sent
	 * @param time the current time
	 */
	virtual void sendMaterial(Material* mat, int time);

	/**
	 * Receives the given Material. The default behavior is to 
	 * append the given Material to this Facility's stocks. 
	 * Subclasses that need a different behavior should override this function.
	 *
	 * !! NOTE: This function should not be called directly but, rather, should 
	 * be called VIA the sendMaterial function of the object sending the Material.
	 * Failure to do so will result in an un-logged transfer. !!
	 *
	 * @param mat the Material to be received
	 * @param time the current time
	 */
	virtual void receiveMaterial(Material* mat, int time);

	/**
	 * Performs any first tasks that must be completed at the beginning of the 
	 * simulation. An obvious example would be preloading the Facility with 
	 * with some Material.
	 *
	 * @param time the current time
	 */
	virtual void handleStart(int time);

	/**
	 * Performs any final tasks that must be completed at the end of the 
	 * simulation. For Faciities this includes deleting all the Materials so 
	 * that their histories get recorded.
	 *
	 * @param time the current time
	 */
	virtual void handleEnd(int time);

	/**
	 * Via some process, decides what type of Commodity to request right now.
	 * 
	 * @return the Commodity (Material type) to request
	 */
	virtual Commodity* pickRequestCommod();

	/**
	 * Via some process, decides what type of Commodity to offer right now.
	 * 
	 * @return the Commodity (Material type) to offer
	 */
	virtual Commodity* pickOfferCommod();

	/**
	 * Decomissions this Facility.
	 */
	virtual void decommission();

	/**
	 * Advances the nextID facility ID number.
	 */
	static int getNextID();

protected: 
		
	/**
	 * The name for this Facility.  
	 */
	string name;
		
	/**
	 * The ID number for this Facility.
	 */
	int ID;
		
	/**
	 * The ID number for the next Facility.
	 */
	static int nextID;

	/**
	 * The date (simulation time, in months) on which construction began
	 * for this Facility.
	 */
	int startConstr; 
		
	/**
	 * The date (simulation time, in months) on which operation began for 
	 * this Facility.
	 */ 
	int startOp;


	/**
	 * A collection  that holds the "product" Material this Facility has on 
	 * hand to send to others. For instance, a Reactor's inventory is its 
	 * collection of old fuel assemblies that have come out of the core.
	 *
	 * @see stocks
	 * @see waste
	 */ 
	deque<Material*> inventory;

	/**
	 * The Institution to which this Facility belongs (for instance, a 
	 * utility or government entity).
	 */ 
	Inst* myInst;

	/** 
	 * A collection of Material objects that in some sense represents "raw 
	 * materials" that this Facility uses to make useful products. Not to be 
	 * confused with inventory, although there's problably some gray area.
	 * Example: a Conversion Facility should keep yellowcake in its stocks
	 * and (unenriched) UF6 in its inventory.
	 *
	 * @see inventory
	 * @see waste
	 */
	deque<Material*> stocks;

	/** 
	 * A collection of Material objects that represents waste sitting around 
	 * at a Facility. Going to have to work out as we go when certain Materials 
	 * (mostly spent fuel), should go here or in the inventory. I think under 
	 * the materials routing problem LP-formulations we've discussed, anything 
	 * that's potentially repository-bound, including spent fuel, should 
	 * probably be stored and treated as "waste" rather than another commodity, 
	 * and therefore should be stored here.
	 *
	 * @see inventory
	 * @see stocks
	 */
	deque<Material*> wastes;

	/**
	 * Feed Commodities
	 */
	list<Commodity*> feeds;
	
	/**
	 * Product Commodities
	 */
	list<Commodity*> prods;

	/**
	 * The duration of the simulation this Facility is taking part in.
	 */
	int simDur;

	/**
	 * Begin's this Facility's next operation cycle.
	 *
	 * @param time the current time
	 */
	virtual void beginCycle(int time);

};

#endif



