// Message.cpp
// Implements the Message class.

#include <iostream>

#include "Message.h"

#include "Communicator.h"
#include "FacilityModel.h"
#include "MarketModel.h"
#include "InstModel.h"
#include "GenericResource.h"
#include "Logger.h"
#include "Timer.h"

using namespace std;

// initialize static variables
int Message::nextTransID_ = 1;
// Database table for transactions
table_ptr Message::trans_table = new Table("Transactions"); 
table_ptr Message::trans_resource_table = new Table("TransactedResources"); 

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
                 Transaction trans) {
  constructBase(sender);
  trans_ = trans;
  receiver_ = receiver;
  setResource(trans.resource);

  if (trans_.is_offer) {
    setSupplier(dynamic_cast<Model*>(sender_));
  } else {
    setRequester(dynamic_cast<Model*>(sender_));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::constructBase(Communicator* sender) {
  sender_ = sender;
  currentOwner_ = sender;
  receiver_ = NULL;
  dead_ = false;
  dir_ = UP_MSG;

  trans_.supplier = NULL;
  trans_.requester = NULL;
  trans_.is_offer = NULL;
  trans_.resource = NULL;
  trans_.minfrac = 0;
  trans_.price = 0;

  sender->trackMessage(msg_ptr(this));
  makeRealParticipant(sender);

  MLOG(LEV_DEBUG4) << "Message " << this << " created.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::~Message() {
  MLOG(LEV_DEBUG4) << "Message " << this << " deleted.";
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::makeRealParticipant(Communicator* who) {
  Model* model = NULL;
  model = dynamic_cast<Model*>(who);
  if (model != NULL) {model->setIsTemplate(false);}
}

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
    pathStack_.back()->untrackMessage(me);
    pathStack_.pop_back();
  } else {
    pathStack_.back()->trackMessage(me);
  }

  Communicator* nextStop = pathStack_.back();
  makeRealParticipant(nextStop);
  currentOwner_ = nextStop;

  CLOG(LEV_DEBUG1) << "Message " << this << " going to comm "
                   << nextStop << " {";

  nextStop->receiveMessage(me);

  CLOG(LEV_DEBUG1) << "} Message " << this << " send to comm "
                   << nextStop << " completed";
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
void Message::validateForSend() {
  int nextStop_index = -1;
  bool receiver_specified = false;
  Communicator* nextStop;

  if (dir_ == UP_MSG) {
    receiver_specified = (pathStack_.size() > 0);
    nextStop_index = pathStack_.size() - 1;
  } else if (dir_ == DOWN_MSG) {
    receiver_specified = (pathStack_.size() > 1);
    nextStop_index = pathStack_.size() - 2;
  }

  if (!receiver_specified) {
    throw CycNoMsgReceiverException();
  }

  nextStop = pathStack_[nextStop_index];
  if (nextStop == currentOwner_) {
    throw CycCircularMsgException();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setNextDest(Communicator* nextStop) {
  if (dir_ == UP_MSG) {
    CLOG(LEV_DEBUG4) << "Message " << this << " set next stop to comm "
                     << nextStop;
    if (pathStack_.size() == 0) {
      pathStack_.push_back(sender_);
    }
    pathStack_.push_back(nextStop);
    return;
  }
  CLOG(LEV_DEBUG4) << "Message " << this
                   << " next stop set attempt ignored";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::reverse() {
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
  CLOG(LEV_DEBUG4) << "Message " << this << " direction manually set to "
                   << newDir << ".";

  dir_ = newDir;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MarketModel* Message::market() {
  MarketModel* market = MarketModel::marketForCommod(trans_.commod);
  return market;
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
Model* Message::supplier() const {
  if (trans_.supplier == NULL) {
    string err_msg = "Uninitilized message supplier.";
    throw CycNullMsgParamException(err_msg);
  }

  return trans_.supplier;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Message::requester() const {
  if (trans_.requester == NULL) {
    string err_msg = "Uninitilized message requester.";
    throw CycNullMsgParamException(err_msg);
  }

  return trans_.requester;
}

void Message::approveTransfer() {
  if (dead_) {
    return;
  }

  msg_ptr me = msg_ptr(this);

  vector<rsrc_ptr> manifest;
  Model* req = requester();
  Model* sup = supplier();

  try {
    manifest = sup->removeResource(me);
    req->addResource(me, manifest);
  } catch (CycException err) {
    CLOG(LEV_ERROR) << "Material transfer failed from " 
                    << sup->ID() << " to " << req->ID() << ": " << err.what();
    return;
  }

  int id = nextTransID_++;
  
  // register that this transaction occured
  this->Message::addTransToTable(id);
  int nResources = manifest.size();
  
  for (int pos = 0; pos < nResources; pos++) {
    // MUST PRECEDE 'addResourceToTable' call! record the resource with its state
    // because this can potentially update the material's stateID
    manifest.at(pos)->addToTable();
  
    // record that what resources belong to this transaction
    this->Message::addResourceToTable(id, pos + 1, manifest.at(pos));
  }
  
  CLOG(LEV_INFO3) << "Material sent from " << sup->ID() << " to " 
                  << req->ID() << ".";
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

void Message::addTransToTable(int transID) {  
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
  column state("StateID","INTEGER");
  column amt("Quantity","REAL");
  // declare the table's primary key
  primary_key pk;
  pk.push_back("TransactionID"), pk.push_back("Position");
  trans_resource_table->setPrimaryKey(pk);
  // add columns to the table
  trans_resource_table->addColumn(transID), trans_resource_table->addColumn(transPos), 
    trans_resource_table->addColumn(resource), trans_resource_table->addColumn(state),
    trans_resource_table->addColumn(amt);
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
    a_resource(r->originalID()), a_state(r->stateID()), an_amt(r->quantity());
  // declare entries
  entry id("TransactionID",an_id), pos("Position",a_pos), 
    resource("ResourceID",a_resource), state("StateID", a_state), amt("Quantity",an_amt);
  // declare row
  row aRow;
  aRow.push_back(id), aRow.push_back(pos), 
    aRow.push_back(resource), aRow.push_back(state), aRow.push_back(amt);
  // add the row
  trans_resource_table->addRow(aRow);
  // record this primary key
  pkref_rsrc_.push_back(id);
  pkref_rsrc_.push_back(pos);
}

