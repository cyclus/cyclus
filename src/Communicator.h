// Communicator.h
#if !defined(_COMMUNICATOR_H)
#define _COMMUNICATOR_H
#include <string>

using namespace std;

#include "Message.h"

/// Enumerate the different types of Communicators
/**
 * The types of Communicators (Market, Regions, Insts, and Facilities).
 */
enum CommunicatorType {StubComm, MarketComm, RegionComm, InstComm, FacilityComm};

/**
 *  An abstract class for deriving simulation entities that can communicate.
 *
 *  This base class does not contain much information or define an extensive
 *  interface.  
 * 
 *  This class header explicitly includes full header for Message so that all
 *  Communicator derived classes automatically get this definition for their
 *  own use.
*/
class Communicator {
    
public:
    
    /// Get the type of this communicator
    CommunicatorType getCommType() { return commType; };

    /**
     *  @brief Default (trivial) sender is implemented
     *
     *  Generates a new, empty OfferRequest and sends it up/down the chain.
     */

    virtual void sendOfferRequest();

    /**
     *  @brief Default (trivial) receiver is implemented to ignore messages
     *
     *  @param msg OfferRequest message to be received
     */
    virtual void receiveOfferRequest(OfferRequest* msg){};

    /**
     *  @brief Default (trivial) transmitter is implemented
     *
     *  For upwards traveling OfferRequest messages, the message is
     *  transmitted untouched along the following path: 
     *    * facility -> institution -> region -> market 
     *
     *  For downwards traveling message, the opposite path is taken.
     *
     *  @param msg OfferRequest message to be transmitted
     *
     */
    virtual void transmitOfferRequest(OfferRequest* msg);

protected:

    /// Copy the base class data members from one object to another
    virtual void copy(Communicator* src) { commType = src->commType; };

    /// All communicators need to know what type they are
    CommunicatorType commType;


};
#endif
