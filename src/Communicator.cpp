// Communicator.cpp
// Implements the Communicator class.
//
#include "FacilityModel.h"
#include "Communicator.h"
#include "GenException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator::~Communicator()
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Communicator::sendMessage()
{
  throw GenException
    ("Sender did not override sendMessage()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Communicator::sendMessage(Message* msg)
{
  Communicator* recipient = msg->getRecipient();
  Communicator* sender = msg->getSender();

  if (recipient == this)
    receiveMessage(msg);
  else 
    switch(msg->getDir())
    {
      case up:
        switch(commType)
        {
          case FacilityComm:
            msg->getInst()->receiveMessage(msg);
            break;
          case InstComm:
            msg->getReg()->receiveMessage(msg);
            break;
          case RegionComm:
            msg->getMkt()->receiveMessage(msg);
            break;
          case MarketComm:
            receiveMessage(msg);
            break;
        }
        break;
      case down:
        switch(commType)
        {
          case FacilityComm:
            receiveMessage(msg);
            break;
          case InstComm:
            msg->getFac()->receiveMessage(msg);
            break;
          case RegionComm:
            msg->getInst()->receiveMessage(msg);
            break;
          case MarketComm:
            msg->getReg()->receiveMessage(msg);
            break;
        }
        break;
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Communicator::receiveMessage(Message* msg)
{
  throw GenException
    ("Recipient did not override receiveMessage(msg)");
}
