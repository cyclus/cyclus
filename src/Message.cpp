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
Message::Message(MessageDir thisDir, Communicator* toSend)
{
  dir = thisDir;
  sender = toSend;

  trans.commod = NULL;
  recipient = NULL;

  trans.supplierID =
      trans.requesterID = NULL;

  trans.comp.insert(make_pair(0,0));

  trans.amount = 
    trans.min =
    trans.price = 0;

  mkt =
    reg =
    inst =
    fac = NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(MessageDir thisDir, Transaction thisTrans,
    Communicator* toSend, Communicator* toReceive)
{
  dir = thisDir;
  trans = thisTrans;
  sender = toSend;
  recipient = toReceive;
  Model* mktModel = trans.commod->getMarket();
  mkt = ((MarketModel*)(mktModel));
  setPath(dir, sender, recipient);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::printTrans(){
  std::cout << "Transaction info (via Message):" << std::endl <<
    "    Requester ID: " << trans.requesterID << std::endl <<
    "    Supplier ID: " << trans.supplierID << std::endl <<
    "    Price: "  << trans.price << std::endl;
};
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Commodity* thisCommod, CompMap thisComp, double thisAmount, double thisPrice, double minAmt,
		 Communicator* toSend, Communicator* toReceive)
{
  dir = none;
  trans.commod = thisCommod;
  trans.amount = thisAmount; 
  trans.min = minAmt;
  trans.price = thisPrice;
  trans.comp = thisComp;
  Model* mktModel = trans.commod->getMarket();
  mkt = ((MarketModel*)(mktModel));
  sender = toSend;
  recipient = toReceive;
  this->setSupplierID(((FacilityModel*)sender)->getSN());
  this->setRequesterID(((FacilityModel*)recipient)->getSN());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(MessageDir thisDir, Commodity* thisCommod, double thisAmount, double minAmt, 
    double thisPrice, Communicator* toSend, Communicator* toReceive, CompMap thisComp)
{
	dir = thisDir;
	trans.commod = thisCommod;
	trans.amount = thisAmount; 
  trans.min = minAmt;
	trans.price = thisPrice;
  trans.comp = thisComp;
  Model* mktModel = trans.commod->getMarket();
  mkt = ((MarketModel*)(mktModel));
  sender = toSend;
  recipient = toReceive;
  setPath(dir, sender, recipient);
  
  // if amt is positive and there is no supplier
  // this message is an offer and 
  // the sender is the supplier
  if (trans.amount > 0){
    this->setSupplierID(((FacilityModel*)sender)->getSN());
  }

  // if amt is negative and there is no requester
  // this message is a request and
  // the sender is the requester
  if (trans.amount < 0){
    this->setRequesterID(((FacilityModel*)sender)->getSN());
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(MessageDir thisDir, Commodity* thisCommod, double thisAmount, double minAmt, 
    double thisPrice, Communicator* toSend, Communicator* toReceive)
{
	dir = thisDir;
	trans.commod = thisCommod;
	trans.amount = thisAmount; 
  trans.min = minAmt;
	trans.price = thisPrice;

  CompMap thisComp;
  thisComp.insert(make_pair(NULL,NULL));

  Model* mktModel = trans.commod->getMarket();
  mkt = ((MarketModel*)(mktModel));
  sender = toSend;
  recipient = toReceive;
  setPath(dir, sender, recipient);
  
  // if amt is positive and there is no supplier
  // this message is an offer and 
  // the sender is the supplier
  if (trans.amount > 0){
    this->setSupplierID(((FacilityModel*)sender)->getSN());
  }

  // if amt is negative and there is no requester
  // this message is a request and
  // the sender is the requester
  if (trans.amount < 0){
    this->setRequesterID(((FacilityModel*)sender)->getSN());
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message* Message::clone() const
{
	return new Message(dir, trans.commod, trans.amount, trans.min, trans.price, 
      sender, recipient);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getSender() const {
	return sender;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getRecipient() const {
  return recipient;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Message::getSupplierID() const {
  return trans.supplierID;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Message::getRequesterID() const {
  return trans.requesterID;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MessageDir Message::getDir() const {
	return dir;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction Message::getTrans() const{
  return trans;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity* Message::getCommod() const {
	return trans.commod;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setCommod(Commodity* newCommod) 
{
	trans.commod = newCommod;
}
//- - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Message::getAmount() const {
	return trans.amount;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getFac() const {
	return fac;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getInst() const {
	return inst;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getReg() const {
	return reg;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getMkt() const {
	return mkt;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setDir(MessageDir newDir) 
{
	dir = newDir;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setAmount(double newAmount) 
{
	trans.amount = newAmount;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setSupplierID(int newID) 
{
	trans.supplierID = newID;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setRequesterID(int newID) 
{
	trans.requesterID = newID;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Message::getPrice() const {
	return trans.price;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CompMap Message::getComp() const {
	return trans.comp;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setComp(CompMap newComp) {
	trans.comp = newComp;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::reverseDirection()
{
	if (down == dir)
		dir = up; 
	else
		dir = down;
		// Speaking of "getting up" or "getting down," check out
		// "National Funk Congress Deadlocked On Get Up/Get Down Issue,"
		// The Onion (1999, October 27), 35(39).
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setPath(MessageDir dir, Communicator* sender, Communicator* recipient)
{
  if(dir==up)
  {
    switch (sender->getCommType())
    {
      case FacilityComm:
        fac = sender; 
        inst = ((FacilityModel*)(fac))->getFacInst();
        reg = ((InstModel*)(inst))->getRegion();
        break;
      case InstComm:
        fac = NULL;
        inst = sender;
        reg = ((InstModel*)(inst))->getRegion();
        break;
      case RegionComm:
        reg = sender;
        break;
      case MarketComm:
        throw GenException("A Market can't send a message *up* to anyone.");
      break;
    }
  }
  else if(dir==down)
  {
    switch (recipient->getCommType())
    {
      case FacilityComm:
        fac = recipient;
        inst = ((FacilityModel*)(fac))->getFacInst();
        reg = ((InstModel*)(inst))->getRegion();
        break;
      case InstComm:
        fac = NULL;
        inst = sender;
        reg = ((InstModel*)(inst))->getRegion();
        break;
      case RegionComm:
        reg = recipient;
        break;
      case MarketComm:
        throw GenException("No one can send a message *down* to a market.");
      break;
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Message::unEnumerateDir(){
	string toRet;
	if (up == dir)
		toRet = "up";
	else if (down == dir)
		toRet = "down";
	else
		throw GenException("Attempted to send a message neither up nor down.");

	return toRet;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::execute()
{
  FacilityModel* theFac = ((FacilityModel*)LI->getFacilityByID(trans.supplierID));
  CommunicatorType type;
  type = ((Communicator*)theFac)->getCommType();
  if (type == FacilityComm)
    (theFac)->receiveMessage(this);
  else
    throw GenException("Only FacilityModels can send material.");
} 
