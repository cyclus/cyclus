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
    
    GreedyMarket(xmlNodePtr cur);
    
    ~GreedyMarket() {};
    
    virtual void print();
    
    /// get model implementation name
    virtual const string getModelName() { return "GreedyMarket"; };

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
extern "C" Model* construct(xmlNodePtr cur) {
    return new GreedyMarket(cur);
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
