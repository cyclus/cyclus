// StubCommModel.h
#if !defined(_STUBCOMMMODEL_H)
#define _STUBCOMMMODEL_H
#include <string>


#include "Model.h"
#include "Communicator.h"

using namespace std;




//-----------------------------------------------------------------------------
/*
 * The StubCommModel class is the abstract class/interface used by all stub models
 * 
 * This StubCommModel is intended as a skeleton to guide the implementation of new
 * Models.
 */
//-----------------------------------------------------------------------------
class StubCommModel : public Model, public Communicator
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
    /// Default constructor for StubCommModel Class
    StubCommModel() { ID = nextID++; model_type="StubComm"; commType=StubComm; };

    /// every model should be destructable
    virtual ~StubCommModel() {};
    
    // every model needs a method to initialize from XML
    virtual void init(xmlNodePtr cur)     { Model::init(cur); } ;
    // every model needs a method to copy one object to another
    virtual void copy(StubCommModel* src) { Model::copy(src); 
                                            Communicator:copy(src); };

    // every model should be able to print a verbose description
    virtual void print()              { Model::print(); cout << endl; } ;

protected: 
    /// Stores the next available stub ID
    static int nextID;
/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
    /// No default STUBCOMMMODEL message receiver.
    virtual void receiveMessage(Message* msg) = 0;

protected:


/* ------------------- */ 


/* --------------------
 * all STUBCOMMMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 
    
};

#endif



