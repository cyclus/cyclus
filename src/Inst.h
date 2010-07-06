// Inst.h

#if !defined(_INST)
# define _INST
#include <vector>
#include <string>
#include <map>
#include "Communicator.h"

using namespace std;

class Region;
class Facility;

/**
 * An Institution class for agencies, corporations, and the like.
 */
class Inst : public Communicator
{
private:
	
	/**
	 * The string name of this Institution.
	 */
	string name;

	/**
	 * The ID number for this Institution.
	 */
	int ID;
	
	/**
	 * A vector of pointers to the Facilities 'owned' by this Institution.
	 */
	vector<Facility*> myFacs; 
		
	/**
	 * A pointer to the Region this Institution serves.
	 */
	Region* myRegion;
	
	/**
	 * The gross capacity of all this Institution's reactors. Eventually
	 * this'll probably be a map with the capacity of the Inst's other 
	 * types of facilities as well.
	 */
	double grossPowerCap;
	
	/**
	 * The (current) annual fixed charge rate for this Institution.
	 */ 
	double annFixedChargeRt;
	
	/**
	 * A time vector of this Institution's annual fixed charge rate.
	 */
	vector<double> chargeRateLog;
	
	/**
	 * The duration of the simulation this Institution is taking part in.
	 */
	int simDur;

	/**
	 * A map whose keys are FacModel names to build and whose 
	 * values are build vectors for that facility 
	 */
	map<string, vector<int> > build;
	
public:
	
	/**
	 * Constructs an Institution with the specified name, Region, and 
	 * annual fixed charge rate.
	 *
	 * @param s the name of this Institution
	 * @param SN the ID number for this Institution
	 * @param reg a pointer to the Region this Institution serves
	 * @param phi the annual fixed charge rate for this Institution
	 * @param dur this simulation's duration
	 */
	Inst(string s, int SN, Region* reg, double phi, int dur);
	
	/**
	 * Builds a new Facility of type specified by facType.
	 *
	 * @param facModel the nickname of the Facility to build
	 * @param time the current time (in months)
	 */
	void buildFac(string facModel, int time);
	
	/**
	 * Adds the given Facility to this Inst's holdings. For use when initializing
	 * a scenario.
	 *
	 * @param f the Facility to add
	 */
	void addFac(Facility* f);

	/**
	 * Returns a pointer to the Facility with the given ID number, if it's owned 
	 * by this Inst. Throws a FacException otherwise.
	 *
	 * @param SN the Facility in question's ID number
	 * @return the Facility in question
	 */
	Facility* getFac(int SN);
	
	/**
	 * Returns this Institution's name.
	 *
	 * @return the name
	 */
	virtual const string getName() const;

	/**
	 * Returns this Institution's ID number.
	 *
	 * @return the ID number
	 */
	virtual int getSN() const;
	
	/**
	 * Returns a pointer to this Institution's Region.
	 *
	 * @return the pointer to the Region
	 */
	Region* getRegion();
	
	/**
	 * Sets the given build information for the given FacType and SubType 
	 * for this Inst for this simulation.
	 *
	 * @param facModel the model for which we want to record build information 
	 * @param v the build vector
	 */
	void setBuild(string facModel, vector<int> v);
	
	/**
	 * Handles this Institution's monthly tick tasks.
	 *
	 * @param time the current time (in months from simulation start)
	 */
	void handleTick(int time);
	
	/**
	 * Handles this Institution's monthly tock tasks.
	 *
	 * @param time the current time (in months from simulation start)
	 */
	void handleTock(int time);
	
	/**
	 * Returns the annual fixed charge rate time vector for this 
	 * Institution (for financial calculations).
	 *
	 * @return the fixed charge rate log
	 */
	vector<double>* getChargeRateLog();

/**
	 * Receives the given Message, handling it if the Message is 
	 * addressed to this Inst and forwarding it on appropriately if not.
	 *
	 * @param theMessage the Message being received
	 */
	virtual void receiveMessage(Message* theMessage);

	/**
	 * (Recursively) deletes this Institution and its Facilities.
	 */
	virtual ~Inst();

	/**
	 * Performs any final tasks that must be completed at the end of the 
	 * simulation, including passing the word to the Facs to do the same.
	 *
	 * @param time the current time
	 */
	void handleEnd(int time);

	/**
	 * Insertion stream operator for a Inst.
	 */
	friend ostream& operator<<(ostream &os, const Inst& i);

	/**
	 * Insertion stream operator for a pointer to a Inst.
	 */
	friend ostream& operator<<(ostream &os, const Inst* i);
};
#endif
