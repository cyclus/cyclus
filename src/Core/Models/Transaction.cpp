// Transaction.cpp

#include "Transaction.h"

#include "Timer.h"
#include "Model.h"
#include "MarketModel.h"
#include "EventManager.h"
#include <string>
#include <vector>

// initialize static variables
int Transaction::next_trans_id_ = 1;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction::Transaction(Model* creator, TransType type, rsrc_ptr res, 
    const double price, const double minfrac) : price_(price), minfrac_(minfrac) { 
  type_ = type;

  this->setResource(res);
  supplier_ = NULL;
  requester_ = NULL;

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

  // register that this transaction occured
  this->Transaction::addTransToTable();
  int nResources = manifest.size();
  
  for (int pos = 0; pos < nResources; pos++) {
    // MUST PRECEDE 'addResourceToTable' call! record the resource with its state
    // because this can potentially update the material's stateID
    manifest.at(pos)->addToTable();
  
    // record that what resources belong to this transaction
    this->Transaction::addResourceToTable(pos + 1, manifest.at(pos));
  }
  
  CLOG(LEV_INFO3) << "Material sent from " << supplier_->ID() << " to " 
                  << requester_->ID() << ".";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::matchWith(Transaction& other) {
  if (other.type_ == type_) {
    throw CycTransMismatchException();
  }

  if (type_ == OFFER) {
    requester_ = other.requester();
    other.supplier_ = supplier();
  } else {
    supplier_ = other.supplier();
    other.requester_ = requester();
  }

  trans_id_ = next_trans_id_++;
  other.trans_id_ = trans_id_;
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
Model* Transaction::requester() const {
  if (requester_ == NULL) {
    std::string err_msg = "Uninitilized message requester.";
    throw CycNullMsgParamException(err_msg);
  }
  return requester_;
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Transaction::minfrac() const {
  return minfrac_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::setMinFrac(double new_minfrac) {
  minfrac_ = new_minfrac;
}

void Transaction::addTransToTable() {  
  EM->newEvent(NULL, "Transactions")
    ->addVal("ID", trans_id_)
    ->addVal("SenderID", supplier_->ID())
    ->addVal("ReceiverID", requester_->ID())
    ->addVal("MarketID", market()->ID())
    ->addVal("Commodity", commod())
    ->addVal("Price", price_)
    ->timestamp()
    ->record();
}

void Transaction::addResourceToTable(int transPos, rsrc_ptr r){  
  EM->newEvent(NULL, "TransactedResources")
    ->addVal("TransactionID", trans_id_)
    ->addVal("Position", transPos)
    ->addVal("ResourceID", r->originalID())
    ->addVal("StateID", r->stateID())
    ->addVal("Quantity", r->quantity())
    ->record();
}

