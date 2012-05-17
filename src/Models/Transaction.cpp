// Transaction.cpp

#include "Transaction.h"

#include "Model.h"
#include "MarketModel.h"
#include <string>

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
void Transaction::setCommod(std::string newCommod) {
  commod_ = newCommod;
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
void Transaction::setPrice(double newPrice) {
  price_ = newPrice;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rsrc_ptr Transaction::resource() const {
  return resource_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::setResource(rsrc_ptr newResource) {
  if (newResource.get()) {
    resource_ = newResource->clone();
  }
}

