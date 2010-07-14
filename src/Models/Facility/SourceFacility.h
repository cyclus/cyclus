// SourceFacility.h
#if !defined(_SOURCEFACILITY_H)
#define _SOURCEFACILITY_H
#include <iostream>

#include "FacilityModel.h"

/**
 * The SourceFacility class inherits from the FacilityModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This FacilityModel will provide a simple source of some capacity (possibly infinite)
 * of some Commodity/Recipe
 *
 */
class SourceFacility : public FacilityModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
    SourceFacility() {};
    
    ~SourceFacility() {};

    // different ways to populate an object after creation
    /// initialize an object from XML input
    virtual void init(xmlNodePtr cur);
    /// initialize an object by copying another
    virtual void copy(SourceFacility* src);

    virtual void print();

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
    /// simply ignore incoming offers/requests
    virtual void receiveOfferRequest(OfferRequest* msg) {};

/* -------------------- */

/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

public:
    /// simply do nothing when sending a shipment
    virtual void sendMaterial(Transaction trans, Communicator* receiver) {};
    
    /// simply do nothing when receiving a shipment
    virtual void receiveMaterial(Transaction trans, vector<Material*> manifest) {};


/* ------------------- */ 

/* --------------------
 * _THIS_ FACILITYMODEL class has these members
 * --------------------
 */

protected:
    /// this facility has only one output commodity
    Commodity* out_commod;
    
    /// this facility has a specific recipe for its output
    Material* recipe;
    
    /// this facility has a specific capcity
    /**
     *  The capacity is defined in terms of the number of units of the recipe
     *  that can be provided each time step.  A very large number can be
     *  provided to represent infinte capacity.
     */
    double capacity;


/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new SourceFacility();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* ------------------- */ 

#endif
