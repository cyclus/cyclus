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

public:
    /// Default constructor for StubModel Class
    StubModel() { ID = nextID++; };
		
    /// name constructor for StubModel Class
    StubModel(string stub_name)
        { name = stub_name; ID = nextID++; };

    /// ever model should be destructable
    virtual ~StubModel() = {};
    
    // every model should be able to print a verbose description
    virtual void print() = 0;

    /// get model implementation name
    virtual const string getModelName() = 0;

protected: 
    /// Stores the next available stub ID
    static int nextID;

    
};

#endif



