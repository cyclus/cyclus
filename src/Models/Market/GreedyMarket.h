// GreedyMarket.h
#if !defined(_GREEDYMARKET_H)
#define _GREEDYMARKET_H
#include <iostream>
#include <map>
#include <deque>

#include "MarketModel.h"

/**
 * The GreedyMarket class inherits from the MarketModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This market will take a set of requests and match the biggest requests with
 * the biggest offers first.
 *
 */
class GreedyMarket : public MarketModel  
{
/* --------------------
 * all MODEL classes have these public members
 * --------------------
 */

public:
    GreedyMarket() {};
    
    ~GreedyMarket() {};
    
    // different ways to populate an object after creation
    /// initialize an object from XML input
    virtual void init(xmlNodePtr cur)    { MarketModel::init(cur); };
    /// initialize an object by copying another
    virtual void copy(GreedyMarket* src) { MarketModel::copy(src); } ;

    virtual void print()                 { MarketModel::print();   } ;

/* -------------------- */

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */

    virtual void receiveOfferRequest(OfferRequest* msg);
/* -------------------- */


/* --------------------
 * all MARKETMODEL classes have these members
 * --------------------
 */

    virtual void resolve();

/* -------------------- */

/* --------------------
 * _THIS_ MARKETMODEL class these members
 * --------------------
 */

/* -------------------- */

private:
    typedef pair<double,OfferRequest*> indexedMsg;
    typedef multimap<double,OfferRequest*> sortedMsgList;
    
    sortedMsgList requests, offers;
    set<OfferRequest*> matchedOffers;
    
    void add(OfferRequest* msg);

    bool match_request(sortedMsgList::iterator request);
    void reject_request(sortedMsgList::iterator request);
    void process_request();

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
extern "C" Model* construct() {
    return new GreedyMarket();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
