// Message.cpp
// Implements the Message class.

#include "Message.h"

#include "CycException.h"
#include "Logician.h"
#include "Communicator.h"
#include "FacilityModel.h"
#include "MarketModel.h"
#include "InstModel.h"
#include "GenericResource.h"

#include <iostream>

// initialize static variables
int Message::nextID_ = 1;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Communicator* sender) {

  dir_ = UP_MSG;
  sender_ = sender;
  recipient_ = NULL;

  trans_.supplier = NULL;
  trans_.requester = NULL;
  trans_.resource = NULL;
  trans_.is_offer = NULL;
  trans_.minfrac = 0;
  trans_.price = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Communicator* sender, Communicator* receiver) {

  dir_ = UP_MSG;
  sender_ = sender;
  recipient_ = receiver;

  trans_.supplier = NULL;
  trans_.requester = NULL;
  trans_.resource = NULL;
  trans_.is_offer = NULL;
  trans_.minfrac = 0;
  trans_.price = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Communicator* sender, Communicator* receiver,
                 Transaction thisTrans) {

  dir_ = UP_MSG;
  trans_ = thisTrans;
  sender_ = sender;
  recipient_ = receiver;

  if (trans_.is_offer) {
    // if this message is an offer, the sender is the supplier
    setSupplier(dynamic_cast<Model*>(sender_));
  } else if (!trans_.is_offer) {
    // if this message is a request, the sender is the requester
    setRequester(dynamic_cast<Model*>(sender_));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::printTrans() {
  std::cout << "Transaction info (via Message):" << std::endl <<
    "    Transaction ID: " << trans_.ID << std::endl <<
    "    Requester ID: " << trans_.requester->ID() << std::endl <<
    "    Supplier ID: " << trans_.supplier->ID() << std::endl <<
    "    Price: "  << trans_.price << std::endl;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message* Message::clone() {
  Message* toRet = new Message(*this);
  switch( trans_.resource->getResourceType()){
    case MATERIAL_RES :
      toRet->setResource(new Material(*(dynamic_cast<Material*>(trans_.resource))));
      break;
    case GENERIC_RES :
      toRet->setResource(new GenericResource(*(dynamic_cast<GenericResource*>(trans_.resource))));
      break;
    default :
      CycException("ResourceType not recognized.");
  }
  // shouldnt we be returning the toRet message?
  return toRet;//new Message(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::sendOn() {
  validateForSend();

  if (dir_ == DOWN_MSG) {
    path_stack_.pop_back();
  }

  Communicator* next_stop = path_stack_.back();

  current_owner_ = next_stop;
  next_stop->receiveMessage(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::validateForSend() {
  int next_stop_index = -1;
  bool receiver_specified = false;
  Communicator* next_stop;

  if (dir_ == UP_MSG) {
    receiver_specified = (path_stack_.size() > 0);
    next_stop_index = path_stack_.size() - 1;
  } else if (dir_ == DOWN_MSG) {
    receiver_specified = (path_stack_.size() > 1);
    next_stop_index = path_stack_.size() - 2;
  }

  if (!receiver_specified) {
    string err_msg = "Can't send the message: next dest is unspecified.";
    throw CycMessageException(err_msg);
  }

  next_stop = path_stack_[next_stop_index];
  if (next_stop == current_owner_) {
    string err_msg = "Message receiver and sender are the same.";
    throw CycMessageException(err_msg);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setNextDest(Communicator* next_stop) {
  if (dir_ == UP_MSG) {
    if (path_stack_.size() == 0) {
      path_stack_.push_back(sender_);
    }

    path_stack_.push_back(next_stop);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::reverseDirection() {
  if (DOWN_MSG == dir_) {
    dir_ = UP_MSG; 
  } else {
  	dir_ = DOWN_MSG;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MessageDir Message::getDir() const {
  return dir_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setDir(MessageDir newDir) {
  dir_ = newDir;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getMarket() {
  MarketModel* market = MarketModel::marketForCommod(trans_.commod);
  return dynamic_cast<Communicator*>(market);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getRecipient() const {
  if (recipient_ == NULL) {
    string err_msg = "Uninitilized message recipient.";
    throw CycMessageException(err_msg);
  }

  return recipient_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Message::getSupplier() const {
  if (trans_.supplier == NULL) {
    string err_msg = "Uninitilized message supplier.";
    throw CycMessageException(err_msg);
  }

  return trans_.supplier;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Message::getRequester() const {
  if (trans_.requester == NULL) {
    string err_msg = "Uninitilized message requester.";
    throw CycMessageException(err_msg);
  }

  return trans_.requester;
}

