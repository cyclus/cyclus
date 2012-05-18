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

// initialize static variables
int Message::next_trans_id_ = 1;
// Database table for transactions
table_ptr Message::trans_table = new Table("Transactions"); 
table_ptr Message::trans_resource_table = new Table("TransactedResources"); 

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
void Message::approveTransfer() {
  if (dead_) {
    return;
  }

  msg_ptr me = msg_ptr(this);

  vector<rsrc_ptr> manifest;
  Model* req = trans_->requester();
  Model* sup = trans_->supplier();

  try {
    manifest = sup->removeResource(me);
    req->addResource(me, manifest);
  } catch (CycException err) {
    CLOG(LEV_ERROR) << "Material transfer failed from " 
                    << sup->ID() << " to " << req->ID() << ": " << err.what();
    return;
  }

  int id = next_trans_id_++;
  
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
msg_ptr Message::partner() {
  return partner_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setPartner(msg_ptr partner) {
  partner_ = partner;
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

///////////////////////////////////////////////////////////////////////////////
////////////// Output db recording code ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


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
  data an_id(transID), a_sender( trans_->supplier()->ID() ), 
    a_receiver( trans_->requester()->ID() ), a_time( TI->time() ), 
    a_price( trans_->price() );
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

