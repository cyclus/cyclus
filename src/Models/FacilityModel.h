// FacilityModel.h
#if !defined(_FACILITYMODEL_H)
#define _FACILITYMODEL_H
#include <string>
#include <vector>

#include "Model.h"
#include "Commodity.h"

using namespace std;

//-----------------------------------------------------------------------------
/*
 * The FacilityModel class is the abstract class/interface used by all facility models
 * 
 * This is all that is known externally about facilities
*/
//-----------------------------------------------------------------------------
class FacilityModel : public Model
{

public:
    /// Default constructor for FacilityModel Class
    FacilityModel() { ID = nextID++; };
		
    /// name constructor for FacilityModel Class
    FacilityModel(string facility_name)
        { name = facility_name; ID = nextID++; };

    /// ever model should be destructable
    virtual ~FacilityModel() {};
    
    // every model should be able to print a verbose description
    virtual void print() = 0;
    
    /// get model implementation name
    virtual const string getModelName() = 0;

protected: 
    /// Stores the next available facility ID
    static int nextID;

    /// all facilities must have at least one input commodity
    vector<Commodity*> in_commods;

    /// all facilities must have at least one output commodity
    vector<Commodity*> out_commods;
    
};

#endif



