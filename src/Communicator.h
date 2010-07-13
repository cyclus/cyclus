// Communicator.h
// An abstract class for deriving simulation entities that can communicate.

#if !defined(_COMMUNICATOR_H)
#define _COMMUNICATOR_H
#include <iostream>
#include <string>

#include "Message.h"
#include "Commodity.h"

using namespace std;

class Material;

/**
 * The types of Communicators (Market, Regions, Insts, and Facilities).
 */
enum CommunicatorType {StubComm, MarketComm, RegionComm, InstComm, FacilityComm};

/**
 * An abstract class for deriving simulation entities that can communicate.
 */
class Communicator {
    
protected:

    CommunicatorType commType;

public:
    
    CommunicatorType getCommType() { return commType; };

    /// default (trivial) sender is implemented
    virtual void sendOfferRequest();
    /// default (trivial) receiver is implemented
    virtual void receiveOfferRequest(OfferRequest* msg) = 0;
    /// default (trivial) transmitter is implemented
    virtual void transmitOfferRequest(OfferRequest* msg);

};
#endif
