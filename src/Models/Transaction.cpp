// Transaction.cpp

#include "Transaction.h"

#include "Model.h"
#include "MarketModel.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction::Transaction(Model* creator, bool isoffer) {
  minfrac_ = 0;
  price_ = 0;

  supplier_ = NULL;
  requester_ = NULL;
  resource_ = NULL;

  isOffer_ = isoffer;
  if (isOffer_) {
    supplier_ = creator;
  } else {
    requester_ = creator
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction Transaction::Request(Model* requester) {
  return *(new Transaction(supplier, false));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction Transaction::Offer(Model* supplier) {
  return *(new Transaction(supplier, true));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction::~Transaction() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MarketModel* Transactoin::market() {
  MarketModel* market = MarketModel::marketForCommod(commod_);
  return market;
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Transactoin::supplier() const {
  if (supplier_ == NULL) {
    string err_msg = "Uninitilized message supplier.";
    throw CycNullMsgParamException(err_msg);
  }
  return supplier_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transactoin::setSupplier(Model* supplier) {
  supplier_ = supplier;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Transactoin::requester() const {
  if (requester_ == NULL) {
    string err_msg = "Uninitilized message requester.";
    throw CycNullMsgParamException(err_msg);
  }
  return requester_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transactoin::setRequester(Model* requester) {
  requester_ = requester;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Transactoin::commod() const {
  return commod_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transactoin::setCommod(std::string newCommod) {
  commod_ = newCommod;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Transactoin::isOffer() const {
  return isOffer_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Transactoin::price() const {
  return price_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transactoin::setPrice(double newPrice) {
  price_ = newPrice;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rsrc_ptr Transactoin::resource() const {
  return resource_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transactoin::setResource(rsrc_ptr newResource) {
  if (newResource.get()) {
    resource_ = newResource->clone();
  }
}

