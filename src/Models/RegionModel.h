// RegionModel.h
#if !defined(_REGIONMODEL_H)
#define _REGIONMODEL_H
#include <string>

#include "Model.h"

using namespace std;

//-----------------------------------------------------------------------------
/*
 * The RegionModel class is the abstract class/interface used by all region models
 * 
 * This is all that is known externally about Regions
 */
//-----------------------------------------------------------------------------
class RegionModel : public Model
{
	
public:
    /// Default constructor for RegionModel Class
    RegionModel() { ID = nextID++; };

    /// name constructor for RegionModel Class
    RegionModel(string region_name)
        { name = region_name; ID = nextID++; };

    /// RegionModels should not be indestructible.
    virtual ~RegionModel() {};
    
    // every model should be able to print a verbose description
    virtual void print() = 0;
    
    /// get model implementation name
    virtual const string getModelName() = 0;

protected:
    /// Stores next available region ID
    static int nextID;

    /// every region has a list of allowed facilities
    vector<Model*> allowedFacilities;

};

#endif
