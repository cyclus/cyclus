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
//#include "Commodity.h"

using namespace std;

class Material;
class Commodity;
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
	 * Initially, these facilities only know how to print their names
	 */
	virtual void printMyName();
	
	
	protected: 
		
	/**
	 * The name for this Facility. The name should be either the 
	 * real-world name of the Facility if it models an existing one, or 
	 * facilityTypeX, where "facilityType" is a string representation of 
	 * the Facility's class (the most specialized class that applies) and 
	 * X is a serial number assigned by the BookKeeper (and reflecting the 
	 * order of construction).
	 */
	string name;
		
	/**
	 * The ID number for this Facility.
	 */
	int ID;
		
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
	 * The capacity factor for this Facility, i.e., the fraction of the 
	 * time this Facility operates. For early implementations, we probably  
	 * won't use this field--it will be calculated as an objective 
	 * function.
	 */ 
	double capFactor; // between 0 and 1

	/**
	 * The duration of the simulation this Facility is taking part in.
	 */
	int simDur;

	/**
	 * The total mass flow required to process all outstanding orders this 
	 * Facility has already committed to. Units are tons, sometimes of 
	 * uranium and sometimes of certain isotopes. For Enrichment, they're 
	 * tons U SWU. For Fuel Fab, they're tons U.
	 */
	double outstMF;

	/**
	 * A map whose keys are times at which this Facility ordered Material 
	 * for a given order and the values are the orders themselves.
	 */
//	multimap<int, Message*> ordersWaiting;

	/**
	 * A map whose keys are times at which this Facility will finish 
	 * executing a given order and the values are pairs comprising the orders 
	 * themselves and the Materials each is to be made with.
	 */
//	ProcessLine ordersExecuting;
	
	/**
	 * Returns some measure of the Material currently in the stocks, less 
	 * any Matieral that's already been earmarked. Default metric is mass of 
	 * uranium (in tons); otherwise overwrite this function.
	 *
	 * @return the measure of Material in stock
	 */
	virtual double checkStocks();

	/**
	 * Returns some measure of the Material currently in the inventory, less 
	 * any Material that's already been earmarked. Default metric is the of 
	 * uranium (in tons); otherwise overwrite this function.
	 *
	 * @return the measure of Material in inventory
	 */
	virtual double checkInventory();

	/**
	 * Returns true if the given Material object can be used to fill the given 
	 * order, false otherwise.
	 *
	 * @param candMat the candidate Material object
	 * @param order the order we're trying to fill
	 * @return true iff they're compatible
	 */
//	virtual bool areCompat(Material* candMat, Message* order);

	/**
	 * Begin's this Facility's next operation cycle.
	 *
	 * @param time the current time
	 */
	virtual void beginCycle(int time);

public:
		
	/**
	 * Constructs a Facility with the specified parameters.
	 *
	 * @param start the startup date (a simulation time, in months)
	 * @param i a pointer to this Facility's Institution
	 * @param s a string representatino of this Facility's name
	 * @param SN the ID number for this Facility 
	 * @param dur this simulation's duration
	 */
	Facility(int start, string s, int SN, int dur);
		
	/**
	 * Constructs a Facility with the specified data.
	 *
	 * @param sCon the time construction started on this Facility
	 * @param sOp the time operation started on this Facility
	 * @param i a pointer to the Institution this Reactor belongs to.
	 * @param name this Reactor's name
	 * @param SN the ID number for this Reactor
	 * @param dur this simulation's duration
	 * @param capFac the current capacity factor of this Reactor
	 * @param time the current time
	 * @param cycle the cycle/process time for this Facility.
	 * @param feeds a list of Commodity type(s) this Facility uses as raw material
	 * @param prods a list of Commoidty type(s) this Facility produces
	 */
	Facility(int sCon, int sOp, string name, int SN, int dur,  
					 double capFac, int time, int cycle, 
					 list<Commodity*> feeds, list<Commodity*> prods);

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
	 * Returns the capacity of this Facility (units vary).
	 *
	 * @return the capacity of this Facility
	 */
	double getCapacity() const;

	/**
	 * Returns the date (a simulation time, in months) on which this 
	 * Facility started operation.
	 *
	 * @return the operation start date
	 */
	int getStartOp() const;
		
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
	 * Returns this Facility's inventory.
	 *
	 * @return the inventory queue
	 */
	deque<Material*>* getInventory();

	/**
	 * Sends the given Material to the given Communicator. This includes logging of 
	 * the transfer, so be sure to use it rather than calling receiveMaterial 
	 * directly.
	 *
	 * @param mat the Material to be sent
	 * @param rec the receiver of the given Material
	 * @param time the current time
	 */
	//virtual void sendMaterial(Material* mat, Communicator* rec, int time);

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
	 * Checks to see if the given candidate Material can be used for enrichment
	 * of any of the ordersWaiting for Material. If so, returns 
	 * an iterator pointing to that item. If not, returns an iterator just past 
	 * the last element.
	 *
	 * @param candMat the candidate material
	 * @return the iterator
	 */
//	virtual multimap<int,Message*>::iterator checkOrdersWaiting(Material* candMat);

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


};

#endif



