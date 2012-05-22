// Transaction.cpp

#include "Transaction.h"

#include "Timer.h"
#include "Model.h"
#include "MarketModel.h"
#include <string>
#include <vector>

// initialize static variables
int Transaction::next_trans_id_ = 1;
// Database table for transactions
table_ptr Transaction::trans_table = new Table("Transactions"); 
table_ptr Transaction::trans_resource_table = new Table("TransactedResources"); 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction::Transaction(Model* creator, TransType type) {
  minfrac = 0;
  price_ = 0;

  supplier_ = NULL;
  requester_ = NULL;
  resource_ = NULL;

  type_ = type;
  if (type == OFFER) {
    supplier_ = creator;
  } else {
    requester_ = creator;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction::~Transaction() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction* Transaction::clone() {
  // clones resource_ and gives copy to the transaction clone
  Transaction* trans = new Transaction(*this);
  trans->setResource(resource_);
  return trans;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::approveTransfer() {
  std::vector<rsrc_ptr> manifest;
  try {
    manifest = supplier_->removeResource(*this);
    requester_->addResource(*this, manifest);
  } catch (CycException err) {
    CLOG(LEV_ERROR) << "Material transfer failed from " 
                    << supplier_->ID() << " to " << requester_->ID() << ": " 
                    << err.what();
    return;
  }

  int id = next_trans_id_++;
  
  // register that this transaction occured
  this->Transaction::addTransToTable(id);
  int nResources = manifest.size();
  
  for (int pos = 0; pos < nResources; pos++) {
    // MUST PRECEDE 'addResourceToTable' call! record the resource with its state
    // because this can potentially update the material's stateID
    manifest.at(pos)->addToTable();
  
    // record that what resources belong to this transaction
    this->Transaction::addResourceToTable(id, pos + 1, manifest.at(pos));
  }
  
  CLOG(LEV_INFO3) << "Material sent from " << supplier_->ID() << " to " 
                  << requester_->ID() << ".";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::matchWith(Transaction& other) {
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MarketModel* Transaction::market() const {
  // put here to make explicit that this method throws
  MarketModel* market;
  try {
    market = MarketModel::marketForCommod(commod_);
  } catch(CycMarketlessCommodException e) {
    throw e;
  }
  return market;
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Transaction::supplier() const {
  if (supplier_ == NULL) {
    std::string err_msg = "Uninitilized message supplier.";
    throw CycNullMsgParamException(err_msg);
  }
  return supplier_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::setSupplier(Model* supplier) {
  supplier_ = supplier;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Transaction::requester() const {
  if (requester_ == NULL) {
    std::string err_msg = "Uninitilized message requester.";
    throw CycNullMsgParamException(err_msg);
  }
  return requester_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::setRequester(Model* requester) {
  requester_ = requester;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Transaction::commod() const {
  return commod_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::setCommod(std::string new_commod) {
  commod_ = new_commod;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Transaction::isOffer() const {
  return type_ == OFFER;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Transaction::price() const {
  return price_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::setPrice(double new_price) {
  price_ = new_price;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rsrc_ptr Transaction::resource() const {
  return resource_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::setResource(rsrc_ptr new_resource) {
  if (new_resource.get()) {
    resource_ = new_resource->clone();
  }
}

///////////////////////////////////////////////////////////////////////////////
////////////// Output db recording code ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void Transaction::define_trans_table(){
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

void Transaction::addTransToTable(int transID) {  
  // if we haven't logged a message yet, define the table
  if ( !trans_table->defined() )
    Transaction::define_trans_table();
  
  // make a row
  // declare data
  data an_id(transID), a_sender( supplier_->ID() ), 
    a_receiver( requester_->ID() ), a_time( TI->time() ), 
    a_price( price_ );
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

void Transaction::define_trans_resource_table(){
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

void Transaction::addResourceToTable(int transID, int transPos, rsrc_ptr r){  
  // if we haven't logged a message yet, define the table
  if ( !trans_resource_table->defined() )
    Transaction::define_trans_resource_table();
  
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

