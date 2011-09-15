// Communicator.cpp
// Implements the Communicator class.
//
#include "FacilityModel.h"
#include "Communicator.h"
#include "GenException.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator::~Communicator() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Communicator::sendMessage() {
  std::string warn_msg = "Sender did not override sendMessage(msg)";
  throw GenException(warn_msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Communicator::sendMessage(Message* msg) {
  Communicator* recipient = msg->getRecipient();
  Communicator* sender = msg->getSender();

  if (recipient == this) {
    receiveMessage(msg);
  } else {
    switch(msg->getDir()) {
      case UP_MSG:
        switch(commType) {
          case FACILITY_COMM:
            msg->getInst()->receiveMessage(msg);
            break;
          case INST_COMM:
            msg->getReg()->receiveMessage(msg);
            break;
          case REGION_COMM:
            msg->getMkt()->receiveMessage(msg);
            break;
          case MARKET_COMM:
            receiveMessage(msg);
            break;
        }
        break;
      case DOWN_MSG:
        switch(commType) {
          case FACILITY_COMM:
            receiveMessage(msg);
            break;
          case INST_COMM:
            msg->getFac()->receiveMessage(msg);
            break;
          case REGION_COMM:
            msg->getInst()->receiveMessage(msg);
            break;
          case MARKET_COMM:
            msg->getReg()->receiveMessage(msg);
            break;
        }
        break;
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Communicator::receiveMessage(Message* msg) {
  std::string warn_msg = "Recipient did not override receiveMessage(msg)";
  throw GenException(warn_msg);
}
