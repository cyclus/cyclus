// StubModel.h
#if !defined(_STUBMODEL_H)
#define _STUBMODEL_H
#include <string>


#include "Model.h"


using namespace std;




//-----------------------------------------------------------------------------
/*
 * The StubModel class is the abstract class/interface used by all stub models
 * 
 * This StubModel is intended as a skeleton to guide the implementation of new
 * Models.
 */
//-----------------------------------------------------------------------------
class StubModel : public Model
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
    /// Default constructor for StubModel Class
    StubModel() { ID = nextID++; model_type="Stub"; };

    /// every model should be destructable
    virtual ~StubModel() {};
    
    // every model needs a method to initialize from XML
    virtual void init(xmlNodePtr cur) { Model::init(cur); } ;
    // every model needs a method to copy one object to another
    virtual void copy(StubModel* src) { Model::copy(src); } ;

    // every model should be able to print a verbose description
    virtual void print() { Model::print(); cout << endl; } ;

protected: 
    /// Stores the next available stub ID
    static int nextID;
/* ------------------- */ 


/* --------------------
 * all STUBMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 
    
};

#endif



