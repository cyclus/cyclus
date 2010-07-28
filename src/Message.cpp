// Message.cpp
// Implements the Message class.

#include "Message.h"

#include "FacilityModel.h"
#include "Communicator.h"
#include "GenException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(MessageDir thisDir, Communicator* toSend)
{
  dir = thisDir;
  sender = toSend;

  trans.commod = NULL;
  recipient = 
    trans.supplier = 
    trans.requester = NULL;

  trans.amount = 
    trans.min =
    trans.price = 0;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(MessageDir thisDir, Transaction thisTrans,
    Communicator* toSend, Communicator* toReceive)
{
  dir = thisDir;
  trans = thisTrans;
  sender = toSend;
  recipient = toReceive;
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
  sender = toSend;
  recipient = toReceive;
  
  // if amt is positive and there is no supplier
  // this message is an offer and 
  // the sender is the supplier
  if (trans.amount > 0  && trans.supplier == NULL){
    trans.supplier = sender;
  }

  // if amt is negative and there is no requester
  // this message is a request and
  // the sender is the requester
  if (trans.amount < 0 && trans.requester == NULL){
    trans.requester = sender;
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
double Message::getAmount() const {
	return trans.amount;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setAmount(double newAmount) 
{
	trans.amount = newAmount;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setSupplier(Communicator* newSupplier) 
{
	trans.supplier = newSupplier;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setRequester(Communicator* newRequester) 
{
	trans.requester = newRequester;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Message::getPrice() const {
	return trans.price;
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
    if (trans.supplier->getCommType() == FacilityComm)
	((FacilityModel*)trans.supplier)->sendMaterial(trans,trans.requester);
    else
	throw GenException("Only FaciliyModels can send material.");
} 
