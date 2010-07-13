// Communicator.cpp
// Implements the Communicator class.

#include "Communicator.h"

/**
 * Default implementation of sendOfferRequest.
 *
 * Generates a new, empty OfferRequest and sends it up the chain.
 *
 */
void Communicator::sendOfferRequest()
{
    OfferRequest *msg = new OfferRequest(this);
    
    transmitOfferRequest(msg);
}

/**
 * Default implementation of receiveOfferRequest.
 *
 * ignores OfferRequest!
 *
 */

void Communicator::transmitOfferRequest(OfferRequest* msg)
{
    if (msg->getReceiver() == this)
	receiveOfferRequest(msg);
    else
	switch(msg->getDir())
	{
	    case up:
		switch(commType)
		{
		    case FacilityComm:
			msg->getInst()->transmitOfferRequest(msg);
			break;
		    case InstComm:
			msg->getReg()->transmitOfferRequest(msg);
			break;
		    case RegionComm:
			msg->getMkt()->receiveOfferRequest(msg);
			break;
		    case MarketComm:
			receiveOfferRequest(msg);
			break;
		}
		break;
	    case down:
		switch(commType)
		{
		    case FacilityComm:
			receiveOfferRequest(msg);
			break;
		    case InstComm:
			msg->getFac()->receiveOfferRequest(msg);
			break;
		    case RegionComm:
			msg->getInst()->transmitOfferRequest(msg);
			break;
		    case MarketComm:
			msg->getReg()->transmitOfferRequest(msg);
			break;
		}
		break;
	}

}
