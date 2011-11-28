// RegionModel.h
#if !defined(_REGIONMODEL_H)
#define _REGIONMODEL_H
#include <set>
#include <vector>

#include "TimeAgent.h"
#include "Communicator.h"

//-----------------------------------------------------------------------------
/*
 * The RegionModel class is the abstract class/interface used by all region
 * models
 * 
 * This is all that is known externally about Regions
 */
//-----------------------------------------------------------------------------
class RegionModel : public TimeAgent, public Communicator {
	
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
public:
    /// Default constructor for RegionModel Class
    RegionModel();

    /// RegionModels should not be indestructible.
    virtual ~RegionModel() {};
    
    // every model needs a method to initialize from XML
    virtual void init(xmlNodePtr cur);

    // every model needs a method to copy one object to another
    virtual void copy(RegionModel* src);

    /**
     * This drills down the dependency tree to initialize all relevant parameters/containers.
     *
     * Note that this function must be defined only in the specific model in question and not in any 
     * inherited models preceding it.
     *
     * @param src the pointer to the original (initialized ?) model to be copied
     */
    virtual void copyFreshModel(Model* src)=0;

    // every model should be able to print a verbose description
    virtual void print();

public:
    /// default RegionModel receiver is to ignore messages
    virtual void receiveMessage(Message* msg);

    /**
     * Each region is prompted to do its beginning-of-life-step
     * stuff before the simulation begins.
     *
     * Normally, Regions simply hand the command down to institutions.
     *
     */
    virtual void handlePreHistory();

    /**
     * @brief Each region is prompted to do its beginning-of-time-step
     * stuff at the tick of the timer.
     * The default behavior is to ignore the tick.
     *
     * @param time is the time to perform the tick
     */
    virtual void handleTick(int time);

    /**
     * @brief Each region is prompted to do its end-of-time-step
     * stuff at the tock of the timer.
     * The default behavior is to ignore the tock.
     *
     * @param time is the time to perform the tock
     */
    virtual void handleTock(int time);

    /**
     * @brief Each region is prompted to do its daily task.
     *
     * @param time is the month since the start of the simulation
     * @param day is the current day of that month
     */
    virtual void handleDailyTasks(int time, int day);

public:
    bool isAllowedFacility(Model* test_fac) 
    { return ( allowedFacilities_.find(test_fac) != allowedFacilities_.end() ); } ;

protected:
    /// every region has a list of allowed facilities
    std::set<Model*> allowedFacilities_;
/* -------------------- */

};

#endif
