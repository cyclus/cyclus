// FacilityModel.h
#if !defined(_FACILITYMODEL_H)
#define _FACILITYMODEL_H
#include <string>
#include <vector>

#include "Model.h"
#include "Communicator.h"

using namespace std;

/// forward declaration to resolve recursion
class Commodity;
// forward declare Material class to avoid full inclusion and dependency
class Material;

//-----------------------------------------------------------------------------
/*
 * The FacilityModel class is the abstract class/interface used by all
 * facility models
 * 
 * This is all that is known externally about facilities
*/
//-----------------------------------------------------------------------------
class FacilityModel : public Model, public Communicator
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
    /// Default constructor for FacilityModel Class
    FacilityModel() { ID = nextID++; model_type="Facility"; commType=FacilityComm; };

    /// ever model should be destructable
    virtual ~FacilityModel() {};
    
    // every model needs a method to initialize from XML
    virtual void init(xmlNodePtr cur);
    // every model needs a method to copy one object to another
    virtual void copy(FacilityModel* src);

    // every model should be able to print a verbose description
    virtual void print()              { Model::print(); };

protected: 
    /// Stores the next available facility ID
    static int nextID;
/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
    /**
     *  There is no default FacilityModel receiver 
     *
     *  Each derived class must implement an offer/request receiver
     */ 
    virtual void receiveOfferRequest(OfferRequest* msg) = 0;

protected:


/* ------------------- */ 


/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */
protected:
    /// each instance of a facility needs a name
    string fac_name;

public:

    void setFacName(string facName) { fac_name = facName; };
    string getFacName() { return fac_name; };

    /**
     *  There is no default FacilityModel shipment sender
     *
     *  Each derived class must implement a shipment sender
     */ 
    virtual void sendMaterial(Transaction trans, Communicator* receiver) = 0;
    /**
     *  There is no default FacilityModel shipment receiver 
     *
     *  Each derived class must implement an shipment receiver
     */ 
    virtual void receiveMaterial(Transaction trans, vector<Material*> manifest) = 0;
    

/* ------------------- */ 
    
};

#endif



