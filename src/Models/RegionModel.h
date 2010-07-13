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
    RegionModel() { ID = nextID++; };

    /// Constructor based on XML node
    RegionModel(xmlNodePtr cur);

    /// RegionModels should not be indestructible.
    virtual ~RegionModel() {};
    
    // every model should be able to print a verbose description
    virtual void print() = 0;
    
    /// get model implementation name
    virtual const string getModelName() = 0;

protected:
    /// Stores next available region ID
    static int nextID;
/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
    /// default RegionModel receiver has no default behavior
    virtual void receiveOfferRequest(OfferRequest* msg) = 0;

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
