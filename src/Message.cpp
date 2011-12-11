// Message.cpp
// Implements the Message class.

#include "Message.h"

#include "CycException.h"
#include "Logician.h"
#include "Communicator.h"
#include "FacilityModel.h"
#include "MarketModel.h"
#include "InstModel.h"

#include <iostream>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Communicator* sender) {
  dir_ = UP_MSG;
  sender_ = sender;
  recipient_ = NULL;

  trans_.supplier = NULL;
  trans_.requester = NULL;
  trans_.resource = NULL;
  trans_.amount = 0;
  trans_.min = 0;
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
  trans_.amount = 0;
  trans_.min = 0;
  trans_.price = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Communicator* sender, Communicator* receiver,
                 Transaction thisTrans) {
  dir_ = UP_MSG;
  trans_ = thisTrans;
  sender_ = sender;
  recipient_ = receiver;

  // if amt is positive this message is an offer and 
  // the sender is the supplier
  if (trans_.amount > 0) {
    setSupplier(dynamic_cast<Model*>(sender_));
  }

  // if amt is negative this message is a request and
  // the sender is the requester
  if (trans_.amount < 0){
    setRequester(dynamic_cast<Model*>(sender_));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::printTrans() {
  std::cout << "Transaction info (via Message):" << std::endl <<
    "    Requester ID: " << trans_.requester->ID() << std::endl <<
    "    Supplier ID: " << trans_.supplier->ID() << std::endl <<
    "    Price: "  << trans_.price << std::endl;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message* Message::clone() {
  return new Message(*this);
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
Communicator* Message::getSender() const {
	return sender_;
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
void Message::setSupplier(Model* supplier) {
  trans_.supplier = supplier;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Message::getRequester() const {
  if (trans_.requester == NULL) {
    string err_msg = "Uninitilized message requester.";
    throw CycMessageException(err_msg);
  }

  return trans_.requester;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction Message::getTrans() const{
  return trans_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Message::commod() const {
  return trans_.commod;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setCommod(std::string newCommod) {
  trans_.commod = newCommod;
}

//- - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Message::getAmount() const {
  return trans_.amount;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setAmount(double newAmount) {
  trans_.amount = newAmount;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setRequester(Model* requester) {
  trans_.requester = requester;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Message::getPrice() const {
  return trans_.price;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource* Message::getResource() const {
  return trans_.resource;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setResource(Resource* newResource) {
  trans_.resource= newResource;
}

