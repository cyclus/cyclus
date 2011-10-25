// Message.cpp
// Implements the Message class.

#include "Message.h"

#include "CycException.h"
#include "Logician.h"
#include "Material.h"
#include "Communicator.h"
#include "FacilityModel.h"
#include "MarketModel.h"
#include "InstModel.h"

#include <iostream>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(MessageDir thisDir, Communicator* toSend) {
  dir_ = thisDir;
  sender_ = toSend;

  trans_.commod = NULL;
  recipient_ = NULL;

  trans_.supplier = NULL;
  trans_.requester = NULL;

  trans_.comp.insert(make_pair(0,0));

  trans_.amount = 0;
  trans_.min = 0;
  trans_.price = 0;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(MessageDir thisDir, Transaction thisTrans,
                 Communicator* toSend, Communicator* toReceive) {
  dir_ = thisDir;
  trans_ = thisTrans;
  sender_ = toSend;
  recipient_ = toReceive;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Commodity* thisCommod, CompMap thisComp, double thisAmount,                 double thisPrice, double minAmt, Communicator* toSend, 
                 Communicator* toReceive) {
  dir_ = NONE_MSG;
  trans_.commod = thisCommod;
  trans_.amount = thisAmount; 
  trans_.min = minAmt;
  trans_.price = thisPrice;
  trans_.comp = thisComp;
  sender_ = toSend;
  recipient_ = toReceive;
  this->setSupplier(dynamic_cast<Model*>(sender_));
  this->setRequester(dynamic_cast<Model*>(recipient_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(MessageDir thisDir, Commodity* thisCommod, double thisAmount, 
                 double minAmt, double thisPrice, Communicator* toSend, 
                 Communicator* toReceive, CompMap thisComp) {
  dir_ = thisDir;
  trans_.commod = thisCommod;
  trans_.amount = thisAmount; 
  trans_.min = minAmt;
  trans_.price = thisPrice;
  trans_.comp = thisComp;
  sender_ = toSend;
  recipient_ = toReceive;
  
  // if amt is positive and there is no supplier
  // this message is an offer and 
  // the sender is the supplier
  if (trans_.amount > 0) {
    this->setSupplier(dynamic_cast<Model*>(sender_));
  }

  // if amt is negative and there is no requester
  // this message is a request and
  // the sender is the requester
  if (trans_.amount < 0){
    this->setRequester(dynamic_cast<Model*>(sender_));
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(MessageDir thisDir, Commodity* thisCommod, double thisAmount, 
                 double minAmt, double thisPrice, Communicator* toSend, 
                 Communicator* toReceive) {
  dir_ = thisDir;
  trans_.commod = thisCommod;
  trans_.amount = thisAmount; 
  trans_.min = minAmt;
  trans_.price = thisPrice;

  CompMap thisComp;
  thisComp.insert(make_pair(NULL, NULL));

  sender_ = toSend;
  recipient_ = toReceive;
  
  // if amt is positive and there is no supplier
  // this message is an offer and 
  // the sender is the supplier
  if (trans_.amount > 0) {
    this->setSupplier(dynamic_cast<Model*>(sender_));
  }

  // if amt is negative and there is no requester
  // this message is a request and
  // the sender is the requester
  if (trans_.amount < 0) {
    this->setRequester(dynamic_cast<Model*>(sender_));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::printTrans() {
  std::cout << "Transaction info (via Message):" << std::endl <<
    "    Requester ID: " << trans_.requester->getSN() << std::endl <<
    "    Supplier ID: " << trans_.supplier->getSN() << std::endl <<
    "    Price: "  << trans_.price << std::endl;
};
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message* Message::clone() const {
	return new Message(dir_, trans_.commod, trans_.amount, trans_.min, 
                       trans_.price, sender_, recipient_);
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
Communicator* Message::getMarket() {
  return dynamic_cast<Communicator*>(trans_.commod->getMarket());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getSender() const {
	return sender_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getRecipient() const {
  if (recipient_ == NULL) {
    string err_msg = "Uninitilized message recipient.";
    throw CycNullException(err_msg);
  }

  return recipient_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Message::getSupplier() const {
  if (trans_.supplier == NULL) {
    string err_msg = "Uninitilized message supplier.";
    throw CycNullException(err_msg);
  }

  return trans_.supplier;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Message::getRequester() const {
  if (trans_.requester == NULL) {
    string err_msg = "Uninitilized message requester.";
    throw CycNullException(err_msg);
  }

  return trans_.requester;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MessageDir Message::getDir() const {
  return dir_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction Message::getTrans() const{
  return trans_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity* Message::getCommod() const {
  return trans_.commod;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setCommod(Commodity* newCommod) {
  trans_.commod = newCommod;
}
//- - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Message::getAmount() const {
  return trans_.amount;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setDir(MessageDir newDir) {
  dir_ = newDir;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setAmount(double newAmount) {
  trans_.amount = newAmount;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setSupplier(Model* supplier) {
  trans_.supplier = supplier;
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
CompMap Message::getComp() const {
  return trans_.comp;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setComp(CompMap newComp) {
  trans_.comp = newComp;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::reverseDirection() {
  if (DOWN_MSG == dir_) {
    dir_ = UP_MSG; 
  } else {
  	dir_ = DOWN_MSG;
    // Speaking of "getting up" or "getting down," check out
    // "National Funk Congress Deadlocked On Get Up/Get Down Issue,"
    // The Onion (1999, October 27), 35(39).
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::execute() {
  Communicator* theFac = dynamic_cast<Communicator*>(trans_.supplier);
  theFac->receiveMessage(this);
} 

