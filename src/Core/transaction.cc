// transaction.cc

#include "transaction.h"

#include "timer.h"
#include "error.h"
#include "model.h"
#include "market_model.h"
#include "event_manager.h"
#include <string>
#include <vector>

namespace cyclus {

// initialize static variables
int Transaction::next_trans_id_ = 1;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction::Transaction(Model* creator, TransType type, Resource::Ptr res,
                         const double price, const double minfrac) : price_(price), minfrac_(minfrac) {
  type_ = type;

  this->SetResource(res);
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
  trans->SetResource(resource_);
  return trans;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::ApproveTransfer() {
  std::vector<Resource::Ptr> manifest;
  manifest = supplier_->RemoveResource(*this);
  requester_->AddResource(*this, manifest);

  // register that this transaction occured
  this->Transaction::AddTransToTable();
  int nResources = manifest.size();

  for (int pos = 0; pos < nResources; pos++) {
    // MUST PRECEDE 'addResourceToTable' call! record the resource with its state
    // because this can potentially update the material's stateID
    manifest.at(pos)->AddToTable();

    // record that what resources belong to this transaction
    this->Transaction::AddResourceToTable(pos + 1, manifest.at(pos));
  }

  CLOG(LEV_INFO3) << "Material sent from " << supplier_->ID() << " to "
                  << requester_->ID() << ".";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::MatchWith(Transaction& other) {
  if (other.type_ == type_) {
    throw ValueError("Cannot match incompatible transactino types.");
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
  MarketModel* market;
  market = MarketModel::MarketForCommod(commod_);
  return market;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Transaction::supplier() const {
  return supplier_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Transaction::requester() const {
  return requester_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Transaction::commod() const {
  return commod_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::SetCommod(std::string new_commod) {
  commod_ = new_commod;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Transaction::IsOffer() const {
  return type_ == OFFER;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Transaction::price() const {
  return price_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::SetPrice(double new_price) {
  price_ = new_price;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr Transaction::resource() const {
  return resource_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::SetResource(Resource::Ptr new_resource) {
  if (new_resource.get()) {
    resource_ = new_resource->clone();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Transaction::Minfrac() const {
  return minfrac_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::SetMinFrac(double new_minfrac) {
  minfrac_ = new_minfrac;
}

void Transaction::AddTransToTable() {
  EM->NewEvent("Transactions")
  ->AddVal("ID", trans_id_)
  ->AddVal("SenderID", supplier_->ID())
  ->AddVal("ReceiverID", requester_->ID())
  ->AddVal("MarketID", market()->ID())
  ->AddVal("Commodity", commod())
  ->AddVal("Price", price_)
  ->AddVal("Time", TI->time())
  ->Record();
}

void Transaction::AddResourceToTable(int transPos, Resource::Ptr r) {
  EM->NewEvent("TransactedResources")
  ->AddVal("TransactionID", trans_id_)
  ->AddVal("Position", transPos)
  ->AddVal("ResourceID", r->OriginalID())
  ->AddVal("StateID", r->StateID())
  ->AddVal("Quantity", r->quantity())
  ->Record();
}
} // namespace cyclus
