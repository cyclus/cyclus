// Message.cpp
// Implements the Message class.

#include "Message.h"

#include "Communicator.h"
#include "Model.h"
#include "MarketModel.h"

#include "Resource.h"
#include "Logger.h"
#include "Timer.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::constructBase(Communicator* sender) {
  sender_ = sender;
  curr_owner_ = sender;
  receiver_ = NULL;
  trans_ = NULL;
  dead_ = false;
  dir_ = UP_MSG;

  sender->trackMessage(msg_ptr(this));
  makeRealParticipant(sender);

  MLOG(LEV_DEBUG4) << "Message " << this << " created.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Communicator* sender) {
  constructBase(sender);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Communicator* sender, Communicator* receiver) {
  constructBase(sender);
  receiver_ = receiver;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Communicator* sender, Communicator* receiver,
                 Transaction& trans) {
  constructBase(sender);
  receiver_ = receiver;
  trans_ = new Transaction(trans);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::~Message() {
  if (trans_ != NULL) {
    delete trans_;
  }
  MLOG(LEV_DEBUG4) << "Message " << this << " deleted.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
msg_ptr Message::clone() {
  CLOG(LEV_DEBUG3) << "Message " << this << "was cloned.";

  msg_ptr new_msg(new Message(*this));
  try {
    new_msg->trans_ = trans_->clone();
  } catch(CycNullMsgParamException) { }
  return new_msg;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::sendOn() {
  if (dead_) {return;}

  validateForSend();
  msg_ptr me = msg_ptr(this);

  if (dir_ == DOWN_MSG) {
    path_stack_.back()->untrackMessage(me);
    path_stack_.pop_back();
  } else {
    path_stack_.back()->trackMessage(me);
  }

  Communicator* next_stop = path_stack_.back();
  makeRealParticipant(next_stop);
  curr_owner_ = next_stop;

  CLOG(LEV_DEBUG1) << "Message " << this << " going to comm "
                   << next_stop << " {";

  next_stop->receiveMessage(me);

  CLOG(LEV_DEBUG1) << "} Message " << this << " send to comm "
                   << next_stop << " completed";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::validateForSend() {
  int next_stop_i = -1;
  bool has_receiver = false;
  Communicator* next_stop;

  if (dir_ == UP_MSG) {
    has_receiver = (path_stack_.size() > 0);
    next_stop_i = path_stack_.size() - 1;
  } else if (dir_ == DOWN_MSG) {
    has_receiver = (path_stack_.size() > 1);
    next_stop_i = path_stack_.size() - 2;
  }

  if (!has_receiver) {
    throw CycNoMsgReceiverException();
  }

  next_stop = path_stack_[next_stop_i];
  if (next_stop == curr_owner_) {
    throw CycCircularMsgException();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::makeRealParticipant(Communicator* who) {
  Model* model = NULL;
  model = dynamic_cast<Model*>(who);
  if (model != NULL) {model->setIsTemplate(false);}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setNextDest(Communicator* next_stop) {
  if (dir_ == UP_MSG) {
    CLOG(LEV_DEBUG4) << "Message " << this << " set next stop to comm "
                     << next_stop;
    if (path_stack_.size() == 0) {
      path_stack_.push_back(sender_);
    }
    path_stack_.push_back(next_stop);
    return;
  }
  CLOG(LEV_DEBUG4) << "Message " << this
                   << " next stop set attempt ignored";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::kill() {
  MLOG(LEV_DEBUG5) << "Message " << this << " was killed.";
  dead_ = true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Message::isDead() {
  return dead_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MessageDir Message::dir() const {
  return dir_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setDir(MessageDir new_dir) {
  CLOG(LEV_DEBUG4) << "Message " << this << " direction manually set to "
                   << new_dir << ".";

  dir_ = new_dir;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::sender() const {
  return sender_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::receiver() const {
  if (receiver_ == NULL) {
    string err_msg = "Uninitilized message receiver.";
    throw CycNullMsgParamException(err_msg);
  }
  return receiver_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction& Message::trans() const {
  if (trans_ == NULL) {
    throw CycNullMsgParamException("Uninitialized transaction parameter.");
  }
  return *trans_;
}

