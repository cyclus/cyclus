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
    StubCommModel() { ID = nextID++; };

    /// constructor that loads common elements from XML
    StubCommModel(xmlNodePtr cur);
		
    /// every model should be destructable
    virtual ~StubCommModel() {};
    
    // every model should be able to print a verbose description
    virtual void print() = 0;

    /// get model implementation name
    virtual const string getModelName() = 0;

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
    virtual void receiveOfferRequest(OfferRequest* msg) = 0;

protected:


/* ------------------- */ 


/* --------------------
 * all STUBCOMMMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 
    
};

#endif



