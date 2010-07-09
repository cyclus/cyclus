// FacilityModel.h
#if !defined(_FACILITYMODEL_H)
#define _FACILITYMODEL_H
#include <string>
#include <vector>

#include "Model.h"

using namespace std;

/// forward declaration to resolve recursion
class Commodity;

//-----------------------------------------------------------------------------
/*
 * The FacilityModel class is the abstract class/interface used by all
 * facility models
 * 
 * This is all that is known externally about facilities
*/
//-----------------------------------------------------------------------------
class FacilityModel : public Model
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
    /// Default constructor for FacilityModel Class
    FacilityModel() { ID = nextID++; };

    /// Constructor based on XML node
    FacilityModel(xmlNodePtr cur);
		
    /// ever model should be destructable
    virtual ~FacilityModel() {};
    
    // every model should be able to print a verbose description
    virtual void print() = 0;
    
    /// get model implementation name
    virtual const string getModelName() = 0;

protected: 
    /// Stores the next available facility ID
    static int nextID;
/* ------------------- */ 


/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */
protected:
    /// all facilities must have at least one input commodity
    vector<Commodity*> in_commods;

    /// all facilities must have at least one output commodity
    vector<Commodity*> out_commods;
/* ------------------- */ 
    
};

#endif



