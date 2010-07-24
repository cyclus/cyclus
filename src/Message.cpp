// Message.cpp
// Implements the Message classes.

#include "Message.h"

#include "Communicator.h"
#include "FacilityModel.h"

#include "GenException.h"

OfferRequest::OfferRequest(Communicator* sender)
{

  msgPath.sndr = sender;

  msgPath.rcvr 
      = msgPath.mkt 
      = msgPath.reg 
      = msgPath.inst 
      = msgPath.fac = NULL;

  switch (sender->getCommType())
  {
    case FacilityComm:
      msgPath.fac = sender;
      break;
    case InstComm:
      msgPath.inst = sender;
      break;
    case RegionComm:
      msgPath.reg = sender;
      break;
    case MarketComm:
      msgPath.mkt = sender;
      break;
  }
}
  
void Shipment::execute()
{
  if (shipPath.shipper->getCommType() == FacilityComm)
    ((FacilityModel*)shipPath.shipper)->sendMaterial(trans,shipPath.receiver);
  else
    throw GenException("Only FaciliyModels can send material.");
}
