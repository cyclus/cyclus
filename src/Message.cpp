// Message.cpp
// Implements the Message class.

#include "Message.h"

#include "CycException.h"
#include "Communicator.h"
#include "FacilityModel.h"
#include "MarketModel.h"
#include "InstModel.h"
#include "GenericResource.h"
#include "Logger.h"
#include "BookKeeper.h"

#include <iostream>

// initialize static variables
int Message::nextTransID_ = 1;

std::string Message::outputDir_ = "/output/transactions";

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Communicator* sender) {
  dir_ = UP_MSG;
  sender_ = sender;
  recipient_ = NULL;
  current_owner_ = NULL;
  path_stack_ = vector<Communicator*>();
  current_owner_ = sender;

  trans_.supplier = NULL;
  trans_.requester = NULL;
  trans_.is_offer = NULL;
  trans_.resource = NULL;
  trans_.minfrac = 0;
  trans_.price = 0;

  setRealParticipant(sender);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Communicator* sender, Communicator* receiver) {
  dir_ = UP_MSG;
  sender_ = sender;
  recipient_ = receiver;
  current_owner_ = NULL;

  trans_.supplier = NULL;
  trans_.requester = NULL;
  trans_.is_offer = NULL;
  trans_.resource = NULL;
  trans_.minfrac = 0;
  trans_.price = 0;

  setRealParticipant(sender);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Communicator* sender, Communicator* receiver,
                 Transaction thisTrans) {
  dir_ = UP_MSG;
  trans_ = thisTrans;
  sender_ = sender;
  recipient_ = receiver;
  current_owner_ = NULL;
  setResource(thisTrans.resource);

  if (trans_.is_offer) {
    // if this message is an offer, the sender is the supplier
    setSupplier(dynamic_cast<Model*>(sender_));
  } else if (!trans_.is_offer) {
    // if this message is a request, the sender is the requester
    setRequester(dynamic_cast<Model*>(sender_));
  }

  setRealParticipant(sender);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setRealParticipant(Communicator* who) {
  Model* model = NULL;
  model = dynamic_cast<Model*>(who);
  if (model != NULL) {model->setIsTemplate(false);}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::printTrans() {
  std::cout << "Transaction info (via Message):" << std::endl <<
    "    Requester ID: " << trans_.requester->ID() << std::endl <<
    "    Supplier ID: " << trans_.supplier->ID() << std::endl <<
    "    Price: "  << trans_.price << std::endl;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
msg_ptr Message::clone() {
  msg_ptr new_msg(new Message(*this));
  new_msg->setResource(resource());
  return new_msg;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::sendOn() {
  validateForSend();

  if (dir_ == DOWN_MSG) {
    path_stack_.pop_back();
  }

  Communicator* next_stop = path_stack_.back();

  setRealParticipant(next_stop);

  current_owner_ = next_stop;

  msg_ptr me = msg_ptr(this);

  LOG(LEV_DEBUG3) << "MemAlloc: Message " << me.get() << " going to " << " ID=" << dynamic_cast<Model*>(next_stop)->ID();
  next_stop->receiveMessage(me);
  LOG(LEV_DEBUG3) << "MemAlloc: Message " << me.get() << " returned from " << " ID=" << dynamic_cast<Model*>(next_stop)->ID();
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
MessageDir Message::dir() const {
  return dir_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setDir(MessageDir newDir) {
  dir_ = newDir;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::market() {
  MarketModel* market = MarketModel::marketForCommod(trans_.commod);
  return market;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::recipient() const {
  if (recipient_ == NULL) {
    string err_msg = "Uninitilized message recipient.";
    throw CycMessageException(err_msg);
  }

  return recipient_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Message::supplier() const {
  if (trans_.supplier == NULL) {
    string err_msg = "Uninitilized message supplier.";
    throw CycMessageException(err_msg);
  }

  return trans_.supplier;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Message::requester() const {
  if (trans_.requester == NULL) {
    string err_msg = "Uninitilized message requester.";
    throw CycMessageException(err_msg);
  }

  return trans_.requester;
}

void Message::approveTransfer() {
  msg_ptr me = msg_ptr(this);

  Model* req = requester();
  Model* sup = supplier();
  vector<Resource*> manifest = sup->removeResource(me);
  req->addResource(me, manifest);

  BI->registerTransaction(nextTransID_++, me, manifest);

  LOG(LEV_DEBUG2) << "Material sent from " << sup->ID() << " to " 
                  << req->ID() << ".";
}

