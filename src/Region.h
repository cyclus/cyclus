// Region.h
#if !defined(_REGION)
#define _REGION
#include "Communicator.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

/**
 * An enumerative type to specify this Region's "fuel-status"; there are 
 * fuel-cycle (FC) and non-fuel-cycle (nonFC) states.
 */
enum RegionType {FC, nonFC};

class Inst;

/**
 * An area in which there is some demand for nuclear power and some Facilities,
 * Institutions, etc. 
 */
class Region : public Communicator 
{
	
private: 

	/**
	 * This Region's fuel-cycle status (for now, either FC or nonFC).
	 */
	RegionType myType;
		
	/**
	 * The total electric production capacity of this Region (gross, 
	 * because it doesn't exclude losses due to non-unity capacity 
	 * factors), in MWe.
	 */
	double grossPowerCap;		

	/**
	 * The name of this region.
	 */
	string name;

	/**
	 * The ID number for this Region.
	 */
	int ID;

	/**
	 * A vector representing this Region's demand. It has a demand value 
	 * (in MWe) for each time step in the simulation.
	 */
	vector<double> demand;

	/**
	 * A vector of pointers to the Institutions serving this Region. (The 
	 * Institutions themselves reside, from a memory standpoint, in the 
	 * Logician's allInsts data vector.)
	 */
	vector<Inst*> myInsts;

	/**
	 * The duration of the simulation this Region is taking part in.
	 */
	int simDur;

public:

	/**
	 * Constructs a Region of fuel-cycle status type for a simulation of 
	 * duration dur.
	 *
	 * @param type FC for fuel cycle, nonFC for non-fuel cycle
	 * @param regName the name of this Region
	 * @param SN the ID number for this Region.
	 * @param dur the duration of this simulation, in months
	 */
	Region(RegionType type, string regName, int SN, int dur);

	/**
	 * Adds an Institution with the specified parameters to this Region.
	 *
	 * @param s the name of the Institution to add
	 * @param SN the ID number of the Institution to add
	 * @param phi the Institution's annual fixed charge rate
	 */
	void addInst(string s, int SN, double phi);

	/**
	 * Adds the given pre-constructed Inst to this Region.
	 *
	 * @param i the Institution to add
	 */
	void addInst(Inst* i);
		
	/**
	 * Returns a pointer to the Institution with the given ID number, if it 
	 * exists in this Region. Throws a GenException otherwise.
	 *
	 * @param SN the ID number of the Inst being sought
	 * @return a pointer to it, if it's in this Region
	 */
	Inst* getInst(int SN);

	/**
	 * Returns a pointer to the Institution with the given name, if it 
	 * exists in this Region. Throws a GenException otherwise.
	 *
	 * @param name the name of the Inst being sought
	 * @return a pointer to it, if it's in this Region
	 */
	Inst* getInst(string name);
	
	// /**
	//  * Returns the entire vector of pointers to this Region's Institutions.
	//  *
	//  * @return the vector of pointers to Institutions
	//  */
	// vector<Inst*>* getInsts();

	/**
	 * Returns the begin() and end() iterators over this Region's Insts.
	 *
	 * @return the pair of iterators
	 */
	pair<vector<Inst*>::iterator , vector<Inst*>::iterator> getInsts();


	/**
	 * Returns this Region's ID number.
	 */
	virtual int getSN() const;

	/**
	 * Returns this Region's name.
	 */
	virtual const string getName() const;

	/**
	 * Returns the number of Insts in this Region.
	 *
	 * @return the number of Insts
	 */
	int getNumInsts() const;

	/**
	 * Returns the RegionType for this Region.
	 *
	 * @return the RegionType
	 */
	RegionType getType();
	
	/**
	 * Makes the specified vector of doubles the demand vector for this 
	 * Region.
	 *
	 * Outstanding issue: We'll need to figure out if this is adjustable. 
	 * If so, it would complicate calculating the objective functions 
	 * (i.e., if you allow a replacement demand vector, you could be 
	 * calculating the objective functions using demand values for the 
	 * early months that aren't acutually accurate--possible fix, only 
	 * copy the values in the new demand vector after the current time).
	 *
	 * @param dem the demand vector for this Region
	 */
	void setDemand(vector<double>& dem);
	
	/**
	 * Gets the demand (in MWe) for this Region at the specified time.
	 *
	 * @param time the time (in months since the beginning of the sim.)
	 * for which you're interested in the demand
	 * @return the demand, in MWe
	 */
	double getDemand(int time);
	
	/**
	 * Returns a pointer to the time vector of the power demand for this 
	 * region.
	 *
	 * @return a pointer to the demand vector
	 */
	vector<double>* getDemand();
	
	/**
	 * Handles this Region's monthly tick tasks.
	 *
	 * @param time the current time
	 */
	void handleTick(int time);
	
	/**
	 * Handles this Region's monthly tock tasks.
	 *
	 * @param time the current time
	 */
	void handleTock(int time);
	
/**
	 * Receives the given Message, handling it if the Message is 
	 * addressed to this Region and forwarding it on appropriately if not.
	 *
	 * @param theMessage the Message being received
	 * 
	 */
	 
	virtual void receiveMessage(Message* theMessage);

	/**
	 * (Recursively) deletes this Region (and its Institutions).
	 */
	virtual ~Region();
	
	/**
	 * Performs any final tasks that must be completed at the end of the 
	 * simulation, including passing the word to the Insts to do the same.
	 *
	 * @param time the current time
	 */
	void handleEnd(int time);

	/**
	 * Insertion stream operator for a Region.
	 */
	friend ostream& operator<<(ostream &os, const Region& r);

	/**
	 * Insertion stream operator for a pointer to a Region.
	 */
	friend ostream& operator<<(ostream &os, const Region* r);
};
#endif
