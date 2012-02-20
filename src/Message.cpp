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
#include "Timer.h"

#include <iostream>

// initialize static variables
int Message::nextTransID_ = 1;
// Database table for transactions
Table *Message::trans_table = new Table("Transactions"); 
Table *Message::trans_resource_table = new Table("TransactedResources"); 


std::string Message::outputDir_ = "/output/transactions";

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(Communicator* sender) {
  MLOG(LEV_DEBUG4) << "Message " << this << " created.";
  dead_ = false;
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
  MLOG(LEV_DEBUG4) << "Message " << this << " created.";
  dead_ = false;
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
  MLOG(LEV_DEBUG4) << "Message " << this << " created.";
  dead_ = false;
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
Message::~Message() {
  MLOG(LEV_DEBUG4) << "Message " << this << " deleted.";
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setRealParticipant(Communicator* who) {
  Model* model = NULL;
  model = dynamic_cast<Model*>(who);
  if (model != NULL) {model->setIsTemplate(false);}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::printTrans() {
  CLOG(LEV_INFO4) << "Transaction info {"
                  << ", Requester ID=" << trans_.requester->ID()
                  << ", Supplier ID=" << trans_.supplier->ID()
                  << ", Price="  << trans_.price << " }";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
msg_ptr Message::clone() {
  CLOG(LEV_DEBUG3) << "Message " << this << "was cloned.";

  msg_ptr new_msg(new Message(*this));
  new_msg->setResource(resource());
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
  setRealParticipant(next_stop);
  current_owner_ = next_stop;

  CLOG(LEV_DEBUG1) << "Message " << this << " going to model"
                   << " ID=" << dynamic_cast<Model*>(next_stop)->ID() << " {";

  next_stop->receiveMessage(me);

  CLOG(LEV_DEBUG1) << "} Message " << this << " returned from model"
                   << " ID=" << dynamic_cast<Model*>(next_stop)->ID();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::kill() {
  MLOG(LEV_DEBUG5) << "Message " << this << " was killed.";
  dead_ = true;
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
    CLOG(LEV_DEBUG4) << "Message " << this << " next-stop set to model ID="
                     << dynamic_cast<Model*>(next_stop)->ID();
    if (path_stack_.size() == 0) {
      path_stack_.push_back(sender_);
    }
    path_stack_.push_back(next_stop);
    return;
  }
  CLOG(LEV_DEBUG4) << "Message " << this
                   << " next-stop set attempt ignored to model ID="
                   << dynamic_cast<Model*>(next_stop)->ID();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::reverseDirection() {
  if (DOWN_MSG == dir_) {
    CLOG(LEV_DEBUG4) << "Message " << this << "direction flipped from 'down' to 'up'.";
    dir_ = UP_MSG; 
  } else {
    CLOG(LEV_DEBUG4) << "Message " << this << "direction flipped from 'up' to 'down'.";
  	dir_ = DOWN_MSG;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MessageDir Message::dir() const {
  return dir_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setDir(MessageDir newDir) {
  CLOG(LEV_DEBUG4) << "Message " << this << "manually changed to "
                   << newDir << ".";

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
  vector<rsrc_ptr> manifest = sup->removeResource(me);
  req->addResource(me, manifest);

  int id = nextTransID_++;

  BI->registerTransaction(id, me, manifest);
  
  // register that this transaction occured
  this->Message::addTransToTable(id);
  int nResources = manifest.size();
  for (int pos = 0; pos < nResources; pos++){
    this->Message::addResourceToTable(id,pos+1,manifest.at(pos));
  }

  CLOG(LEV_INFO3) << "Material sent from " << sup->ID() << " to " 
                  << req->ID() << ".";

  CLOG(LEV_INFO4) << "Material transfer details {";
  printTrans();
  for (int i = 0; i < manifest.size(); i++) {
    manifest.at(i)->print();
  }
  CLOG(LEV_INFO4) << "}";

}

void Message::define_trans_table(){
  // declare the table columns
  column id("ID","INTEGER");
  column sender("SenderID","INTEGER");
  column receiver("ReceiverID","INTEGER");
  column time("Time","INTEGER");
  column price("Price","REAL");
  // declare the table's primary key
  primary_key pk;
  pk.push_back("ID");
  trans_table->setPrimaryKey(pk);
  // add columns to the table
  trans_table->addColumn(id), trans_table->addColumn(sender), 
    trans_table->addColumn(receiver), trans_table->addColumn(time),
    trans_table->addColumn(price);
  // add foreign keys
  foreign_key_ref *fkref;
  foreign_key *fk;
  key myk, theirk;
  //   Agents table foreign keys
  theirk.push_back("ID");
  fkref = new foreign_key_ref("Agents",theirk);
  //     the sender id
  myk.push_back("SenderID");
  fk = new foreign_key(myk, (*fkref) );
  trans_table->addForeignKey( (*fk) ); // sender id references agents' id
  myk.clear();
  //     the reciever id
  myk.push_back("ReceiverID");
  fk = new foreign_key(myk, (*fkref) );
  trans_table->addForeignKey( (*fk) ); // receiver id references agents' id
  myk.clear();
  theirk.clear();
  // we've now defined the table
  trans_table->tableDefined();
}

void Message::addTransToTable(int transID){  
  // if we haven't logged a message yet, define the table
  if ( !trans_table->defined() )
    Message::define_trans_table();
  
  // make a row
  // declare data
  data an_id(transID), a_sender( trans_.supplier->ID() ), 
    a_receiver( trans_.requester->ID() ), a_time( TI->time() ), 
    a_price( trans_.price );
  // declare entries
  entry id("ID",an_id), sender("SenderID",a_sender), 
    receiver("ReceiverID",a_receiver), time("Time",a_time), 
    price("Price",a_price);
  // declare row
  row aRow;
  aRow.push_back(id), aRow.push_back(sender), aRow.push_back(receiver), 
    aRow.push_back(time),aRow.push_back(price);
  // add the row
  trans_table->addRow(aRow);
  // record this primary key
  pkref_trans_.push_back(id);
}

void Message::define_trans_resource_table(){
  // declare the table columns
  column transID("TransactionID","INTEGER");
  column transPos("Position","INTEGER");
  column resource("ResourceID","INTEGER");
  column amt("Quantity","REAL");
  // declare the table's primary key
  primary_key pk;
  pk.push_back("TransactionID"), pk.push_back("Position");
  trans_resource_table->setPrimaryKey(pk);
  // add columns to the table
  trans_resource_table->addColumn(transID), trans_resource_table->addColumn(transPos), 
    trans_resource_table->addColumn(resource), trans_resource_table->addColumn(amt);
  // add foreign keys
  foreign_key_ref *fkref;
  foreign_key *fk;
  key myk, theirk;
  //   Transactions table foreign keys
  theirk.push_back("ID");
  fkref = new foreign_key_ref("Transactions",theirk);
  //     the transaction id
  myk.push_back("TransactionID");
  fk = new foreign_key(myk, (*fkref) );
  trans_resource_table->addForeignKey( (*fk) ); // transid references transaction's id
  myk.clear(), theirk.clear();
  //    Resource table foreign keys
  theirk.push_back("ID");
  fkref = new foreign_key_ref("Resources",theirk);
  //      the resource id
  myk.push_back("ResourceID");
  fk = new foreign_key(myk, (*fkref) );
  trans_resource_table->addForeignKey( (*fk) ); // resourceid references resource's id
  // we've now defined the table
  trans_resource_table->tableDefined();
}

void Message::addResourceToTable(int transID, int transPos, rsrc_ptr r){  
  // if we haven't logged a message yet, define the table
  if ( !trans_resource_table->defined() )
    Message::define_trans_resource_table();
  
  // make a row
  // declare data
  data an_id(transID), a_pos(transPos), 
    a_resource(r->originalID()), an_amt(r->quantity());
  // declare entries
  entry id("TransactionID",an_id), pos("Position",a_pos), 
    resource("ResourceID",a_resource), amt("Quantity",an_amt);
  // declare row
  row aRow;
  aRow.push_back(id), aRow.push_back(pos), 
    aRow.push_back(resource), aRow.push_back(amt);
  // add the row
  trans_resource_table->addRow(aRow);
  // record this primary key
  pkref_rsrc_.push_back(id);
  pkref_rsrc_.push_back(pos);
}
