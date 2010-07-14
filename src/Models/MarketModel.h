// MarketModel.h
#if !defined(_MARKETMODEL_H)
#define _MARKETMODEL_H
#include <string>
#include <deque>
#include <set>

#include "Model.h"
#include "Communicator.h"

using namespace std;

/// forward declaration to resolve recursion
class Commodity;

//-----------------------------------------------------------------------------
/*
 * The MarketModel class is the abstract class/interface used by all market
 * models
 * 
 * This is all that is known externally about Markets
 */
//-----------------------------------------------------------------------------
class MarketModel : public Model, public Communicator
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
    /// Default constructor for the MarketModel Class
    MarketModel() { ID = nextID++; model_type="Market"; commType=MarketComm; };
    
    /// MarketModels should not be indestructible.
    virtual ~MarketModel() {};
    
    // every model needs a method to initialize from XML
    virtual void init(xmlNodePtr cur);
    // every model needs a method to copy one object to another
    virtual void copy(MarketModel* src);

    // every model should be able to print a verbose description
    virtual void print();

protected:
    /// Stores next available market ID
    static int nextID;
/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
    /// default MarketModel receiver simply logs the offer/request
    virtual void receiveOfferRequest(OfferRequest* msg) 
         { messages.insert(msg); };

protected:


/* ------------------- */ 


/* --------------------
 * all MARKETMODEL classes have these members
 * --------------------
 */
public:
    /// every market should provide its commodity
    Commodity* getCommodity() { return commodity; } ;

    // Primary MarketModel methods

    /// Resolve requests with offers
    /**
     *  Primary funcation of a Market is to resolve the set of 
     *  requests with the set of offers.
     */
    virtual void resolve() = 0;

    /// Execute list of shipments
    /**
     *  Once market is resolved, go through the deque/queue and 
     *  execute the shipments.
     */
    virtual void executeOrderQueue();

protected: 
    /// every market has a commodity
    Commodity* commodity;

    /// every market collects offers & requests
    set<OfferRequest*> messages;

    /// every market generates a set of orders
    deque<Shipment*> orders;
    int firmOrders;
    

/* ------------------- */ 
    
};

#endif



