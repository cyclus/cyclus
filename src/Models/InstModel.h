// InstModel.h
#if !defined(_INSTMODEL_H)
#define _INSTMODEL_H
#include <string>


#include "Model.h"

#include "RegionModel.h"

using namespace std;




//-----------------------------------------------------------------------------
/*
 * The InstModel class is the abstract class/interface used by all institution models
 * 
 * This InstModel is intended as a skeleton to guide the implementation of new
 * Models.
 */
//-----------------------------------------------------------------------------
class InstModel : public Model
{

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
public:
    /// Default constructor for InstModel Class
    InstModel() { ID = nextID++; };

    /// constructor that loads common elements from XML
    InstModel(xmlNodePtr cur);
		
    /// ever model should be destructable
    virtual ~InstModel() {};
    
    // every model should be able to print a verbose description
    virtual void print() = 0;

    /// get model implementation name
    virtual const string getModelName() = 0;

protected: 
    /// Stores the next available institution ID
    static int nextID;
/* ------------------- */ 


/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */

public:
    void setRegion(Model* my_region) { region = my_region; };

protected:
    Model* region;

/* ------------------- */ 
    
};

#endif



