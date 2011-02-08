// RegionModel.h
#if !defined(_REGIONMODEL_H)
#define _REGIONMODEL_H
#include <string>
#include <set>

#include "Model.h"
#include "Communicator.h"

using namespace std;




//-----------------------------------------------------------------------------
/*
 * The RegionModel class is the abstract class/interface used by all region
 * models
 * 
 * This is all that is known externally about Regions
 */
//-----------------------------------------------------------------------------
class RegionModel : public Model, public Communicator
{
	
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
public:
    /// Default constructor for RegionModel Class
    RegionModel() { ID = nextID++; model_type="Region"; commType=RegionComm; };

    /// RegionModels should not be indestructible.
    virtual ~RegionModel() {};
    
    // every model needs a method to initialize from XML
    virtual void init(xmlNodePtr cur);
    // every model needs a method to copy one object to another
    virtual void copy(RegionModel* src);

    // every model should be able to print a verbose description
    virtual void print();

protected:
    /// Stores next available region ID
    static int nextID;
/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
    /// default RegionModel receiver is to ignore messages
    virtual void receiveMessage(Message* msg);

    /**
     * Each region is prompted to do its beginning-of-time-step
     * stuff at the tick of the timer.
     * The default behavior is to ignore the tick.
     *
     * @param time is the time to perform the tick
     */
    virtual void handleTick(int time) {};

    /**
     * Each region is prompted to do its beginning-of-time-step
     * stuff at the tock of the timer.
     * The default behavior is to ignore the tock.
     *
     * @param time is the time to perform the tock
     */
    virtual void handleTock(int time) {};

protected:


/* ------------------- */ 


/* --------------------
 * all REGIONMODEL classes have these members
 * --------------------
 */
public:
    void addInstitution(Model* new_inst) { institutions.push_back(new_inst); };
    bool isAllowedFacility(Model* test_fac) 
    { return ( allowedFacilities.find(test_fac) != allowedFacilities.end() ); } ;

protected:
    /// every region has a list of allowed facilities
    set<Model*> allowedFacilities;

    /// every region has a list of institutions
    vector<Model*> institutions;
/* -------------------- */

};

#endif
