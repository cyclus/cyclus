// Message.cpp
// Implements the Message class.

#include "Message.h"

#include "Communicator.h"
#include "error.h"
#include "Model.h"
#include "MarketModel.h"
#include "Resource.h"
#include "Logger.h"
#include "Timer.h"

namespace cyclus {

std::map<std::string, std::map<int, double> > Message::offer_qtys_;
std::map<std::string, std::map<int, double> > Message::request_qtys_;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::ConstructBase(Communicator* sender) {
  sender_ = sender;
  curr_owner_ = sender;
  receiver_ = NULL;
  trans_ = NULL;
  dead_ = false;
  dir_ = UP_MSG;

  path_stack_.push_back(sender_);
  sender->TrackMessage(msg_ptr(this));

  MLOG(LEV_DEBUG4) << "Message " << this << " created.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Communicator* sender) {
  ConstructBase(sender);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Communicator* sender, Communicator* receiver) {
  ConstructBase(sender);
  receiver_ = receiver;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Communicator* sender, Communicator* receiver,
                 Transaction& trans) {
  ConstructBase(sender);
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
  new_msg->trans_ = trans_->clone();
  return new_msg;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::SendOn() {
  if (dead_) {return;}
  msg_ptr me = msg_ptr(this);

  if (dir_ == DOWN_MSG) {
    path_stack_.back()->UntrackMessage(me);
    path_stack_.pop_back();
  } else if (dir_ == UP_MSG) {
    AutoSetNextDest();
    path_stack_.back()->TrackMessage(me);
  } else {
    return;
  }

  ValidateForSend();

  Communicator* next_stop = path_stack_.back();
  curr_owner_ = next_stop;

  CLOG(LEV_DEBUG1) << "Message " << this << " going to comm "
                   << next_stop << " {";

  next_stop->ReceiveMessage(me);

  CLOG(LEV_DEBUG1) << "} Message " << this << " send to comm "
                   << next_stop << " completed";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::AutoSetNextDest() {
  if (path_stack_.back() != curr_owner_) {
    return;
  }

  Model* curr = dynamic_cast<Model*>(curr_owner_);
  if (curr == NULL) {
    // cannot set auto-set (e.g. curr_owner_ is not a Model*
    return;
  }

  Communicator* next_dest;
  try {
    Model* next_model = curr->parent();
    TallyOrder(next_model);
    next_dest = dynamic_cast<Communicator*>(next_model);
  } catch (ValueError err) {
    next_dest = receiver_;
  }
  SetNextDest(next_dest);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::TallyOrder(Model* next_model) {
  if (next_model->ModelType() != "Market") {
    return;
  }

  Transaction tran = trans();
  if (tran.IsOffer()) {
    Message::offer_qtys_[tran.commod()][TI->time()] += tran.Resource()->quantity();
  } else {
    Message::request_qtys_[tran.commod()][TI->time()] += tran.Resource()->quantity();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::ValidateForSend() {
  int next_stop_i = path_stack_.size() - 1;
  bool has_receiver = (path_stack_.size() > 0);

  if (!has_receiver) {
    throw Error("Can't send the message - must call setNextDest first");
  }

  Communicator* next_stop = path_stack_[next_stop_i];

  if (next_stop == curr_owner_) {
    throw Error("Message receiver and sender are the same.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::SetNextDest(Communicator* next_stop) {
  if (dir_ == DOWN_MSG) {
    CLOG(LEV_DEBUG4) << "Message " << this
                     << " next stop set attempt ignored";
    return;
  }

  path_stack_.push_back(next_stop);

  CLOG(LEV_DEBUG4) << "Message " << this << " set next stop to comm "
                   << next_stop;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::Kill() {
  MLOG(LEV_DEBUG5) << "Message " << this << " was killed.";
  dead_ = true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Message::IsDead() {
  return dead_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MessageDir Message::Dir() const {
  return dir_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::SetDir(MessageDir new_dir) {
  CLOG(LEV_DEBUG4) << "Message " << this << " direction manually set to "
                   << new_dir << ".";

  dir_ = new_dir;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Message::notes() {
  return notes_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::SetNotes(std::string text) {
  notes_ = text;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::sender() const {
  return sender_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::receiver() const {
  return receiver_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction& Message::trans() const {
  if (!trans_) {
    throw Error("message has no transactionn payload");
  }
  return *trans_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Message::UnmetDemand(std::string commod, int time) {
  double supply = Message::offer_qtys_[commod][time];
  double demand = Message::request_qtys_[commod][time];
  return demand - supply;
}

} // namespace cyclus
