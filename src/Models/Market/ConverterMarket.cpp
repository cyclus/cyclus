// ConverterMarket.cpp
// Implements the ConverterMarket class
#include <iostream>

#include "ConverterMarket.h"
#include "ConverterModel.h"

#include "Logician.h"
#include "CycException.h"
#include "InputXML.h"
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::init(xmlNodePtr cur)
{ 
  MarketModel::init(cur);  /// get facility list
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"model/ConverterMarket/converter");
  
  xmlNodePtr conv_node = nodes->nodeTab[0];
  conv_name_ = XMLinput->get_xpath_content(conv_node,"type");
  
  Model* converter = NULL; 
  converter = LI->getModelByName(conv_name_, CONVERTER);

  if (NULL == converter){
    throw CycException("Converter '" 
        + conv_name_ 
        + "' is not defined in this problem.");
    }
  Model* new_converter = Model::create(converter);

  // The commodity initialized as the mktcommodity is the request commodity.

  offer_commod_ = NULL; 

  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/ConverterMarket");

  string commod_name = XMLinput->get_xpath_content(cur,"offercommodity");
  offer_commod_ = LI->getCommodity(commod_name);
  if (NULL == offer_commod_)
    throw CycException("Offer commodity '" + commod_name 
                       + "' does not exist for converter market '" + getName() 
                       + "'.");
  
  offer_commod_->setMarket(this);
  
  commod_name = XMLinput->get_xpath_content(cur,"reqcommodity");
  req_commod_ = LI->getCommodity(commod_name);
  if (NULL == req_commod_)
    throw CycException("Request commodity '" + commod_name 
                       + "' does not exist for converter market '" + getName() 
                       + "'.");
  
  req_commod_->setMarket(this);

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::copy(ConverterMarket* src)
{ 
  MarketModel::copy(src);
  offer_commod_ = src->offer_commod_;
  offer_commod_->setMarket(this);
  req_commod_ = src->req_commod_;
  req_commod_->setMarket(this);
  conv_name_ = src->conv_name_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::copyFreshModel(Model* src)
{ 
  copy(dynamic_cast<ConverterMarket*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::print()
{ 
  MarketModel::print();
  cout << "where the offer commodity is {"
      << offer_commod_->getName()
      << "}, the request commodity is {"
      << req_commod_->getName()
      << "}. "
      << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
ConverterModel* ConverterMarket::getConverter() {
  Model* converter = NULL;
  converter = LI->getModelByName(conv_name_, CONVERTER);

  if (NULL == converter){
    throw CycException("Converter '" 
        + conv_name_ 
        + "' is not defined in this problem.");
    }

  return dynamic_cast<ConverterModel*>(converter);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void ConverterMarket::receiveMessage(Message *msg)
{
  messages_.insert(msg);

  if (msg->getAmount() > 0){
    offers_.insert(indexedMsg(msg->getAmount(),msg));
  }
  else{
    requests_.insert(indexedMsg(-msg->getAmount(),msg));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void ConverterMarket::reject_request(sortedMsgList::iterator request)
{
  // send a failure message to the facility
  //  Transaction trans;
  //  trans.amount = 0;
  //  orders.push_back(new Message(down, trans, this, 
  //                               (*request).second->getRequester()));

  // delete the tentative orders
  while ( orders_.size() > firmOrders_)
  {
    delete orders_.back();
    orders_.pop_back();
  }

  // put all matched offers back in the sorted list
  while (matchedOffers_.size() > 0)
  {
    Message *msg = *(matchedOffers_.begin());
    offers_.insert(indexedMsg(msg->getAmount(),msg));
    matchedOffers_.erase(msg);
  }

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void ConverterMarket::process_request()
{
  // update pointer to firm orders
  firmOrders_ = orders_.size();

  while (matchedOffers_.size() > 0)
  {
    Message *msg = *(matchedOffers_.begin());
    messages_.erase(msg);
    matchedOffers_.erase(msg);
  }
}
 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
bool ConverterMarket::match_request(sortedMsgList::iterator request)
{
  sortedMsgList::iterator offer;
  double requestAmt,offerAmt;
  Message *offerMsg, *requestMsg;

  requestAmt = (*request).first;
  requestMsg = (*request).second;
  
  // if this request is not yet satisfied &&
  // there are more offers_ left
  while ( requestAmt > 0 && offers_.size() > 0)
  {
    // get a new offer
    offer = offers_.end();
    offer--;
    offerMsg = (this->getConverter())->convert((*offer).second, (*request).second);
    offerAmt = offerMsg->getAmount();

    // pop off this offer
    offers_.erase(offer);
  
    if (requestAmt > offerAmt) { 
      // put a new message in the order stack
      // it goes down to supplier
      offerMsg->setRequester(requestMsg->getRequester());
      offerMsg->setDir(DOWN_MSG);

      // tenatively queue a new order (don't execute yet)
      matchedOffers_.insert(offerMsg);

      orders_.push_back(offerMsg);

      cout << "ConverterMarket has resolved a match from "
          << offerMsg->getSupplier()->getSN()
          << " to "
          << offerMsg->getRequester()->getSN()
          << " for the amount:  " 
          << offerMsg->getAmount() << endl;

      requestAmt -= offerAmt;
    } 
    else {
      // split offer

      // queue a new order
      Message* maybe_offer = new Message(*offerMsg);
      maybe_offer->setAmount(requestAmt);
      maybe_offer->setDir(DOWN_MSG);
      maybe_offer->setRequester(requestMsg->getRequester());

      matchedOffers_.insert(offerMsg);

      orders_.push_back(maybe_offer);

      cout << "ConverterMarket has resolved a partial match from "
          << maybe_offer->getSupplier()->getSN()
          << " to "
          << maybe_offer->getRequester()->getSN()
          << " for the amount:  " 
          << maybe_offer->getAmount() << endl;

      // reduce the offer amount
      offerAmt -= requestAmt;

      // if the residual is above threshold,
      // make a new offer with reduced amount

      if(offerAmt > eps){
        Message *new_offer = new Message(*offerMsg);
        new_offer->setAmount(offerAmt);
        // call this method for consistency
        receiveMessage(new_offer);
      }
      
      // zero out request
      requestAmt = 0;
    }
  }

  return (0 == requestAmt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void ConverterMarket::resolve()
{
  sortedMsgList::iterator request;

  firmOrders_ = 0;

  /// while requests_ remain and there is at least one offer left
  while (requests_.size() > 0)
  {
    request = requests_.end();
    request--;
    
    if(match_request(request)) {
      process_request();
    } 
    else {
      cout << "The request from Requester "<< 
          (*request).second->getRequester()->getSN()
          << " for the amount " << (*request).first 
          << " rejected by the ConverterMarket. "<<endl;
      reject_request(request);
    }
    // remove this request
    messages_.erase((*request).second);
    requests_.erase(request);
  }

  executeOrderQueue();
}

