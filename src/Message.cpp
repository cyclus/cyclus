// Message.cpp
// Implements the Message class.

#include "Message.h"

#include "Logician.h"
#include "Material.h"
#include "FacilityModel.h"
#include "MarketModel.h"
#include "InstModel.h"
#include "Communicator.h"
#include "GenException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(MessageDir thisDir, Communicator* toSend) {
  dir_ = thisDir;
  sender_ = toSend;

  trans_.commod = NULL;
  recipient_ = NULL;

  trans_.supplierID = NULL;
  trans_.requesterID = NULL;

  trans_.comp.insert(make_pair(0,0));

  trans_.amount = 0;
  trans_.min = 0;
  trans_.price = 0;

  mkt_ = NULL;
  reg_ = NULL;
  inst_ = NULL;
  fac_ = NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(MessageDir thisDir, Transaction thisTrans,
                 Communicator* toSend, Communicator* toReceive) {
  dir_ = thisDir;
  trans_ = thisTrans;
  sender_ = toSend;
  recipient_ = toReceive;
  Model* mktModel = trans_.commod->getMarket();
  mkt_ = ((MarketModel*)(mktModel));
  setPath();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Commodity* thisCommod, CompMap thisComp, double thisAmount,                 double thisPrice, double minAmt, Communicator* toSend, 
                 Communicator* toReceive) {
  dir_ = none;
  trans_.commod = thisCommod;
  trans_.amount = thisAmount; 
  trans_.min = minAmt;
  trans_.price = thisPrice;
  trans_.comp = thisComp;
  Model* mktModel = trans_.commod->getMarket();
  mkt_ = ((MarketModel*)(mktModel));
  sender_ = toSend;
  recipient_ = toReceive;
  this->setSupplierID(((FacilityModel*)sender_)->getSN());
  this->setRequesterID(((FacilityModel*)recipient_)->getSN());
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
  Model* mktModel = trans_.commod->getMarket();
  mkt_ = ((MarketModel*)(mktModel));
  sender_ = toSend;
  recipient_ = toReceive;
  setPath();
  
  // if amt is positive and there is no supplier
  // this message is an offer and 
  // the sender is the supplier
  if (trans_.amount > 0){
    this->setSupplierID(((FacilityModel*)sender_)->getSN());
  }

  // if amt is negative and there is no requester
  // this message is a request and
  // the sender is the requester
  if (trans_.amount < 0){
    this->setRequesterID(((FacilityModel*)sender_)->getSN());
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
  thisComp.insert(make_pair(NULL,NULL));

  Model* mktModel = trans_.commod->getMarket();
  mkt_ = ((MarketModel*)(mktModel));
  sender_ = toSend;
  recipient_ = toReceive;
  setPath();
  
  // if amt is positive and there is no supplier
  // this message is an offer and 
  // the sender is the supplier
  if (trans_.amount > 0){
    this->setSupplierID(((FacilityModel*)sender_)->getSN());
  }

  // if amt is negative and there is no requester
  // this message is a request and
  // the sender is the requester
  if (trans_.amount < 0){
    this->setRequesterID(((FacilityModel*)sender_)->getSN());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::printTrans() {
  std::cout << "Transaction info (via Message):" << std::endl <<
    "    Requester ID: " << trans_.requesterID << std::endl <<
    "    Supplier ID: " << trans_.supplierID << std::endl <<
    "    Price: "  << trans_.price << std::endl;
};
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message* Message::clone() const
{
	return new Message(dir_, trans_.commod, trans_.amount, trans_.min, 
                       trans_.price, sender_, recipient_);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getSender() const {
	return sender_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getRecipient() const {
  return recipient_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Message::getSupplierID() const {
  return trans_.supplierID;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Message::getRequesterID() const {
  return trans_.requesterID;
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
Communicator* Message::getFac() const {
  return fac_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getInst() const {
  return inst_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getReg() const {
  return reg_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getMkt() const {
  return mkt_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setDir(MessageDir newDir) {
  dir_ = newDir;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setAmount(double newAmount)  {
  trans_.amount = newAmount;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setSupplierID(int newID) {
  trans_.supplierID = newID;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setRequesterID(int newID) {
  trans_.requesterID = newID;
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
  if (down == dir_) {
    dir_ = up; 
  } else {
  	dir_ = down;
    // Speaking of "getting up" or "getting down," check out
    // "National Funk Congress Deadlocked On Get Up/Get Down Issue,"
    // The Onion (1999, October 27), 35(39).
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setPath() {
  if(dir_==up) {
    switch (sender_->getCommType())
    {
      case FacilityComm:
        fac_ = sender_; 
        inst_ = ((FacilityModel*)(fac_))->getFacInst();
        reg_ = ((InstModel*)(inst_))->getRegion();
        break;
      case InstComm:
        fac_ = NULL;
        inst_ = sender_;
        reg_ = ((InstModel*)(inst_))->getRegion();
        break;
      case RegionComm:
        reg_ = sender_;
        break;
      case MarketComm:
        throw GenException("A Market can't send a message *up* to anyone.");
      break;
    }
  } else if(dir_==down) {
    switch (recipient_->getCommType()) {
      case FacilityComm:
        fac_ = recipient_;
        inst_ = ((FacilityModel*)(fac_))->getFacInst();
        reg_ = ((InstModel*)(inst_))->getRegion();
        break;
      case InstComm:
        fac_ = NULL;
        inst_ = sender_;
        reg_ = ((InstModel*)(inst_))->getRegion();
        break;
      case RegionComm:
        reg_ = recipient_;
        break;
      case MarketComm:
        throw GenException("No one can send a message *down* to a market.");
      break;
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Message::unEnumerateDir() {
  string toRet;
  if (up == dir_)
    toRet = "up";
  else if (down == dir_)
    toRet = "down";
  else
    throw GenException("Attempted to send a message neither up nor down.");

  return toRet;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::execute() {
  FacilityModel* theFac;
  theFac = ((FacilityModel*)LI->getModelByID(trans_.supplierID, FACILITY));
  CommunicatorType type;
  type = ((Communicator*)theFac)->getCommType();

  if (type == FacilityComm)
    (theFac)->receiveMessage(this);
  else
    throw GenException("Only FacilityModels can send material.");
} 

